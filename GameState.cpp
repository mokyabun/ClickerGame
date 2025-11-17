#include "pch.h"
#include "GameState.h"
#include <cmath>

GameState::GameState()
        : totalClicks(0.0)
        , clickPower(1.0)
    {
        InitializeUpgrades();
    }

    void GameState::InitializeUpgrades() {
        upgrades.clear();

        // 기본 업그레이드 아이템들 추가
        upgrades.emplace_back(0, L"\uCEE4\uC11C", L"\uC790\uB3D9\uC73C\uB85C \uD074\uB9AD\uD569\uB2C8\uB2E4", 15.0, 1.15, 0.1);
        upgrades.emplace_back(1, L"\uD560\uBA38\uB2C8", L"\uCFE0\uD0A4\uB97C \uAD6C\uC6CC\uC90D\uB2C8\uB2E4", 100.0, 1.15, 1.0);
        upgrades.emplace_back(2, L"\uB18D\uC7A5", L"\uCFE0\uD0A4 \uC7AC\uB8CC\uB97C \uC0DD\uC0B0\uD569\uB2C8\uB2E4", 1100.0, 1.15, 8.0);
        upgrades.emplace_back(3, L"\uAD11\uC0B0", L"\uC124\uD0D5\uC744 \uCC44\uAD74\uD569\uB2C8\uB2E4", 12000.0, 1.15, 47.0);
        upgrades.emplace_back(4, L"\uACF5\uC7A5", L"\uB300\uB7C9\uC73C\uB85C \uC0DD\uC0B0\uD569\uB2C8\uB2E4", 130000.0, 1.15, 260.0);
        upgrades.emplace_back(5, L"\uC740\uD589", L"\uCFE0\uD0A4 \uD22C\uC790\uB85C \uC218\uC775\uC744 \uB0C5\uB2C8\uB2E4", 1400000.0, 1.15, 1400.0);
    }

    const UpgradeData* GameState::GetUpgrade(int id) const {
        if (id < 0 || id >= static_cast<int>(upgrades.size())) {
            return nullptr;
        }
        return &upgrades[id];
    }

    bool GameState::CanAffordUpgrade(int upgradeId) const {
        if (upgradeId < 0 || upgradeId >= static_cast<int>(upgrades.size())) {
            return false;
        }
        const UpgradeData& upgrade = upgrades[upgradeId];
        return totalClicks >= upgrade.GetCurrentCost();
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
    json += L"\n  ]\n}";
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