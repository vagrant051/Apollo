#include "pch.h"
#include "SpriteAnimation.h"

namespace Apollo
{
	SpriteAnimation::SpriteAnimation(std::vector<Ref<SubTexture2D>> frames, float secondsPerFrame)
		: m_Frames(std::move(frames)), m_SecondsPerFrame(secondsPerFrame)
	{
	}

	void SpriteAnimation::Update(float seconds, bool playing)
	{
		if (m_Frames.empty())
			return;

		if (!playing)
		{
			Reset();
			return;
		}

		m_Time += seconds;
		float length = m_SecondsPerFrame * (float)m_Frames.size();
		if (length > 0.0f)
		{
			while (m_Time >= length)
				m_Time -= length;
		}

		m_FrameIndex = m_SecondsPerFrame > 0.0f ? (uint32_t)(m_Time / m_SecondsPerFrame) : 0;
		if (m_FrameIndex >= m_Frames.size())
			m_FrameIndex = (uint32_t)m_Frames.size() - 1;
	}

	void SpriteAnimation::Reset()
	{
		m_Time = 0.0f;
		m_FrameIndex = 0;
	}

	const Ref<SubTexture2D>& SpriteAnimation::GetFrame() const
	{
		APOLLO_CORE_ASSERT(!m_Frames.empty(), "SpriteAnimation has no frames");
		return m_Frames[m_FrameIndex];
	}
}
