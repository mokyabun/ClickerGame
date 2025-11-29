#include "pch.h"
#include "GameState.h"
#include <cmath>

GameState::GameState()
        : totalClicks(0.0)
        , clickPower(1.0)
    {
        InitializeUpgrades();
        InitializeSpecialUpgrades();
    }

    void GameState::InitializeUpgrades() {
        upgrades.clear();

        // 기본 업그레이드 아이템들 추가
        upgrades.emplace_back(0, _T("수상한 이메일"), _T("수상한 이메일을 전송합니다"), 15.0, 1.15, 0.1);
        upgrades.emplace_back(1, _T("공용 와이파이"), _T("공용 와이파이를 해킹합니다"), 100.0, 1.15, 1.0);
        upgrades.emplace_back(2, _T("트로이 목마"), _T("트로이 목마를 설치합니다"), 1100.0, 1.15, 8.0);
        upgrades.emplace_back(3, _T("랜섬웨어 공장"), _T("랜섬웨어 공장을 구축합니다"), 12000.0, 1.15, 47.0);
        upgrades.emplace_back(4, _T("DDOS 공격 위성"), _T ("DDOS 공격 위성을 구축합니다"), 130000.0, 1.15, 260.0);
        upgrades.emplace_back(5, _T("자가 진화 AI"), _T("자가 진화 AI를 구축합니다"), 1400000.0, 1.15, 1400.0);
    }

    void GameState::InitializeSpecialUpgrades() {
        specialUpgrades.clear();

        specialUpgrades.emplace_back(0, _T("수상한 이메일"), _T("제목학살 수문증"), 100.0);
        specialUpgrades.emplace_back(1, _T("수상한 이메일"), _T("유출된 개인정보 DB"), 500.0);
        specialUpgrades.emplace_back(2, _T("공용 와이파이"), _T("가짜 핫스팟"), 1000.0);
        specialUpgrades.emplace_back(3, _T("공용 와이파이"), _T("암호화 해제"), 5000.0);
        specialUpgrades.emplace_back(4, _T("트로이 목마"), _T("System32.dll 위장술"), 11000.0);
        specialUpgrades.emplace_back(5, _T("트로이 목마"), _T("투명 아이콘 (.ico)"), 55000.0);
        specialUpgrades.emplace_back(6, _T("랜섬웨어 공장"), _T("QR 결제창"), 12000.0);
        specialUpgrades.emplace_back(7, _T("랜섬웨어 공장"), _T("이중 암벽"), 60000.0);
        specialUpgrades.emplace_back(8, _T("DDOS 공격 위성"), _T("대기업 신호송 속지기"), 130000.0);
        specialUpgrades.emplace_back(9, _T("DDOS 공격 위성"), _T("캔도 수정 추진제"), 650000.0);
        specialUpgrades.emplace_back(10, _T("자가 진화 AI"), _T("유리 알고리즘 제각 코드"), 1400000.0);
        specialUpgrades.emplace_back(11, _T("자가 진화 AI"), _T("인터넷 아카이브 다운로드"), 7000000.0);
    }

    const UpgradeData* GameState::GetUpgrade(int id) const {
        if (id < 0 || id >= static_cast<int>(upgrades.size())) {
            return nullptr;
        }
        return &upgrades[id];
    }

    const SpecialUpgradeData* GameState::GetSpecialUpgrade(int id) const {
        if (id < 0 || id >= static_cast<int>(specialUpgrades.size())) {
            return nullptr;
        }
        return &specialUpgrades[id];
    }

    bool GameState::CanAffordUpgrade(int upgradeId) const {
        if (upgradeId < 0 || upgradeId >= static_cast<int>(upgrades.size())) {
            return false;
        }
        const UpgradeData& upgrade = upgrades[upgradeId];
        return totalClicks >= upgrade.GetCurrentCost();
    }

    bool GameState::CanAffordSpecialUpgrade(int specialUpgradeId) const {
        if (specialUpgradeId < 0 || specialUpgradeId >= static_cast<int>(specialUpgrades.size())) {
            return false;
        }
        const SpecialUpgradeData& upgrade = specialUpgrades[specialUpgradeId];
        
        // 이미 구매한 경우 구매 불가
        if (upgrade.isPurchased) {
            return false;
        }
        
        return totalClicks >= upgrade.cost;
    }

    void GameState::PurchaseUpgrade(int upgradeId) {
        if (upgradeId < 0 || upgradeId >= static_cast<int>(upgrades.size())) {
            return;
        }

        UpgradeData& upgrade = upgrades[upgradeId];
        double cost = upgrade.GetCurrentCost();

        // 구매 가능한지 확인
        if (totalClicks < cost) return;

        // 구매 확정
        totalClicks -= cost;
        upgrade.owned++;
    }

    void GameState::PurchaseSpecialUpgrade(int specialUpgradeId) {
        // Panic 방지
        if (specialUpgradeId < 0) return;
        if (specialUpgradeId >= static_cast<int>(specialUpgrades.size())) return;

        SpecialUpgradeData& upgrade = specialUpgrades[specialUpgradeId];

        // 구매 가능한지 확인
        if (upgrade.isPurchased) return;
        if (totalClicks < upgrade.cost) return;

        // 구매 확정
        totalClicks -= upgrade.cost;
        upgrade.isPurchased = true;
    }

    double GameState::GetAutoProductionRate() const {
        double totalRate = 0.0;
        
        for (const auto& upgrade : upgrades) {
            double upgradeRate = upgrade.GetTotalProduction();
            
            // 특별 업그레이드 효과 적용
            double multiplier = 1.0;
            for (const auto& special : specialUpgrades) {
                if (!special.isPurchased) continue;
                if (special.category != upgrade.name) continue;

                multiplier *= 2.0;
            }
            
            totalRate += upgradeRate * multiplier;
        }
        
        return totalRate;
    }

    void GameState::Reset() {
    totalClicks = 0.0;
    clickPower = 1.0;
    InitializeUpgrades();
    InitializeSpecialUpgrades();
}