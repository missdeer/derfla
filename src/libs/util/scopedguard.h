#ifndef ScopedGuard_H
#define ScopedGuard_H

#include <functional>

#include "util_global.h"

class UTILSHARED_EXPORT ScopedGuard
{
public:
    explicit ScopedGuard(std::function<void()> task);

    // Use default move constructor.
    ScopedGuard(ScopedGuard &&other) noexcept;

    // Deleted copy constructor and assignment.
    ScopedGuard(const ScopedGuard&) = delete;
    ScopedGuard& operator=(const ScopedGuard&) = delete;

    ~ScopedGuard();

    void deactivate();

private:
    std::function<void()> onExitTask;
    bool isActive;
};


#endif // ScopedGuard_H
