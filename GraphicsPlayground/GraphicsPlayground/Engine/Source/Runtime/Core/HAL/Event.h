#pragma once

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <cassert>

namespace Event
{
    enum class ResetPolicy { Manual, Auto };
    enum class NotifyMode { One, All };
}

// 템플릿 이벤트
template <Event::ResetPolicy Policy, Event::NotifyMode Mode>
class TEvent
{
    static_assert(!(Policy == Event::ResetPolicy::Auto && Mode == Event::NotifyMode::All),
        "Auto+All 조합은 모호하거나 비권장: 하나만 깨우는 Auto 의미가 희석됩니다.");

public:
    TEvent() = default;
    TEvent(const TEvent&) = delete;
    TEvent& operator=(const TEvent&) = delete;

    // 수동 리셋 전용 헬퍼: 초기 상태를 true 로 만들고 싶다면 생성자 오버로드 가능
    void Reset() noexcept
    {
        std::lock_guard<std::mutex> _(Mutex);
        bSignaled = false;
    }

    // 신호 보내기
    void Trigger() noexcept
    {
        {
            std::lock_guard<std::mutex> _(Mutex);
            bSignaled = true;
        }
        NotifyImpl(); // 잠금 밖에서 notify
    }

    // 무조건 대기
    void Wait() noexcept
    {
        std::unique_lock<std::mutex> lock(Mutex);
        CondVar.wait(lock, [this]() {
            if (!bSignaled)
                return false;

            if constexpr (Policy == Event::ResetPolicy::Auto)
            {
                // Auto + One : 소비 후 false
                if constexpr (Mode == Event::NotifyMode::One)
                    bSignaled = false;
                // Auto + All 은 금지했으므로 여기 도달하지 않음
            }
            // Manual 은 여기서 flag 유지
            return true;
            });
    }

    // 타임아웃 대기 (true: 획득, false: 타임아웃)
    template <class Rep, class Period>
    bool WaitFor(const std::chrono::duration<Rep, Period>& dur) noexcept
    {
        std::unique_lock<std::mutex> lock(Mutex);
        bool acquired = CondVar.wait_for(lock, dur, [this]() {
            if (!bSignaled)
                return false;
            if constexpr (Policy == Event::ResetPolicy::Auto)
            {
                if constexpr (Mode == Event::NotifyMode::One)
                    bSignaled = false;
            }
            return true;
            });
        return acquired;
    }

    // 현재 상태 확인 (주의: 순간적인 값)
    bool IsSignaled() const noexcept
    {
        std::lock_guard<std::mutex> _(Mutex);
        return bSignaled;
    }

private:
    void NotifyImpl() noexcept
    {
        if constexpr (Mode == Event::NotifyMode::One)
            CondVar.notify_one();
        else
            CondVar.notify_all();
    }

private:
    mutable std::mutex      Mutex;
    std::condition_variable CondVar;
    bool                    bSignaled = false;
};
