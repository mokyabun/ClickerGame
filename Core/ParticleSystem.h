#pragma once
#include <vector>
#include <windows.h>
#include <gdiplus.h>

// 개별 파티클 구조체
struct Particle
{
	float x, y;           // 위치
	float vx, vy;         // 속도
	float life;           // 생명 (0.0 ~ 1.0)
	float size;           // 크기
	float alpha;          // 투명도
	float angle;          // 회전 각도 (도)
	
	Particle(float _x, float _y)
		: x(_x), y(_y)
		, vx((rand() % 400 - 200) / 10.0f)  // -20 ~ 20 (좌우로 퍼짐)
		, vy((rand() % 400 - 200) / 10.0f)  // -20 ~ 20 (상하로 퍼짐)
		, life(1.0f)
		, size(8.0f + (rand() % 8))  // 8~15 크기
		, alpha(255.0f)
		, angle((float)(rand() % 360))  // 0~360도 랜덤 각도
	{
	}
};

// 파티클 시스템 클래스
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	// 파티클 방출 (클릭 위치에서)
	void Emit(float x, float y, int count = 10);
	
	// 업데이트 (deltaTime: 초 단위)
	void Update(float deltaTime);
	
	// 렌더링
	void Draw(Gdiplus::Graphics& graphics);
	
	// 모든 파티클 제거
	void Clear();

private:
	std::vector<Particle> m_particles;
	const float m_gravity = 500.0f;  // 중력 가속도
	const float m_fadeSpeed = 1.5f;   // 페이드 아웃 속도
};
