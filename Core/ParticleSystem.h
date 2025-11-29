#pragma once
#include <vector>
#include <windows.h>
#include <gdiplus.h>

struct Particle
{
	float x, y;
	float vx, vy;
	float life;
	float size;
	float alpha;
	float angle;
	
	Particle(float _x, float _y)
		: x(_x), y(_y)
		, vx((rand() % 400 - 200) / 10.0f)  // -20 ~ 20 (좌우 Vector 퍼짐)
		, vy((rand() % 400 - 200) / 10.0f)  // -20 ~ 20 (상하 Vector 퍼짐)
		, life(1.0f)
		, size(8.0f + (rand() % 8))  // 8~15 크기
		, alpha(255.0f)
		, angle((float)(rand() % 360))  // 0~360 랜덤 각도
	{
	}
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Emit(float x, float y, int count = 10);
	void Update(float deltaTime);
	void Draw(Gdiplus::Graphics& graphics);
	void Clear();

private:
	std::vector<Particle> m_particles;
	const float m_gravity = 500.0f;
	const float m_fadeSpeed = 1.5f;
};
