#pragma once
#include "GameState.h"
#include <functional>

// 게임 코어 클래스 (로직 처리)
class GameCore {
    private:
        GameState state;  // 중앙 상태 관리

        // 콜백 함수들 (UI 업데이트용)
        std::function<void(double)> onClicksChanged;
        std::function<void()> onUpgradesChanged;

    public:
        GameCore();
        ~GameCore() = default;

        // 기본 게임 동작
        void PerformClick();  // 클릭 수행
        void Update(double deltaTime);  // 게임 업데이트 (자동 생산)

        // 업그레이드 관리
        bool PurchaseUpgrade(int upgradeId);  // 업그레이드 구매
        bool PurchaseSpecialUpgrade(int specialUpgradeId);  // 특별 업그레이드 구매

        // State 접근
        const GameState& GetState() const { return state; }
        GameState& GetState() { return state; }

        // 편의 게터 (State로 위임)
        double GetTotalClicks() const { return state.GetTotalClicks(); }
        double GetClickPower() const { return state.GetClickPower(); }
        double GetAutoProductionRate() const { return state.GetAutoProductionRate(); }
        const std::vector<UpgradeData>& GetUpgrades() const { return state.GetUpgrades(); }
        const UpgradeData* GetUpgrade(int id) const { return state.GetUpgrade(id); }

        // 특별 업그레이드 접근
        const std::vector<SpecialUpgradeData>& GetSpecialUpgrades() const { return state.GetSpecialUpgrades(); }
        const SpecialUpgradeData* GetSpecialUpgrade(int id) const { return state.GetSpecialUpgrade(id); }

        // 세터
        void SetClickPower(double power) { state.SetClickPower(power); }

        // 콜백 등록
        void SetOnClicksChangedCallback(std::function<void(double)> callback) {
            onClicksChanged = callback;
        }
        void SetOnUpgradesChangedCallback(std::function<void()> callback) {
            onUpgradesChanged = callback;
        }

        // 게임 초기화
        void Reset();

    private:
        void NotifyClicksChanged();
        void NotifyUpgradesChanged();

        // Frenzy State
        double m_frenzyMultiplier = 1.0;
        double m_frenzyTimer = 0.0;

    public:
        void ActivateFrenzy(double multiplier, double duration);
        bool IsFrenzyActive() const { return m_frenzyTimer > 0.0; }
        double GetFrenzyMultiplier() const { return m_frenzyMultiplier; }
    };
