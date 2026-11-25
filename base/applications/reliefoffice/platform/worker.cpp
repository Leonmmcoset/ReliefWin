/*
 * PROJECT:     ReliefOffice
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Joinable file and image encoding jobs
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include "worker.h"
#include <stdlib.h>
#include <errno.h>
#include <new>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <atomic>
#endif
namespace ro {
struct Worker::State {
    Function function;
    void *context;
#ifdef _WIN32
    HANDLE thread;
    static unsigned __stdcall run(void *opaque) {
        State *state = static_cast<State *>(opaque);
        state->function(state->context);
        return 0;
    }
#else
    pthread_t thread;
    std::atomic<bool> done{false};
    static void *run(void *opaque) {
        State *state = static_cast<State *>(opaque);
        state->function(state->context);
        state->done.store(true, std::memory_order_release);
        return nullptr;
    }
#endif
};
Worker::~Worker() { wait(); }
bool Worker::start(Function function, void *context) {
    if (state_ || !function) { errno = EINVAL; return false; }
    State *state = new (std::nothrow) State;
    if (!state) { errno = ENOMEM; return false; }
    state->function = function;
    state->context = context;
#ifdef _WIN32
    state->thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, State::run, state, 0, nullptr));
    if (!state->thread) { delete state; errno = EAGAIN; return false; }
#else
    int result = pthread_create(&state->thread, nullptr, State::run, state);
    if (result) { delete state; errno = result; return false; }
#endif
    state_ = state;
    return true;
}
bool Worker::finished() const {
    if (!state_) return true;
#ifdef _WIN32
    return WaitForSingleObject(state_->thread, 0) == WAIT_OBJECT_0;
#else
    return state_->done.load(std::memory_order_acquire);
#endif
}
void Worker::wait() {
    if (!state_) return;
#ifdef _WIN32
    WaitForSingleObject(state_->thread, INFINITE);
    CloseHandle(state_->thread);
#else
    pthread_join(state_->thread, nullptr);
#endif
    delete state_;
    state_ = nullptr;
}
}
