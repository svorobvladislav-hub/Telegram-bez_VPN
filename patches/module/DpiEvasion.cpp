#include "DpiEvasion.h"
#include <mutex>

namespace tgnet {

static std::mutex g_dpiMutex;

int32_t DpiEvasion::userMode = DpiEvasion::MODE_FRAG_LIGHT;
int32_t DpiEvasion::failureStreak = 0;
int32_t DpiEvasion::successStreak = 0;

void DpiEvasion::setMode(int32_t mode) {
    std::lock_guard<std::mutex> guard(g_dpiMutex);
    userMode = (mode >= MODE_OFF && mode <= MODE_FRAG_AGGRESSIVE) ? mode : MODE_FRAG_LIGHT;
    failureStreak = 0;
    successStreak = 0;
}

int32_t DpiEvasion::getMode() {
    std::lock_guard<std::mutex> guard(g_dpiMutex);
    return userMode;
}

int32_t DpiEvasion::effectiveMode() {
    std::lock_guard<std::mutex> guard(g_dpiMutex);
    if (userMode == MODE_OFF) {
        return MODE_OFF;
    }
    if (failureStreak >= 3 && userMode < MODE_FRAG_AGGRESSIVE) {
        return MODE_FRAG_AGGRESSIVE;
    }
    return userMode;
}

bool DpiEvasion::enabled() {
    return effectiveMode() != MODE_OFF;
}

DpiEvasion::Plan DpiEvasion::planFor() {
    std::lock_guard<std::mutex> guard(g_dpiMutex);
    Plan plan{2, 5, 5000};
    switch (effectiveMode()) {
        case MODE_OFF:
            plan.chunk1 = 0;
            plan.chunk2 = 0;
            plan.delayUs = 0;
            break;
        case MODE_FRAG_AGGRESSIVE:
            plan.chunk1 = 1;
            plan.chunk2 = 1;
            plan.delayUs = 20000;
            break;
        case MODE_FRAG_LIGHT:
        default:
            plan.chunk1 = 2;
            plan.chunk2 = 5;
            plan.delayUs = 5000;
            break;
    }
    return plan;
}

void DpiEvasion::reportHandshakeFailure() {
    std::lock_guard<std::mutex> guard(g_dpiMutex);
    failureStreak++;
    successStreak = 0;
}

void DpiEvasion::reportHandshakeSuccess() {
    std::lock_guard<std::mutex> guard(g_dpiMutex);
    successStreak++;
    if (successStreak >= 5) {
        successStreak = 0;
        if (failureStreak > 0) {
            failureStreak--;
        }
    }
}

}
