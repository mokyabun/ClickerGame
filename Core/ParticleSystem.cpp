#include "pch.h"
#include "ParticleSystem.h"
#include <algorithm>

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Emit(float x, float y, int count)
{
	for (int i = 0; i < count; i++)
	{
		m_particles.emplace_back(x, y);
	}
}

void ParticleSystem::Update(float deltaTime)
{
	// ëª¨ë“  íŒŒí‹°í´ ì—…ë°ì´íŠ¸
	for (auto it = m_particles.begin(); it != m_particles.end(); )
	{
		Particle& p = *it;
		
		// ë¬¼ë¦¬ ì—…ë°ì´íŠ¸
		p.vy += m_gravity * deltaTime;  // ì¤‘ë ¥ ì ìš©
		p.x += p.vx * deltaTime;
		p.y += p.vy * deltaTime;
		
		// ìƒëª… ê°ì†Œ
		p.life -= m_fadeSpeed * deltaTime;
		p.alpha = p.life * 255.0f;
		
		// ìƒëª…ì´ ë‹¤í•œ íŒŒí‹°í´ ì œê±°
		if (p.life <= 0.0f)
		{
			it = m_particles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ParticleSystem::Draw(Gdiplus::Graphics& graphics)
{
	for (const auto& p : m_particles)
	{
		// ì•ŒíŒŒê°’ ì ìš©í•œ ê²€ì€ìƒ‰ (0~255 ë²”ìœ„ë¡œ í´ëž¨í•‘)
		float clampedAlpha = p.alpha;
		if (clampedAlpha < 0.0f) clampedAlpha = 0.0f;
		if (clampedAlpha > 255.0f) clampedAlpha = 255.0f;
		int alpha = (int)clampedAlpha;
		
		Gdiplus::Color color(alpha, 0, 0, 0);
		Gdiplus::SolidBrush brush(color);
		
		// ì‚¬ê°í˜• íŒŒí‹°í´ íšŒì „í•˜ì—¬ ê·¸ë¦¬ê¸°
		Gdiplus::GraphicsState state = graphics.Save();
		
		// íŒŒí‹°í´ ì¤‘ì‹¬ìœ¼ë¡œ ì´ë™ í›„ íšŒì „
		graphics.TranslateTransform(p.x, p.y);
		graphics.RotateTransform(p.angle);
		
		// ì¤‘ì‹¬ ê¸°ì¤€ìœ¼ë¡œ ì‚¬ê°í˜• ê·¸ë¦¬ê¸°
		graphics.FillRectangle(&brush, 
			-p.size / 2, 
			-p.size / 2, 
			p.size, 
			p.size);
		
		graphics.Restore(state);
	}
}

void ParticleSystem::Clear()
{
	m_particles.clear();
}
