#pragma once

#include <unordered_set>
#include <mutex>
#include <optional>

template<typename T, typename Hash = std::hash<T>>
class ThreadSafeSet {
public:
    void Push(T value) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Set.insert(std::move(value)); // Ignore if already present
    }

    std::optional<T> TryPop() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Set.empty())
            return std::nullopt;
        T value = *m_Set.begin();
        m_Set.erase(m_Set.begin());
        return value;
    }

    bool Empty() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Set.empty();
    }

private:
    std::unordered_set<T, Hash> m_Set;
    std::mutex m_Mutex;
};