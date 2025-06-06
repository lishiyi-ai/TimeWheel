#include "Timer.hpp"

namespace TimeWheel {

Timer::Timer(int delay, std::function<void()> _callback, bool _islong)
    : islong(_islong), isvalid(true), delay(delay), ticks(0), callback(_callback) {
        mutex = nullptr;
        timer_list = nullptr;
    }

bool Timer::delete_timer_list() {
    if (!mutex) {
        return false; // 如果互斥锁已过期，直接返回
    }
    
    if (isvalid) {
        isvalid = false;
        std::lock_guard<std::mutex> lock(*mutex); // 确保线程安全
        if (timer_list) {
            timer_list->delete_node(shared_from_this()); // 从定时轮中移除
            timer_list = nullptr; // 清空定时轮指针
        }
        return true;
    }
    return false;
}
void Timer::delete_mutex() {
    mutex = nullptr; // 清空互斥锁指针
}

bool Timer::stop() {
    return delete_timer_list(); // 删除定时轮
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