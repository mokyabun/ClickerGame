#include "pch.h"
#include "GameCore.h"

GameCore::GameCore()
        : onClicksChanged(nullptr)
        , onUpgradesChanged(nullptr)
    {
    }

    void GameCore::PerformClick() {
        state.AddClicks(state.GetClickPower() * m_frenzyMultiplier);
        NotifyClicksChanged();
    }

    void GameCore::Update(double deltaTime) {
        // Update Frenzy Timer
        if (m_frenzyTimer > 0) {
            m_frenzyTimer -= deltaTime;
            if (m_frenzyTimer <= 0) {
                m_frenzyTimer = 0;
                m_frenzyMultiplier = 1.0;
            }
        }

        double productionRate = state.GetAutoProductionRate() * m_frenzyMultiplier;
        if (productionRate > 0) {
            state.AddClicks(productionRate * deltaTime);
            NotifyClicksChanged();
        }
    }

    bool GameCore::PurchaseUpgrade(int upgradeId) {
        if (!state.CanAffordUpgrade(upgradeId)) {
            return false;
        }

        state.PurchaseUpgrade(upgradeId);
        NotifyClicksChanged();
        NotifyUpgradesChanged();
        return true;
    }

    bool GameCore::PurchaseSpecialUpgrade(int specialUpgradeId) {
        if (!state.CanAffordSpecialUpgrade(specialUpgradeId)) {
            return false;
        }

        state.PurchaseSpecialUpgrade(specialUpgradeId);
        NotifyClicksChanged();
        NotifyUpgradesChanged();
        return true;
    }

    void GameCore::Reset() {
        state.Reset();
        m_frenzyMultiplier = 1.0;
        m_frenzyTimer = 0.0;
        NotifyClicksChanged();
        NotifyUpgradesChanged();
    }

    void GameCore::ActivateFrenzy(double multiplier, double duration) {
        m_frenzyMultiplier = multiplier;
        m_frenzyTimer = duration;
    }

    void GameCore::NotifyClicksChanged() {
        if (onClicksChanged) {
            onClicksChanged(state.GetTotalClicks());
        }
    }

    void GameCore::NotifyUpgradesChanged() {
    if (onUpgradesChanged) {
        onUpgradesChanged();
    }
}