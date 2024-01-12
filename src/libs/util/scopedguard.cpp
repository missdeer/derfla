#include "stdafx.h"

#include "scopedguard.h"

ScopedGuard::ScopedGuard(std::function<void()> task) : onExitTask(std::move(task)), isActive(true) {}

ScopedGuard::ScopedGuard(ScopedGuard &&other) noexcept : onExitTask(std::move(other.onExitTask)), isActive(other.isActive)
{
    other.isActive = false;
}

ScopedGuard::~ScopedGuard()
{
    // If active, execute task.
    if (isActive)
    {
        onExitTask();
    }
}

void ScopedGuard::deactivate()
{
    isActive = false;
}
