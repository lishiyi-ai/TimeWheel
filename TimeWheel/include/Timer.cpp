#include "Timer.hpp"

namespace TimeWheel {

Timer::Timer(int delay, std::function<void()> _callback, bool _islong)
    : islong(_islong), isvalid(true), delay(delay), ticks(0), callback(_callback) {
    }

bool Timer::stop() {
    if (isvalid) {
        isvalid = false;
        return true;
    }
    return false;
}

int Timer::get_delay() const {
    return delay; // 获取延迟时间
}

void Timer::handler() {
    if (isvalid && callback) {
        callback(); // 执行回调函数
    }
}

}