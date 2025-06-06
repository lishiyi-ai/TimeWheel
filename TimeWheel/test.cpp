#include "./include/dlist.hpp"
#include "./include/Timer.hpp"
#include "./include/TimerWheel.hpp"
#include <bits/stdc++.h>


int main() {
    using namespace TimeWheel;
    TimerWheel timerWheel(100, 12, 10); // 创建一个时间轮，tick_duration为100ms，wheel_levels为12，wheel_size为10
    // 获取当前时间单位ms
    int time = std::chrono::system_clock::now().time_since_epoch().count() / 1000000; // 转换为毫秒
    auto timer1 = std::make_shared<Timer>(500, [&]() {
        int t = std::chrono::system_clock::now().time_since_epoch().count() / 1000000; // 获取当前时间单位ms
        
        std::cout << t - time << std::endl;
        
        time = t;
    }, 1);

    auto timer2 = std::make_shared<Timer>(1000, [&]() {
        timer1->stop(); // 停止定时器1
        std::cout << "Stopping TimerWheel..." << std::endl;
    }, true); // 创建一个长定时器

    timerWheel.add_timer(timer1);
    timerWheel.add_timer(timer2); // 添加定时器到时间轮
    timerWheel.start(); // 启动时间轮
    timerWheel.run(); // 运行时间轮
    return 0;
}
