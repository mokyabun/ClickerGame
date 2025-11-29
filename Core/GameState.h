#pragma once
#include <string>
#include <vector>

struct UpgradeData {
    int id;
    std::wstring name;
    std::wstring description;
    double baseCost;
    double costMultiplier;
    double productionPerSecond;
    int owned;

    UpgradeData(int _id, const std::wstring& _name, const std::wstring& _desc,
        double _baseCost, double _costMultiplier, double _production)
        : id(_id), name(_name), description(_desc),
        baseCost(_baseCost), costMultiplier(_costMultiplier),
        productionPerSecond(_production), owned(0) {}

    double GetCurrentCost() const {
        return baseCost * pow(costMultiplier, owned);
    }

    double GetTotalProduction() const {
        return productionPerSecond * owned;
    }
};

struct SpecialUpgradeData {
    int id;
    std::wstring category;
    std::wstring name;
    double cost;
    bool isPurchased;

    SpecialUpgradeData(int _id, const std::wstring& _category, const std::wstring& _name, double _cost)
        : id(_id), category(_category), name(_name), cost(_cost), isPurchased(false) {}
};

class GameState {
private:
    double totalClicks;
    double clickPower;
    std::vector<UpgradeData> upgrades;
    std::vector<SpecialUpgradeData> specialUpgrades;

public:
    GameState();
    ~GameState() = default;

    double GetTotalClicks() const { return totalClicks; }
    double GetClickPower() const { return clickPower; }
    const std::vector<UpgradeData>& GetUpgrades() const { return upgrades; }
    std::vector<UpgradeData>& GetUpgrades() { return upgrades; }
    const UpgradeData* GetUpgrade(int id) const;

    const std::vector<SpecialUpgradeData>& GetSpecialUpgrades() const { return specialUpgrades; }
    std::vector<SpecialUpgradeData>& GetSpecialUpgrades() { return specialUpgrades; }
    const SpecialUpgradeData* GetSpecialUpgrade(int id) const;

    void SetTotalClicks(double value) { totalClicks = value; }
    void SetClickPower(double value) { clickPower = value; }
    void AddClicks(double value) { totalClicks += value; }

    bool CanAffordUpgrade(int upgradeId) const;
    void PurchaseUpgrade(int upgradeId);

    bool CanAffordSpecialUpgrade(int specialUpgradeId) const;
    void PurchaseSpecialUpgrade(int specialUpgradeId);

    double GetAutoProductionRate() const;

    void Reset();
    void InitializeUpgrades();
    void InitializeSpecialUpgrades();
};