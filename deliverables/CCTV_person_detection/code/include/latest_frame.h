// 추론 대기는 최신 프레임 하나만 유지한다. 느린 소비자를 기다리지 않고 이전 대기를 교체한다.
// 프로젝트용 동시성 보조 코드이며 공식 YOLO 예제의 단일 루프에는 없는 구조다.
#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <utility>

template<class T> class LatestFrame {
public:
    void publish(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            value_ = std::move(value);
            ready_ = true;
        }
        changed_.notify_one();
    }
    bool take(T &value, const std::atomic<bool> &stopped) {
        std::unique_lock<std::mutex> lock(mutex_);
        changed_.wait_for(lock, std::chrono::milliseconds(100), [&] {
            return ready_ || stopped.load();
        });
        if (stopped.load() || !ready_) return false;
        value = std::move(value_);
        value_ = T{};
        ready_ = false;
        return true; // 반환 시 mutex 해제. 추론은 이 잠금 밖에서 실행한다.
    }
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = T{};
        ready_ = false;
    }
    void wake() { changed_.notify_all(); }
private:
    std::mutex mutex_;
    std::condition_variable changed_;
    T value_{};
    bool ready_ = false;
};
