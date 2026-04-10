#pragma once

#include <queue>
#include <mutex>
#include <optional>

template<typename T>
class ThreadSafeQueue {
public:
    void Push(T value) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Queue.push(std::move(value));
    }

    std::optional<T> TryPop() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Queue.empty())
            return std::nullopt;
        T value = std::move(m_Queue.front());
        m_Queue.pop();
        return value;
    }

    bool Empty() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.empty();
    }

private:
    std::queue<T> m_Queue;
    std::mutex m_Mutex;
};