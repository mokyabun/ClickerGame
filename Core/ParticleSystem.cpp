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
	// 모든 파티클 업데이트
	for (auto it = m_particles.begin(); it != m_particles.end(); )
	{
		Particle& p = *it;
		
		// 물리 업데이트
		p.vy += m_gravity * deltaTime;  // 중력 적용
		p.x += p.vx * deltaTime;
		p.y += p.vy * deltaTime;
		
		// 생명 감소
		p.life -= m_fadeSpeed * deltaTime;
		p.alpha = p.life * 255.0f;
		
		// 생명이 다한 파티클 제거
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
		// 알파값 적용한 검은색 (0~255 범위로 클램핑)
		float clampedAlpha = p.alpha;
		if (clampedAlpha < 0.0f) clampedAlpha = 0.0f;
		if (clampedAlpha > 255.0f) clampedAlpha = 255.0f;
		int alpha = (int)clampedAlpha;
		
		Gdiplus::Color color(alpha, 0, 0, 0);
		Gdiplus::SolidBrush brush(color);
		
		// 사각형 파티클 회전하여 그리기
		Gdiplus::GraphicsState state = graphics.Save();
		
		// 파티클 중심으로 이동 후 회전
		graphics.TranslateTransform(p.x, p.y);
		graphics.RotateTransform(p.angle);
		
		// 중심 기준으로 사각형 그리기
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
