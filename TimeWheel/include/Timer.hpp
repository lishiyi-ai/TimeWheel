#ifndef TIMER_HPP
#define TIMER_HPP

#include <functional>
#include <list>
#include <memory>
#include "dlist.hpp"
#include <mutex>

namespace TimeWheel {

class Timer : public std::enable_shared_from_this<Timer> {
private:
    int delay; // ms单位延迟
    std::function<void()> callback; // 回调函数
public:

    int ticks; // 换算后的实际ticks
    int islong; // 0: short, 1: long;
    bool isvalid;  // true: valid, false: invalid

    Timer(int delay, std::function<void()> callback, bool islong = false);

    // 停止定时器
    bool stop();

    // 获取定时器延迟时间
    int get_delay() const;

    // 定时轮上删除定时器
    bool delete_timer_list();

    // 删除定时轮锁
    void delete_mutex();

    void handler();

    void update_ticks(int tick_duration) {
        ticks = delay / tick_duration; // 假设每100ms为一个tick
        if(ticks == 0) {
            ticks = 1; // 至少为1个tick
        }
    }

};

}

#endif // TIMER_HPP