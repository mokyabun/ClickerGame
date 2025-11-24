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
        upgrades.emplace_back(0, L"커서", L"자동으로 클릭합니다", 15.0, 1.15, 0.1);
        upgrades.emplace_back(1, L"할머니", L"쿠키를 구워줍니다", 100.0, 1.15, 1.0);
        upgrades.emplace_back(2, L"농장", L"쿠키 재료를 생산합니다", 1100.0, 1.15, 8.0);
        upgrades.emplace_back(3, L"광산", L"설탕을 채굴합니다", 12000.0, 1.15, 47.0);
        upgrades.emplace_back(4, L"공장", L"대량으로 생산합니다", 130000.0, 1.15, 260.0);
        upgrades.emplace_back(5, L"은행", L"쿠키 투자로 수익을 냅니다", 1400000.0, 1.15, 1400.0);
    }

    void GameState::InitializeSpecialUpgrades() {
        specialUpgrades.clear();

        specialUpgrades.emplace_back(0, L"수상한 이메일", L"제목학살 수문증", 100.0);
        specialUpgrades.emplace_back(1, L"수상한 이메일", L"유출된 개인정보 DB", 500.0);
        specialUpgrades.emplace_back(2, L"공용 와이파이", L"가짜 핫스팟", 1000.0);
        specialUpgrades.emplace_back(3, L"공용 와이파이", L"암호화 해제", 5000.0);
        specialUpgrades.emplace_back(4, L"트로이 목마", L"\"System32.dll\" 위장술", 11000.0);
        specialUpgrades.emplace_back(5, L"트로이 목마", L"투명 아이콘 (.ico)", 55000.0);
        specialUpgrades.emplace_back(6, L"랜섬웨어 공장", L"QR 결제창", 12000.0);
        specialUpgrades.emplace_back(7, L"랜섬웨어 공장", L"이중 암벽", 60000.0);
        specialUpgrades.emplace_back(8, L"DDOS 공격 위성", L"대기업 신호송 속지기", 130000.0);
        specialUpgrades.emplace_back(9, L"DDOS 공격 위성", L"캔도 수정 추진제", 650000.0);
        specialUpgrades.emplace_back(10, L"자가 진화 AI", L"유리 알고리즘 제각 코드", 1400000.0);
        specialUpgrades.emplace_back(11, L"자가 진화 AI", L"인터넷 아카이브 다운로드", 7000000.0);
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

        if (totalClicks >= cost) {
            totalClicks -= cost;
            upgrade.owned++;
        }
    }

    void GameState::PurchaseSpecialUpgrade(int specialUpgradeId) {
        if (specialUpgradeId < 0 || specialUpgradeId >= static_cast<int>(specialUpgrades.size())) {
            return;
        }

        SpecialUpgradeData& upgrade = specialUpgrades[specialUpgradeId];
        
        // 이미 구매한 경우 리턴
        if (upgrade.isPurchased) {
            return;
        }

        if (totalClicks >= upgrade.cost) {
            totalClicks -= upgrade.cost;
            upgrade.isPurchased = true;
        }
    }

    double GameState::GetAutoProductionRate() const {
        double rate = 0.0;
        for (const auto& upgrade : upgrades) {
            rate += upgrade.GetTotalProduction();
        }
        return rate;
    }

    void GameState::Reset() {
    totalClicks = 0.0;
    clickPower = 1.0;
    InitializeUpgrades();
    InitializeSpecialUpgrades();
}

CString GameState::ToJson() const {
    CString json;
    json.Format(L"{\n  \"totalClicks\": %.2f,\n  \"clickPower\": %.2f,\n  \"upgrades\": [", 
        totalClicks, clickPower);
    
    for (size_t i = 0; i < upgrades.size(); i++) {
        CString upgradeJson;
        upgradeJson.Format(L"\n    {\"id\": %d, \"owned\": %d}%s", 
            upgrades[i].id, upgrades[i].owned, 
            (i < upgrades.size() - 1) ? L"," : L"");
        json += upgradeJson;
    }
    json += L"\n  ]  \n}";
    return json;
}

void GameState::FromJson(const CString& json) {
    // 간단한 파싱 구현 (정규표현식 대신 문자열 검색 사용)
    int pos = 0;
    
    // totalClicks 파싱
    pos = json.Find(L"\"totalClicks\":", pos);
    if (pos >= 0) {
        pos += 14; // "totalClicks": 길이
        int endPos = json.Find(L",", pos);
        if (endPos >= 0) {
            CString value = json.Mid(pos, endPos - pos);
            value.Trim();
            totalClicks = _wtof(value);
        }
    }
    
    // clickPower 파싱
    pos = json.Find(L"\"clickPower\":", 0);
    if (pos >= 0) {
        pos += 13; // "clickPower": 길이
        int endPos = json.Find(L",", pos);
        if (endPos >= 0) {
            CString value = json.Mid(pos, endPos - pos);
            value.Trim();
            clickPower = _wtof(value);
        }
    }
    
    // upgrades 파싱
    pos = json.Find(L"\"upgrades\":", 0);
    if (pos >= 0) {
        pos = json.Find(L"[", pos);
        if (pos >= 0) {
            pos++; // '[' 다음으로 이동
            
            // 각 업그레이드 객체 파싱
            while (true) {
                int objStart = json.Find(L"{", pos);
                if (objStart < 0) break;
                
                int objEnd = json.Find(L"}", objStart);
                if (objEnd < 0) break;
                
                CString upgradeObj = json.Mid(objStart, objEnd - objStart + 1);
                
                // id 파싱
                int idPos = upgradeObj.Find(L"\"id\":");
                int ownedPos = upgradeObj.Find(L"\"owned\":");
                
                if (idPos >= 0 && ownedPos >= 0) {
                    idPos += 5; // "id": 길이
                    int idEnd = upgradeObj.Find(L",", idPos);
                    CString idStr = upgradeObj.Mid(idPos, idEnd - idPos);
                    idStr.Trim();
                    int id = _wtoi(idStr);
                    
                    ownedPos += 8; // "owned": 길이
                    int ownedEnd = upgradeObj.Find(L"}", ownedPos);
                    CString ownedStr = upgradeObj.Mid(ownedPos, ownedEnd - ownedPos);
                    ownedStr.Trim();
                    int owned = _wtoi(ownedStr);
                    
                    // 유효한 ID라면 owned 값 설정
                    if (id >= 0 && id < static_cast<int>(upgrades.size())) {
                        upgrades[id].owned = owned;
                    }
                }
                
                pos = objEnd + 1;
            }
        }
    }
}