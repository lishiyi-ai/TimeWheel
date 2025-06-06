#include "./include/dlist.hpp"
#include "./include/Timer.hpp"
#include "./include/TimerWheel.hpp"
#include <bits/stdc++.h>

class test{
public:
    test() {
        std::cout << "test constructor called" << std::endl;
    }

    ~test() {
        std::cout << "test destructor called" << std::endl;
    }

    void func(int& a){
        std::cout << "Value of a: " << a << std::endl;
        a += 10; // 修改传入的引用变量
        std::cout << "Modified value of a: " << a << std::endl;
    }
};


int main()
{
    int a = 10;
    test t;


    std::function<void()> func = [&]() {
        t.func(a); // 调用test类的成员函数，传入a的引用
    };

    func(); // 执行函数对象，调用test类的成员函数

    using namespace TimeWheel;
    TimerWheel timerWheel(100, 10, 10); // 创建一个时间轮，tick_duration为100ms，wheel_levels为12，wheel_size为10
    auto timer1 = std::make_shared<Timer>(100, func, 1);
    auto timer2 = std::make_shared<Timer>(10000, [&](){timerWheel.stop();}, true); // 创建一个长定时器
    timerWheel.add_timer(timer1);
    timerWheel.add_timer(timer2); // 添加定时器到时间轮
    timerWheel.start(); // 启动时间轮
    timerWheel.run(); // 运行时间轮
    
    std::cout << "a is" << a << std::endl;

    return 0;
}

