#ifndef _DLIST_HPP_
#define _DLIST_HPP_
#include <iostream>
#include <memory>
#include <iterator>

template <typename T>
class DList {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev;
        Node() : data(nullptr), next(nullptr), prev() {}
        Node(const std::shared_ptr<T> value) : data(value), next(nullptr), prev() {}
        void delete_node() {
            if (auto prev_ptr = prev.lock()) { // 检查前驱节点是否存在
                prev_ptr->next = next;        // 更新前驱的 next
            }
            if (next) {
                next->prev = prev; // 更新后继节点的prev指针
            }
        }

    };

    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    size_t size;
public:
    DList() : size(0) {
        head = std::make_shared<Node>(); // 创建头节点
        tail = head;
    }

    DList(DList& t){
        head = std::make_shared<Node>(); // 创建头节点
        tail = head;
        
        if(t.head == t.tail) return;
        
        head->next = t.head->next;
        t.head->next->prev = head;
        tail = t.tail; 
        t.clear();
    }

    DList(DList&& t){
        if(t.head == t.tail) return;
        head->next = t.head->next;
        t.head->next->prev = head;
        tail = t.tail; 
        t.clear();
    }
    ~DList() {};

    // 尾插入
    void push_back(const std::shared_ptr<T> value) {
        auto newNode = std::make_shared<Node>(value);

        tail->next = newNode; // 将新节点链接到尾节点的next
        newNode->prev = tail; // 设置新节点的prev为当前尾节点
        tail = newNode; // 更新尾节点为新节点

        ++size;
        return;
    }

    // 头插入
    void push_front(const std::shared_ptr<T> value) {
        auto newNode = std::make_shared<Node>(value);

        newNode->prev = head; // 设置新节点的prev为头节点
        newNode->next = head->next; // 将新节点的next指向原头节点的next
        head->next->prev = newNode; // 设置头节点的next的prev为新节点
        head->next = newNode; // 更新头节点的next为新节点
        if (tail == head) { // 如果链表之前为空，更新尾节点
            tail = newNode;
        }
        
        ++size;
        return;
    }

    // 尾插入链表
    void push_back_list(DList<T>& other) {
        if (other.is_empty()) return; // 如果其他链表为空，直接返回
        if (is_empty()) {
            head->next = other.head->next;
            other.head->next->prev = head;
            tail = other.tail; 
        } else {
            tail->next = other.head->next; 
            other.head->next->prev = tail; 
            tail = other.tail; 
        }
        size += other.size; // 更新当前链表的大小
        other.clear();
    }

    // 尾删除并返回数据
    std::shared_ptr<T> pop_back() {

        if(tail == head) return nullptr; // 如果链表为空，直接返回
        
        std::shared_ptr<T> data = tail->data; // 保存尾节点的数据

        if (tail->prev.lock()) {
            tail = tail->prev.lock(); // 将尾节点更新为前驱节点
            tail->next.reset(); // 清空新的尾节点的next指针
        }
        --size; // 减少链表大小
        return data;
    }

    // 头删除并返回数据
    std::shared_ptr<T> pop_front() {
        if (head == tail) return nullptr; // 如果链表为空，直接返回

        std::shared_ptr<T> data = head->next->data; // 保存头节点的下一个节点的数据
        
        if(head->next == tail){
            head->next.reset(); // 如果只有一个节点，清空头节点的next指针
            tail = head; // 更新尾节点为头节点
        } else {
            head->next = head->next->next; // 将头节点的next更新为下一个节点
            head->next->prev = head; // 更新下一个节点的prev指针为头节点
        }

        --size; // 减少链表大小
        return data; // 返回被删除节点的数据
    }

    // 链表大小
    size_t get_size() const {
        return size;
    }
    
    // 是否为空
    bool is_empty() const {
        return tail == head; // 如果尾节点等于头节点，说明链表为空
    }

    // 清空链表
    void clear() {
        head->next.reset(); // 清空头节点的next指针
        tail.reset();
        tail = head;
        size = 0;
    }

    // 删除指定节点
    bool delete_node(std::shared_ptr<T> node) {
        if (!node) return false;
        auto current = head->next;
        while(current){
            if(current->data == node){
                if (current->prev.lock()) {
                    current->prev.lock()->next = current->next; // 更新前驱节点的next指针
                }

                if (current->next) {
                    current->next->prev = current->prev; // 更新后继节点的prev指针
                }

                if(current == tail) {
                    tail = current->prev.lock(); // 如果删除的是尾节点，更新尾节点
                }
                --size;
                return true;
            }
            current = current->next; // 继续遍历下一个节点
        }
        return false;
    }

    // 得到头节点
    std::shared_ptr<Node> get_head() const {
        return head->next;
    }

    // 得到尾节点
    std::shared_ptr<Node> get_tail() const {
        return tail;
    }

    // 迭代器支持
    class iterator {
    private:
        std::shared_ptr<Node> current;
    public:
        iterator(std::shared_ptr<Node> node) : current(node) {}

        T& operator*() { return *(current->data); }

        iterator& operator++() {
            current = current->next;
            return *this;
        }

        bool operator!=(const iterator& other) const { return current != other.current; }
    };

    iterator begin() { return iterator(head->next); }
    iterator end() { return iterator(tail->next); }
};

#endif // DLIST_HPP