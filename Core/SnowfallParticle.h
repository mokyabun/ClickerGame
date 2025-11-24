#pragma once
#include <vector>
#include <windows.h>
#include <gdiplus.h>

// 눈꽃처럼 떨어지는 바이러스 파티클 구조체
struct SnowfallParticle
{
	float x, y;           // 위치
	float vx, vy;         // 속도
	float life;           // 생명 (0.0 ~ 1.0)
	float maxLife;        // 최대 생명
	float size;           // 크기
	float alpha;          // 투명도
	
	SnowfallParticle(float _x, float _y)
		: x(_x), y(_y)
		, vx(((rand() % 200) - 100) / 10.0f)  // -10 ~ +10 (좌우 흔들림)
		, vy(30.0f + (rand() % 51))            // 30 ~ 80 (아래로 낙하)
		, maxLife(8.0f + (rand() % 5))         // 8 ~ 12초 생명
		, life(1.0f)
		, size(15.0f + (rand() % 16))          // 15 ~ 30 픽셀
		, alpha(150.0f)                        // 초기 투명도
	{
	}
};

// 눈꽃 파티클 시스템 클래스
class SnowfallParticleSystem
{
public:
	SnowfallParticleSystem();
	~SnowfallParticleSystem();

	// 이미지 로드
	bool LoadVirusImage(const CString& imagePath);
	
	// ZPC/s에 따른 생산율 설정
	void SetProductionRate(double zpcPerSecond);
	
	// 마우스 클릭 시 파티클 방출
	void OnMouseClick(float x);
	
	// 업데이트 (deltaTime: 초 단위)
	void Update(float deltaTime);
	
	// 렌더링
	void Draw(Gdiplus::Graphics& graphics);
	
	// 모든 파티클 제거
	void Clear();

private:
	std::vector<SnowfallParticle> m_particles;
	Gdiplus::Image* m_pVirusImage;
	
	// 현재 spawn rate (초당 파티클 수)
	float m_spawnRate;
	float m_spawnAccumulator;
	
	// ZPC/s 단계별 spawn rate 결정
	float CalculateSpawnRate(double zpcPerSecond) const;
	
	// 새 파티클 생성 (랜덤 X 위치, 화면 위에서)
	void SpawnParticle(float x = -1.0f);
};
