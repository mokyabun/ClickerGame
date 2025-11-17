#pragma once
#include <string>
#include <vector>

// 업그레이드 데이터
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

        // 현재 구매 비용 계산
        double GetCurrentCost() const {
            return baseCost * pow(costMultiplier, owned);
        }

        // 총 생산량 계산
        double GetTotalProduction() const {
            return productionPerSecond * owned;
        }
    };

    // 게임 상태 관리 클래스 (데이터 중앙 집중화)
    class GameState {
    private:
        double totalClicks;
        double clickPower;
        std::vector<UpgradeData> upgrades;

    public:
        GameState();
        ~GameState() = default;

        // Getters
        double GetTotalClicks() const { return totalClicks; }
        double GetClickPower() const { return clickPower; }
        const std::vector<UpgradeData>& GetUpgrades() const { return upgrades; }
        std::vector<UpgradeData>& GetUpgrades() { return upgrades; }
        const UpgradeData* GetUpgrade(int id) const;

        // Setters
        void SetTotalClicks(double value) { totalClicks = value; }
        void SetClickPower(double value) { clickPower = value; }
        void AddClicks(double value) { totalClicks += value; }

        // 업그레이드 관리
        bool CanAffordUpgrade(int upgradeId) const;
        void PurchaseUpgrade(int upgradeId);

        // 자동 생산량 계산
        double GetAutoProductionRate() const;

	// 초기화
	void Reset();
	void InitializeUpgrades();

	// JSON 직렬화
	CString ToJson() const;
	void FromJson(const CString& json);
};