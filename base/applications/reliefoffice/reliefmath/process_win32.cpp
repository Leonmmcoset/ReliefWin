/*
 * PROJECT:     ReliefMath
 * LICENSE:     Apache-2.0 (https://spdx.org/licenses/Apache-2.0)
 * PURPOSE:     Asynchronous Win32 renderer process and bounded protocol transport
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include "process.h"

#include "protocol.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

namespace rm {
namespace {

constexpr size_t kWorkerPathBytes = 4096;
constexpr size_t kPumpIoBytes = 16384;
constexpr uint64_t kRequestTimeoutMs = 10000;
constexpr uint64_t kRestartWindowMs = 30000;
constexpr unsigned kMaxAutomaticRestarts = 2;

struct ClientState {
    char worker_path[kWorkerPathBytes]{};
    bool configured = false;
    HANDLE child = nullptr;
    uint64_t child_request_id = 0;
    uint64_t child_formula_id = 0;
    uint64_t child_generation = 0;
    HANDLE input_fd = nullptr;
    OVERLAPPED input_operation{};
    bool input_pending = false;
    HANDLE output_fd = nullptr;
    bool child_reaped = false;
    bool child_status_known = false;
    int child_status = 0;
    bool intentional_stop = false;

    bool active = false;
    Snapshot snapshot;
    Bytes request_wire;
    size_t request_offset = 0;
    bool request_sent = false;
    uint64_t started_ms = 0;

    Decoder decoder;
    Rendered candidate;
    ArtifactInfo current_artifact;
    bool artifact_active = false;
    uint64_t artifact_received = 0;
    uint8_t artifacts_received = 0;
    double baseline = 0;

    bool result_ready = false;
    Error result_error;
    uint64_t restart_times[kMaxAutomaticRestarts]{};
    unsigned restart_count = 0;
};

void set_error(Error &error, Code code, const char *message,
               size_t begin = 0, size_t end = 0) noexcept {
    error.set(code, message, begin, end);
}

uint64_t monotonic_ms() noexcept {
    return GetTickCount64();
}

void close_fd(HANDLE &handle) noexcept {
    if (handle && handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    handle = nullptr;
}

void drain_write(ClientState &state) noexcept {
    if (state.input_pending) {
        CancelIoEx(state.input_fd, &state.input_operation);
        DWORD written = 0;
        GetOverlappedResult(state.input_fd, &state.input_operation, &written, TRUE);
        state.input_pending = false;
    }
    close_fd(state.input_operation.hEvent);
}

bool append_frame(Bytes &stream, const Frame &frame, Error &error) noexcept {
    Bytes encoded;
    if (!encode_frame(frame, encoded, error)) return false;
    if (!stream.append(encoded.data(), encoded.size())) {
        set_error(error, Code::NoMemory, "Cannot queue render request");
        return false;
    }
    return true;
}

bool make_request(const Snapshot &snapshot, Bytes &wire, Error &error) noexcept {
    wire.clear();
    Frame begin;
    begin.kind = Kind::Begin;
    begin.request_id = snapshot.request_id;
    begin.formula_id = snapshot.formula.id;
    begin.generation = snapshot.formula.generation;
    if (!encode_begin_payload(snapshot, begin.payload, error) || !append_frame(wire, begin, error))
        return false;

    if (!snapshot.formula.source.empty()) {
        Frame source;
        source.kind = Kind::SourceChunk;
        source.request_id = snapshot.request_id;
        source.formula_id = snapshot.formula.id;
        source.generation = snapshot.formula.generation;
        if (!source.payload.append(snapshot.formula.source.c_str(), snapshot.formula.source.size())) {
            set_error(error, Code::NoMemory, "Cannot copy source chunk");
            return false;
        }
        if (!append_frame(wire, source, error)) return false;
    }

    Frame end;
    end.kind = Kind::End;
    end.request_id = snapshot.request_id;
    end.formula_id = snapshot.formula.id;
    end.generation = snapshot.formula.generation;
    return append_frame(wire, end, error);
}

ClientState *state_of(void *state) noexcept {
    return static_cast<ClientState *>(state);
}

void retry_or_fail(ClientState &state) noexcept;

void finish_reaped_worker(ClientState &state) noexcept {
    const bool intentional = state.intentional_stop;
    const bool same_request = state.active &&
        state.child_request_id == state.snapshot.request_id &&
        state.child_formula_id == state.snapshot.formula.id &&
        state.child_generation == state.snapshot.formula.generation;
    if (same_request && !intentional) retry_or_fail(state);
    state.child_request_id = 0;
    state.child_formula_id = 0;
    state.child_generation = 0;
    state.child_reaped = false;
    state.child_status_known = false;
    state.child_status = 0;
    state.intentional_stop = false;
}

void make_identity_result(ClientState &state) noexcept {
    Rendered result;
    result.snapshot = state.snapshot;
    result.formula_id = state.snapshot.formula.id;
    result.request_id = state.snapshot.request_id;
    result.generation = state.snapshot.formula.generation;
    state.candidate = static_cast<Rendered &&>(result);
}

void clear_request(ClientState &state) noexcept {
    state.active = false;
    state.request_wire.clear();
    state.request_offset = 0;
    state.request_sent = false;
    state.started_ms = 0;
    state.artifact_active = false;
    state.artifact_received = 0;
    state.artifacts_received = 0;
    state.baseline = 0;
}

void complete_failure(ClientState &state, const Error &failure) noexcept {
    make_identity_result(state);
    state.result_error = failure;
    state.result_ready = true;
    clear_request(state);
}

void complete_success(ClientState &state) noexcept {
    state.candidate.baseline = state.baseline;
    state.result_error.clear();
    state.result_ready = true;
    state.active = false;
    state.request_wire.clear();
    state.request_offset = 0;
    state.request_sent = false;
    state.started_ms = 0;
    state.artifact_active = false;
    state.artifact_received = 0;
    state.artifacts_received = 0;
}

void request_worker_stop(ClientState &state) noexcept {
    if (state.child != nullptr) {
        state.intentional_stop = true;
        (void)TerminateProcess(state.child, ERROR_CANCELLED);
    } else if (state.child_reaped) {
        state.intentional_stop = true;
    }
    drain_write(state);
    close_fd(state.input_fd);
    close_fd(state.output_fd);
    if (state.child == nullptr && state.child_reaped) finish_reaped_worker(state);
}

void reap_nonblocking(ClientState &state) noexcept {
    if (!state.child || WaitForSingleObject(state.child, 0) != WAIT_OBJECT_0) return;
    DWORD status = 0;
    state.child_status_known = GetExitCodeProcess(state.child, &status) != FALSE;
    state.child_status = static_cast<int>(status);
    close_fd(state.child);
    state.child_reaped = true;
    drain_write(state);
    close_fd(state.input_fd);
    if (!state.output_fd) finish_reaped_worker(state);
}

bool spawn_worker(ClientState &state, Error &error) noexcept {
    SECURITY_ATTRIBUTES security{sizeof(security), nullptr, TRUE};
    HANDLE child_input = nullptr, child_output = nullptr, child_error = nullptr;
    HANDLE input = nullptr, output = nullptr;
    wchar_t executable[kWorkerPathBytes];
    wchar_t command[kWorkerPathBytes + 24];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, state.worker_path, -1,
                             executable, kWorkerPathBytes) ||
        wcschr(executable, L'"')) {
        set_error(error, Code::Format, "Renderer path is not valid UTF-8");
        return false;
    }
    static LONG sequence = 0;
    wchar_t name[128];
    _snwprintf(name, 128, L"\\\\.\\pipe\\ReliefMath-%lu-%ld", GetCurrentProcessId(),
               InterlockedIncrement(&sequence));
    input = CreateNamedPipeW(name, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE,
                            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1,
                            kPumpIoBytes, kPumpIoBytes, 0, nullptr);
    if (input == INVALID_HANDLE_VALUE) input = nullptr;
    if (input) child_input = CreateFileW(name, GENERIC_READ, 0, &security, OPEN_EXISTING, 0, nullptr);
    if (child_input == INVALID_HANDLE_VALUE) child_input = nullptr;
    if (input && child_input) {
        OVERLAPPED connection{};
        connection.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        BOOL connected = connection.hEvent && ConnectNamedPipe(input, &connection);
        DWORD failure = connected ? ERROR_SUCCESS : GetLastError();
        if (!connected && failure == ERROR_IO_PENDING) {
            DWORD ignored;
            connected = GetOverlappedResult(input, &connection, &ignored, TRUE);
        } else if (failure == ERROR_PIPE_CONNECTED) connected = TRUE;
        close_fd(connection.hEvent);
        if (!connected) close_fd(child_input);
    }
    bool okay = input && child_input &&
                CreatePipe(&output, &child_output, &security, kPumpIoBytes) &&
                SetHandleInformation(output, HANDLE_FLAG_INHERIT, 0);
    if (okay) child_error = CreateFileW(L"NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      &security, OPEN_EXISTING, 0, nullptr);
    if (child_error == INVALID_HANDLE_VALUE) child_error = nullptr;
    okay = okay && child_error;
    SIZE_T list_bytes = 0;
    InitializeProcThreadAttributeList(nullptr, 1, 0, &list_bytes);
    auto *attributes = static_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(malloc(list_bytes));
    bool list_ready = attributes && InitializeProcThreadAttributeList(attributes, 1, 0, &list_bytes);
    HANDLE inherited[] = {child_input, child_output, child_error};
    okay = okay && list_ready && UpdateProcThreadAttribute(attributes, 0,
        PROC_THREAD_ATTRIBUTE_HANDLE_LIST, inherited, sizeof(inherited), nullptr, nullptr);
    STARTUPINFOEXW startup{};
    startup.StartupInfo.cb = sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput = child_input;
    startup.StartupInfo.hStdOutput = child_output;
    startup.StartupInfo.hStdError = child_error;
    startup.lpAttributeList = attributes;
    PROCESS_INFORMATION process{};
    _snwprintf(command, kWorkerPathBytes + 24, L"\"%ls\" --render-worker", executable);
    if (okay) okay = CreateProcessW(executable, command, nullptr, nullptr, TRUE,
        CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &startup.StartupInfo, &process);
    if (list_ready) DeleteProcThreadAttributeList(attributes);
    free(attributes);
    close_fd(child_input); close_fd(child_output); close_fd(child_error);
    if (!okay) {
        close_fd(input); close_fd(output);
        set_error(error, Code::Io, "Cannot start renderer worker");
        return false;
    }
    close_fd(process.hThread);
    state.child = process.hProcess;
    state.child_request_id = state.active ? state.snapshot.request_id : 0;
    state.child_formula_id = state.active ? state.snapshot.formula.id : 0;
    state.child_generation = state.active ? state.snapshot.formula.generation : 0;
    state.child_reaped = false;
    state.child_status_known = false;
    state.child_status = 0;
    state.input_fd = input;
    state.output_fd = output;
    state.input_operation = OVERLAPPED{};
    state.input_operation.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    state.input_pending = false;
    state.intentional_stop = false;
    if (!state.input_operation.hEvent) {
        request_worker_stop(state);
        set_error(error, Code::Io, "Cannot create renderer I/O event");
        return false;
    }
    return true;
}

uint8_t artifact_bit(ArtifactType type) noexcept {
    switch (type) {
        case ArtifactType::Rgba: return WantRaster;
        case ArtifactType::Svg: return WantSvg;
        case ArtifactType::Png: return WantPng;
    }
    return 0;
}

Bytes *artifact_bytes(Rendered &rendered, ArtifactType type) noexcept {
    switch (type) {
        case ArtifactType::Rgba: return &rendered.raster.rgba;
        case ArtifactType::Svg: return &rendered.svg;
        case ArtifactType::Png: return &rendered.png;
    }
    return nullptr;
}

void protocol_failure(ClientState &state, const char *message) noexcept {
    Error failure;
    set_error(failure, Code::Protocol, message);
    state.active = false;
    request_worker_stop(state);
    complete_failure(state, failure);
}

bool matches_request(const Frame &frame, const ClientState &state) noexcept {
    return frame.request_id == state.snapshot.request_id &&
           frame.formula_id == state.snapshot.formula.id &&
           frame.generation == state.snapshot.formula.generation;
}

bool handle_frame(ClientState &state, Frame &frame) noexcept {
    if (!state.active || !state.request_sent || !matches_request(frame, state)) {
        protocol_failure(state, "Renderer response identity or sequence mismatch");
        return false;
    }
    switch (frame.kind) {
        case Kind::ArtifactBegin: {
            if (state.artifact_active) {
                protocol_failure(state, "Nested ArtifactBegin frame");
                return false;
            }
            ArtifactInfo info;
            Error error;
            if (!decode_artifact_info(frame, info, error)) {
                protocol_failure(state, error.message.c_str());
                return false;
            }
            const uint8_t bit = artifact_bit(info.type);
            if (!bit || !(state.snapshot.artifacts & bit) || (state.artifacts_received & bit)) {
                protocol_failure(state, "Renderer declared an unrequested or duplicate artifact");
                return false;
            }
            Bytes *storage = artifact_bytes(state.candidate, info.type);
            if (!storage || !storage->resize(static_cast<size_t>(info.bytes))) {
                Error failure;
                set_error(failure, Code::NoMemory, "Cannot allocate declared render artifact");
                state.active = false;
                request_worker_stop(state);
                complete_failure(state, failure);
                return false;
            }
            if (info.type == ArtifactType::Rgba) {
                state.candidate.raster.width = info.width;
                state.candidate.raster.height = info.height;
                state.candidate.raster.stride = info.stride;
            }
            state.current_artifact = info;
            state.artifact_received = 0;
            state.artifact_active = true;
            return true;
        }
        case Kind::ArtifactChunk: {
            if (!state.artifact_active || !frame.payload.size() ||
                frame.payload.size() > state.current_artifact.bytes - state.artifact_received) {
                protocol_failure(state, "ArtifactChunk has no matching bounded artifact");
                return false;
            }
            Bytes *storage = artifact_bytes(state.candidate, state.current_artifact.type);
            if (!storage) {
                protocol_failure(state, "ArtifactChunk type is invalid");
                return false;
            }
            memcpy(storage->data() + static_cast<size_t>(state.artifact_received),
                   frame.payload.data(), frame.payload.size());
            state.artifact_received += frame.payload.size();
            return true;
        }
        case Kind::ArtifactEnd:
            if (!state.artifact_active || state.artifact_received != state.current_artifact.bytes ||
                frame.payload.size()) {
                protocol_failure(state, "ArtifactEnd arrived before the declared bytes");
                return false;
            }
            state.artifacts_received |= artifact_bit(state.current_artifact.type);
            state.artifact_active = false;
            state.artifact_received = 0;
            return true;
        case Kind::Done: {
            if (state.artifact_active || state.artifacts_received != state.snapshot.artifacts) {
                protocol_failure(state, "Done arrived before all requested artifacts");
                return false;
            }
            Error error;
            if (!decode_done_payload(frame, state.baseline, error)) {
                protocol_failure(state, error.message.c_str());
                return false;
            }
            complete_success(state);
            return true;
        }
        case Kind::Failure: {
            if (state.artifact_active || state.artifacts_received) {
                protocol_failure(state, "Failure interrupted an artifact stream");
                return false;
            }
            Error failure;
            Error decode_error;
            if (!decode_failure_payload(frame, state.snapshot.formula.source.size(), failure, decode_error)) {
                protocol_failure(state, decode_error.message.c_str());
                return false;
            }
            complete_failure(state, failure);
            return true;
        }
        default:
            protocol_failure(state, "Unexpected frame from renderer worker");
            return false;
    }
}

bool record_restart(ClientState &state, uint64_t now) noexcept {
    if (state.restart_count && now >= state.restart_times[0] &&
        now - state.restart_times[0] >= kRestartWindowMs) {
        if (state.restart_count > 1) state.restart_times[0] = state.restart_times[1];
        --state.restart_count;
    }
    if (state.restart_count >= kMaxAutomaticRestarts) return false;
    state.restart_times[state.restart_count++] = now;
    return true;
}

void retry_or_fail(ClientState &state) noexcept {
    const uint64_t now = monotonic_ms();
    if (!record_restart(state, now)) {
        Error failure;
        char message[128];
        if (state.child_status_known)
            (void)snprintf(message, sizeof(message),
                "Renderer worker exited with status %d; submit again to retry", state.child_status);
        else
            (void)snprintf(message, sizeof(message),
                "Renderer worker disappeared; submit again to retry");
        set_error(failure, Code::Io, message);
        complete_failure(state, failure);
        return;
    }
    state.request_offset = 0;
    state.request_sent = false;
    state.decoder.reset();
    state.artifact_active = false;
    state.artifact_received = 0;
    state.artifacts_received = 0;
    state.baseline = 0;
    state.candidate = Rendered();
    state.candidate.snapshot = state.snapshot;
    state.candidate.formula_id = state.snapshot.formula.id;
    state.candidate.request_id = state.snapshot.request_id;
    state.candidate.generation = state.snapshot.formula.generation;
}

void read_worker(ClientState &state) noexcept {
    uint8_t buffer[kPumpIoBytes];
    DWORD available = 0;
    if (!PeekNamedPipe(state.output_fd, nullptr, 0, nullptr, &available, nullptr)) {
        available = 0;
        if (GetLastError() != ERROR_BROKEN_PIPE) return;
    } else if (!available) return;
    DWORD bytes = 0;
    BOOL read_ok = available && ReadFile(state.output_fd, buffer,
        available < sizeof(buffer) ? available : DWORD(sizeof(buffer)), &bytes, nullptr);
    const long received = read_ok ? static_cast<long>(bytes) : 0;
    if (received > 0) {
        if (!state.active) {
            if (state.result_ready && state.snapshot.request_id)
                protocol_failure(state, "Renderer emitted data after request completion");
            else
                request_worker_stop(state);
            return;
        }
        Error error;
        if (!state.decoder.feed(buffer, static_cast<size_t>(received), error)) {
            protocol_failure(state, error.message.c_str());
            return;
        }
        Frame frame;
        while (state.decoder.take(frame)) {
            if (!handle_frame(state, frame)) return;
        }
        if (!state.active && state.result_ready && state.decoder.has_partial_frame())
            protocol_failure(state, "Trailing partial frame after render completion");
        return;
    }
    if (received < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) return;
    if (received < 0) {
        close_fd(state.output_fd);
        drain_write(state);
        close_fd(state.input_fd);
        if (state.child != nullptr) (void)TerminateProcess(state.child, ERROR_CANCELLED);
        if (state.child_reaped) finish_reaped_worker(state);
        return;
    }

    if (state.active) {
        if (state.artifact_active) {
            protocol_failure(state, "EOF before the declared artifact completed");
            return;
        }
        Error error;
        if (!state.decoder.finish(error)) {
            protocol_failure(state, error.message.c_str());
            return;
        }
    }
    close_fd(state.output_fd);
    if (state.child_reaped) finish_reaped_worker(state);
    else if (state.child != nullptr) {
        drain_write(state);
        close_fd(state.input_fd);
        if (!state.active) state.intentional_stop = true;
        (void)TerminateProcess(state.child, ERROR_CANCELLED);
    }
}

void write_worker(ClientState &state) noexcept {
    if (state.input_fd == nullptr || !state.active || state.request_offset >= state.request_wire.size()) return;
    size_t count = state.request_wire.size() - state.request_offset;
    if (count > kPumpIoBytes) count = kPumpIoBytes;
    DWORD written = 0;
    if (state.input_pending) {
        if (!GetOverlappedResult(state.input_fd, &state.input_operation, &written, FALSE)) {
            if (GetLastError() == ERROR_IO_INCOMPLETE) return;
        } else {
            state.input_pending = false;
            state.request_offset += written;
            if (state.request_offset == state.request_wire.size()) state.request_sent = true;
            return;
        }
        state.input_pending = false;
    } else {
        ResetEvent(state.input_operation.hEvent);
        if (WriteFile(state.input_fd, state.request_wire.data() + state.request_offset,
                      static_cast<DWORD>(count), &written, &state.input_operation)) {
            state.request_offset += written;
            if (state.request_offset == state.request_wire.size()) state.request_sent = true;
            return;
        }
        if (GetLastError() == ERROR_IO_PENDING) {
            state.input_pending = true;
            return;
        }
    }
    if (state.active) retry_or_fail(state);
    request_worker_stop(state);
}

} // namespace

RenderClient::~RenderClient() {
    ClientState *state = state_of(state_);
    if (!state) return;
    request_worker_stop(*state);
    if (state->child) {
        WaitForSingleObject(state->child, INFINITE);
        close_fd(state->child);
    }
    state->~ClientState();
    free(state);
    state_ = nullptr;
}

bool RenderClient::start(const char *worker_path, Error &error) noexcept {
    error.clear();
    if (!worker_path || !worker_path[0]) {
        set_error(error, Code::Format, "Renderer worker path is empty");
        return false;
    }
    size_t length = 0;
    while (length < kWorkerPathBytes && worker_path[length]) ++length;
    if (length == kWorkerPathBytes) {
        set_error(error, Code::Limit, "Renderer worker path is too long");
        return false;
    }
    ClientState *state = state_of(state_);
    if (!state) {
        void *memory = detail::allocate(sizeof(ClientState));
        if (!memory) {
            set_error(error, Code::NoMemory, "Cannot allocate renderer client");
            return false;
        }
        state = new (memory, Placement{}) ClientState();
        state_ = state;
    }
    if (state->active) {
        set_error(error, Code::Conflict, "Cannot change worker path during a render");
        return false;
    }
    const bool same_path = state->configured && strcmp(state->worker_path, worker_path) == 0;
    if (same_path && (state->child != nullptr || state->output_fd != nullptr)) return true;
    if (!same_path && (state->child != nullptr || state->child_reaped || state->output_fd != nullptr)) {
        set_error(error, Code::Conflict, "Cannot change worker path while its child is being reaped");
        return false;
    }
    if (state->child_reaped) finish_reaped_worker(*state);
    memcpy(state->worker_path, worker_path, length + 1);
    state->configured = true;
    return spawn_worker(*state, error);
}

bool RenderClient::submit(const Snapshot &snapshot, Error &error) noexcept {
    error.clear();
    ClientState *state = state_of(state_);
    if (!state || !state->configured) {
        set_error(error, Code::Conflict, "Renderer worker path has not been configured");
        return false;
    }

    Bytes request;
    if (!make_request(snapshot, request, error)) return false;

    Snapshot retained = snapshot;
    const bool replacing_active = state->active;
    if (replacing_active) request_worker_stop(*state);
    else if (state->child != nullptr) {
        state->child_request_id = snapshot.request_id;
        state->child_formula_id = snapshot.formula.id;
        state->child_generation = snapshot.formula.generation;
    }
    state->result_ready = false;
    state->result_error.clear();
    state->snapshot = static_cast<Snapshot &&>(retained);
    state->request_wire = static_cast<Bytes &&>(request);
    state->request_offset = 0;
    state->request_sent = false;
    state->started_ms = monotonic_ms();
    state->decoder.reset();
    state->candidate = Rendered();
    state->candidate.snapshot = state->snapshot;
    state->candidate.formula_id = snapshot.formula.id;
    state->candidate.request_id = snapshot.request_id;
    state->candidate.generation = snapshot.formula.generation;
    state->artifact_active = false;
    state->artifact_received = 0;
    state->artifacts_received = 0;
    state->baseline = 0;
    state->active = true;
    return true;
}

void RenderClient::pump() noexcept {
    ClientState *state = state_of(state_);
    if (!state) return;

    reap_nonblocking(*state);
    const uint64_t now = monotonic_ms();
    if (state->active && now >= state->started_ms &&
        now - state->started_ms >= kRequestTimeoutMs) {
        Error failure;
        set_error(failure, Code::Timeout, "Renderer request exceeded 10 seconds");
        state->active = false;
        request_worker_stop(*state);
        complete_failure(*state, failure);
    }

    if (state->active && state->child == nullptr && state->output_fd == nullptr) {
        Error failure;
        if (!spawn_worker(*state, failure)) {
            complete_failure(*state, failure);
            return;
        }
    }
    if (state->child == nullptr && state->output_fd == nullptr) return;

    if (state->input_fd && state->active) write_worker(*state);
    if (state->output_fd) read_worker(*state);
    reap_nonblocking(*state);
    if (state->active && state->child == nullptr && state->output_fd == nullptr) {
        Error failure;
        if (!spawn_worker(*state, failure)) complete_failure(*state, failure);
    }
}

bool RenderClient::take(Rendered &rendered, Error &error) noexcept {
    ClientState *state = state_of(state_);
    if (!state || !state->result_ready) return false;
    rendered = static_cast<Rendered &&>(state->candidate);
    error = state->result_error;
    state->candidate = Rendered();
    state->result_error.clear();
    state->result_ready = false;
    if (!state->active) {
        state->snapshot = Snapshot();
        state->decoder.reset();
    }
    return true;
}

bool RenderClient::busy() const noexcept {
    const ClientState *state = state_of(state_);
    return state && state->active;
}

void RenderClient::cancel() noexcept {
    ClientState *state = state_of(state_);
    if (!state || !state->active) return;
    Error failure;
    set_error(failure, Code::Cancelled, "Render request cancelled");
    state->active = false;
    request_worker_stop(*state);
    complete_failure(*state, failure);
    reap_nonblocking(*state);
}

} // namespace rm
