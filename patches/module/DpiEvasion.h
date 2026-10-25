/*
 * DpiEvasion — adaptive DPI-evasion layer for tgnet.
 *
 * Three modes, settable at runtime from the UI (via JNI):
 *   OFF          — plain sends, stock behavior
 *   FRAG_LIGHT   — first handshake packet split into 2 + 5 byte micro-fragments
 *   FRAG_AGGRESSIVE — 1 + 1 byte fragments with longer inter-chunk delays
 *
 * Auto-escalation: after 3 consecutive handshake failures the effective mode
 * is raised to FRAG_AGGRESSIVE; after 5 consecutive successes it de-escalates
 * back to the user-selected mode.
 */
#ifndef DPIEVASION_H
#define DPIEVASION_H

#include <cstdint>
#include <cstddef>
#include <sys/types.h>

namespace tgnet {

class DpiEvasion {
public:
    enum Mode : int32_t {
        MODE_OFF = 0,
        MODE_FRAG_LIGHT = 1,
        MODE_FRAG_AGGRESSIVE = 2
    };

    struct Plan {
        size_t chunk1;
        size_t chunk2;
        uint32_t delayUs;
    };

    static void setMode(int32_t mode);
    static int32_t getMode();
    static int32_t effectiveMode();
    static bool enabled();
    static Plan planFor();

    static void reportHandshakeFailure();
    static void reportHandshakeSuccess();

private:
    static int32_t userMode;
    static int32_t failureStreak;
    static int32_t successStreak;
};

}
#endif
