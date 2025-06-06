#ifndef TIMERWHEEL_HPP
#define TIMERWHEEL_HPP

#include <mutex>
#include <list>
#include "Timer.hpp"
#include <vector>
#include "dlist.hpp"
#include <thread>
#include <memory>

namespace TimeWheel {

class TimerWheel {
private:
    int tick_duration; // 一次心跳间隔
    int wheel_levels; // 时间级数
    int wheel_size; // 时间轮槽数
    int runwheel_loc = 0; // 当前执行时间轮位置
    bool isrunning = false; // 是否正在运行
    std::mutex queue_mutex; // 任务队列

    std::vector<int> wheel_level_loc;
    std::vector<std::mutex>* wheel_mutex; // 时间轮锁
    std::vector<std::vector<DList<Timer>>> multiwheel; // 多级时间轮 levels * wheel_size
    DList<Timer> task_queue; // 任务队列
    std::thread threadtime; // 线程对象

public:
    ~TimerWheel() {
        delete wheel_mutex; // 删除时间轮锁
    }
    // 构造函数，初始化时间轮
    TimerWheel(int tick_duration = 15, int wheel_levels = 12, int wheel_size = 10)
        : tick_duration(tick_duration), wheel_levels(wheel_levels), wheel_size(wheel_size) {
        
        multiwheel.resize(wheel_levels);
        wheel_mutex = new std::vector<std::mutex>(wheel_levels); // 初始化时间轮锁
        wheel_level_loc.resize(wheel_levels);
        runwheel_loc = 0; // 初始化当前执行时间轮位置

        for (int i = 0; i < wheel_levels; ++i) {
            wheel_level_loc[i] = 0;
            multiwheel[i].resize(wheel_size);
        }
    }

    // 时间轮心跳函数
    void Tick(){
        std::cout << "Tick: " << std::endl;
        std::lock_guard<std::mutex> run_lock((*wheel_mutex)[0]); // 锁定执行时间轮，防止并发访问
        std::lock_guard<std::mutex> queue_lock(queue_mutex); // 锁定任务队列，防止并发访问

        task_queue.push_back_list(multiwheel[0][wheel_level_loc[0]]); // 获取当前执行时间轮的头节点

        bool need_update = update_wheel_loc(0);        

        if(need_update) {
            swap_wheel(1); // 如果需要更新多级时间轮，执行交换操作
        }

    }

    void swap_wheel(int level) {
        if(level >= wheel_levels) {
            return; // 如果超过了时间轮级数，直接返回
        }
        std::unique_lock<std::mutex> lock((*wheel_mutex)[level]); // 锁定当前时间轮级别的锁

        DList<Timer> list(multiwheel[level][wheel_level_loc[level]]);

        lock.unlock();

        while(!list.is_empty()){
            auto timer = list.pop_front();
            add_to_multiwheel(timer); // 将定时器添加到多级时间轮
        }

        bool need_update = update_wheel_loc(level); // 更新当前时间轮级别的位置
        if(need_update) {
            swap_wheel(level + 1); // 递归更新下一级时间轮
        }
    }

    bool update_wheel_loc(int level){
        wheel_level_loc[level]++; // 更新当前时间轮级别的位置
        if(wheel_level_loc[level] >= wheel_size) {
            wheel_level_loc[level] = 0; // 如果超过了时间轮槽数，重置为0
            return true; // 返回true表示需要更新多级时间轮
        }
        return false; // 返回false表示不需要更新多级时间轮
    }

    void add_timer(std::shared_ptr<Timer> timer) {
        timer->update_ticks(tick_duration); // 更新定时器的ticks
        add_to_multiwheel(timer); // 将定时器添加到时间轮中
    }

    void add_to_multiwheel(std::shared_ptr<Timer> timer) {
        int ticks = timer->ticks;
        int level_size = 1;
        for(int i = 0; i < wheel_levels; ++i) {
            std::lock_guard<std::mutex> lock((*wheel_mutex)[i]); // 锁定当前时间轮级别的锁
            ticks = ticks + wheel_level_loc[i]; // 计算新的ticks
            if(ticks <= wheel_size || i == wheel_levels - 1) {
                bind_to_list(timer, multiwheel[i][(ticks - 1) % wheel_size], (*wheel_mutex)[i]); // 将定时器绑定到多级时间轮
                multiwheel[i][(ticks - 1) % wheel_size].push_back(timer); // 将定时器添加到多级时间轮
                return; // 添加成功，直接返回
            } else {
                timer->ticks = timer->ticks - wheel_size * level_size;
                level_size *= wheel_size;
                ticks = ticks / wheel_size;
            }
        }
    }

    void bind_to_list(std::shared_ptr<Timer> timer, DList<Timer>& list, std::mutex& _mtx) {
        timer->timer_list = &list;
        timer->mutex = &_mtx;
    }

    void start(){
        if(isrunning){
            return; // 如果已经在运行，直接返回
        }
        isrunning = true; // 设置为正在运行状态
        threadtime = std::thread([this]() {
            while (isrunning) {
                std::this_thread::sleep_for(std::chrono::milliseconds(tick_duration));
                Tick();
            }
        });
    }

    void stop(){
        std::cout << "Stopping TimerWheel..." << std::endl;
        if(!isrunning){
            return; // 如果没有在运行，直接返回
        }
        isrunning = false; // 设置为不在运行状态
        if(threadtime.joinable()) {
            threadtime.join(); // 等待线程结束
        }
    }

    void run() {
        while(true)
        {
            std::lock_guard<std::mutex> lock(queue_mutex); // 锁定任务队列，防止并发访问
            auto timer = task_queue.pop_front(); // 从任务队列中删除头节点
            if(timer == nullptr) {
                continue; // 继续下一次循环
            }
            if(timer->islong == 1){
                add_timer(timer); // 如果是长定时器，重新添加到时间轮
            }
            if(timer->isvalid) { // 如果定时器仍然有效
                timer->handler(); // 执行定时器的回调函数
            }
        }
    }

};

}


#endif // TIMERWHEEL_HPP
