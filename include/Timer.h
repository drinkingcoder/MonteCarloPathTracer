#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include "Type.h"

// chrono timer
namespace chrono = std::chrono;
using chrono::duration_cast;
using chrono::system_clock;
using chrono::microseconds;
using time_point = system_clock::time_point;
typedef chrono::duration<double, std::micro> microseconds_duration;

class timer {
    microseconds_duration m_duration;
    time_point m_start_point, m_stop_point;
    bool m_counting;
public:
    timer():m_duration(microseconds_duration::zero()), m_counting(false) {
    }

    inline void start() {
        m_start_point = system_clock::now();
        m_counting = true;
    }

    inline void stop() {
        m_stop_point = system_clock::now();
        if (!m_counting) {
            std::cout << "Timer stopped before started." << std::endl;
            exit(1);
        }
        m_counting = false;
        m_duration += duration_cast<microseconds>(m_stop_point - m_start_point);
    }

    inline double get_microseconds() {
        return m_duration.count();
    }

    inline double get_seconds() {
        return m_duration.count() * microseconds::period::num / microseconds::period::den;
    }

    inline void reset() {
        m_duration = microseconds_duration::zero();
    }
};

enum TIMER_TYPE {
    GLOBAL_TIMER,
    KDTREE_INTERSECTION_TIMER,
    TRIANGLE_INTERSECTION_TIMER,
    TIMER_NUM
};

class global_timer {
public:
    static std::vector<timer> timers;
    static void initialize() {
        timers.resize(TIMER_NUM);
    }
};

#endif
