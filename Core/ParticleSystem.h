#pragma once
#include <vector>
#include <windows.h>
#include <gdiplus.h>

// ê°œë³„ íŒŒí‹°í´ êµ¬ì¡°ì²´
struct Particle
{
	float x, y;           // ìœ„ì¹˜
	float vx, vy;         // ì†ë„
	float life;           // ìƒëª… (0.0 ~ 1.0)
	float size;           // í¬ê¸°
	float alpha;          // íˆ¬ëª…ë„
	float angle;          // íšŒì „ ê°ë„ (ë„)
	
	Particle(float _x, float _y)
		: x(_x), y(_y)
		, vx((rand() % 400 - 200) / 10.0f)  // -20 ~ 20 (ì¢Œìš°ë¡œ í¼ì§)
		, vy((rand() % 400 - 200) / 10.0f)  // -20 ~ 20 (ìƒí•˜ë¡œ í¼ì§)
		, life(1.0f)
		, size(8.0f + (rand() % 8))  // 8~15 í¬ê¸°
		, alpha(255.0f)
		, angle((float)(rand() % 360))  // 0~360ë„ ëžœë¤ ê°ë„
	{
	}
};

// íŒŒí‹°í´ ì‹œìŠ¤í…œ í´ëž˜ìŠ¤
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	// íŒŒí‹°í´ ë°©ì¶œ (í´ë¦­ ìœ„ì¹˜ì—ì„œ)
	void Emit(float x, float y, int count = 10);
	
	// ì—…ë°ì´íŠ¸ (deltaTime: ì´ˆ ë‹¨ìœ„)
	void Update(float deltaTime);
	
	// ë Œë”ë§
	void Draw(Gdiplus::Graphics& graphics);
	
	// ëª¨ë“  íŒŒí‹°í´ ì œê±°
	void Clear();

private:
	std::vector<Particle> m_particles;
	const float m_gravity = 500.0f;  // ì¤‘ë ¥ ê°€ì†ë„
	const float m_fadeSpeed = 1.5f;   // íŽ˜ì´ë“œ ì•„ì›ƒ ì†ë„
};
