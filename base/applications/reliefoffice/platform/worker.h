/*
 * PROJECT:     ReliefOffice
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Owned background jobs with completion and teardown synchronization
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#ifndef RELIEFOFFICE_PLATFORM_WORKER_H
#define RELIEFOFFICE_PLATFORM_WORKER_H
namespace ro {
class Worker {
    struct State;
    State *state_ = nullptr;
public:
    using Function = void (*)(void *);
    Worker() = default;
    Worker(const Worker &) = delete;
    Worker &operator=(const Worker &) = delete;
    ~Worker();
    bool start(Function, void *);
    bool finished() const;
    void wait();
};
}
#endif
