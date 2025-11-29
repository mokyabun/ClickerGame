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
	for (auto it = m_particles.begin(); it != m_particles.end(); ) {
		Particle& p = *it;
		
		p.vy += m_gravity * deltaTime;
		p.x += p.vx * deltaTime;
		p.y += p.vy * deltaTime;
		
		// 점점 사라지게
		p.life -= m_fadeSpeed * deltaTime;
		p.alpha = p.life * 255.0f;
		
		// 죽은 파티클 제거
		if (p.life <= 0.0f) {
			it = m_particles.erase(it);
		} else {
			++it;
		}
	}
}

void ParticleSystem::Draw(Gdiplus::Graphics& graphics)
{
	for (const auto& p : m_particles) {
		float clampedAlpha = std::clamp(p.alpha, 0.0f, 255.0f);
		int alpha = static_cast<int>(clampedAlpha);
		
		Gdiplus::Color color(alpha, 0, 0, 0);
		Gdiplus::SolidBrush brush(color);
		
		Gdiplus::GraphicsState state = graphics.Save();
		
		graphics.TranslateTransform(p.x, p.y);
		graphics.RotateTransform(p.angle);
		
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
