#include "pch.h"
#include "SnowfallParticle.h"
#include <algorithm>

SnowfallParticleSystem::SnowfallParticleSystem()
	: m_pVirusImage(nullptr)
	, m_spawnRate(0.0f)
	, m_spawnAccumulator(0.0f)
{
}

SnowfallParticleSystem::~SnowfallParticleSystem()
{
	if (m_pVirusImage)
	{
		delete m_pVirusImage;
		m_pVirusImage = nullptr;
	}
}

bool SnowfallParticleSystem::LoadVirusImage(const CString& imagePath)
{
	if (m_pVirusImage)
	{
		delete m_pVirusImage;
		m_pVirusImage = nullptr;
	}
	
	m_pVirusImage = Gdiplus::Image::FromFile(imagePath);
	return (m_pVirusImage && m_pVirusImage->GetLastStatus() == Gdiplus::Ok);
}

void SnowfallParticleSystem::SetProductionRate(double zpcPerSecond)
{
	m_spawnRate = CalculateSpawnRate(zpcPerSecond);
}

float SnowfallParticleSystem::CalculateSpawnRate(double zpcPerSecond) const
{
	// ZPC/s 단계별 spawn rate (초당 파티클 수) - 성능 최적화
	if (zpcPerSecond < 1.0)
		return 0.3f;      // Tier 0: 0-1 ZPC/s
	else if (zpcPerSecond < 10.0)
		return 0.8f;      // Tier 1: 1-10 ZPC/s
	else if (zpcPerSecond < 50.0)
		return 1.5f;      // Tier 2: 10-50 ZPC/s
	else if (zpcPerSecond < 200.0)
		return 3.0f;      // Tier 3: 50-200 ZPC/s
	else if (zpcPerSecond < 1000.0)
		return 5.0f;      // Tier 4: 200-1000 ZPC/s
	else
		return 8.0f;      // Tier 5: 1000+ ZPC/s
}

void SnowfallParticleSystem::OnMouseClick(float x)
{
	// 클릭 시 1~2개 파티클 방출 (성능 최적화)
	int burstCount = 1 + (rand() % 2);
	for (int i = 0; i < burstCount; i++)
	{
		SpawnParticle(x);
	}
}

void SnowfallParticleSystem::SpawnParticle(float x)
{
	// x가 -1이면 완전 랜덤 위치 (0~800), 아니면 해당 X 위치
	float spawnX;
	if (x < 0.0f)
	{
		// 완전 랜덤 X 위치 (0.0 ~ 800.0)
		spawnX = (float)(rand()) / RAND_MAX * 800.0f;
	}
	else
	{
		spawnX = x;
	}
	float spawnY = -50.0f;  // 화면 위에서 시작
	
	m_particles.emplace_back(spawnX, spawnY);
}

void SnowfallParticleSystem::Update(float deltaTime)
{
	// Spawn rate에 따라 새 파티클 생성
	m_spawnAccumulator += m_spawnRate * deltaTime;
	while (m_spawnAccumulator >= 1.0f)
	{
		SpawnParticle();
		m_spawnAccumulator -= 1.0f;
	}
	
	// 모든 파티클 업데이트
	for (auto it = m_particles.begin(); it != m_particles.end(); )
	{
		SnowfallParticle& p = *it;
		
		// 물리 업데이트
		p.x += p.vx * deltaTime;
		p.y += p.vy * deltaTime;
		
		// 생명 감소 (시간 기반)
		p.life -= deltaTime / p.maxLife;
		
		// 화면 하단에 가까워질수록 페이드 아웃
		if (p.y > 500.0f)
		{
			// 500~650 구간에서 페이드
			float fadeProgress = (p.y - 500.0f) / 150.0f;
			if (fadeProgress > 1.0f) fadeProgress = 1.0f;
			p.alpha = 150.0f * (1.0f - fadeProgress);
		}
		else
		{
			p.alpha = 150.0f * p.life;
		}
		
		// 생명이 다했거나 화면 밖으로 나간 파티클 제거
		if (p.life <= 0.0f || p.y > 650.0f)
		{
			it = m_particles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void SnowfallParticleSystem::Draw(Gdiplus::Graphics& graphics)
{
	if (!m_pVirusImage || m_pVirusImage->GetLastStatus() != Gdiplus::Ok)
		return;
	
	for (const auto& p : m_particles)
	{
		// 알파값 클램핑
		float clampedAlpha = p.alpha;
		if (clampedAlpha < 0.0f) clampedAlpha = 0.0f;
		if (clampedAlpha > 255.0f) clampedAlpha = 255.0f;
		int alpha = (int)clampedAlpha;
		
		// 이미지 매트릭스 설정
		Gdiplus::ImageAttributes imgAttr;
		Gdiplus::ColorMatrix colorMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		imgAttr.SetColorMatrix(&colorMatrix);
		
		// 이미지 그리기 (크기 조절)
		Gdiplus::RectF destRect(
			p.x - p.size / 2.0f,
			p.y - p.size / 2.0f,
			p.size,
			p.size
		);
		
		graphics.DrawImage(
			m_pVirusImage,
			destRect,
			0.0f, 0.0f,
			(Gdiplus::REAL)m_pVirusImage->GetWidth(),
			(Gdiplus::REAL)m_pVirusImage->GetHeight(),
			Gdiplus::UnitPixel,
			&imgAttr
		);
	}
}

void SnowfallParticleSystem::Clear()
{
	m_particles.clear();
	m_spawnAccumulator = 0.0f;
}
