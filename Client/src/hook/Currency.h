//
// Created by kiosk on 2020/1/31.
//

#ifndef FILE_MONITOR_DEEPIN_CURRENCY_H
#define FILE_MONITOR_DEEPIN_CURRENCY_H

#include <memory>
#include <unistd.h>
#include <linux/types.h>
#include <vector>
#include <sys/fanotify.h>

enum Event : uint32_t {
    k_close_write = (1 << 3),
    k_close_nowrite = (1 << 4),
    k_open = (1 << 5),
    k_none = (1 << 12),
    k_open_pram = (1 << 16),
};

constexpr static const std::array<std::uint32_t, 7> AllFanFlags = {{
    FAN_MODIFY,
    FAN_CLOSE_WRITE,
    FAN_CLOSE_NOWRITE,
    FAN_OPEN,
    FAN_Q_OVERFLOW,
    FAN_OPEN_PERM,
    }
};

static Event GetFaniontyEvent( uint32_t e) {
    for (const auto &c :AllFanFlags) {
        if ((e & c) == c) {
            return static_cast<Event>(c);
        }
    }
    return Event ::k_none;
}

#endif //FILE_MONITOR_DEEPIN_CURRENCY_H
