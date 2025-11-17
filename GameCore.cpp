#include "pch.h"
#include "GameCore.h"

GameCore::GameCore()
        : onClicksChanged(nullptr)
        , onUpgradesChanged(nullptr)
    {
    }

    void GameCore::PerformClick() {
        state.AddClicks(state.GetClickPower());
        NotifyClicksChanged();
    }

    void GameCore::Update(double deltaTime) {
        double productionRate = state.GetAutoProductionRate();
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

    void GameCore::Reset() {
        state.Reset();
        NotifyClicksChanged();
        NotifyUpgradesChanged();
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