#pragma once

#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
public:
    void Push(T value) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push(std::move(value));
        }
        m_ConditionVar.notify_one(); // Wake up one worker
    }

    std::optional<T> TryPop() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Queue.empty())
            return std::nullopt;
        T value = std::move(m_Queue.front());
        m_Queue.pop();
        return value;
    }

    std::optional<T> Pop()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);

        m_ConditionVar.wait(lock, [this]()
            {
                return !m_Queue.empty() || m_Stopped;
            });

        if (m_Stopped && m_Queue.empty())
            return std::nullopt;

        T value = std::move(m_Queue.front());
        m_Queue.pop();
        return value;
    }

    void Stop()
    {
        m_Stopped = true;
        m_ConditionVar.notify_all();
    }   

    bool Empty() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.empty();
    }

private:
    std::queue<T> m_Queue;
    std::mutex m_Mutex;
    std::condition_variable m_ConditionVar;
    std::atomic<bool> m_Stopped = false;
};