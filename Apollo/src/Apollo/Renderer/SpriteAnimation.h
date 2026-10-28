#pragma once

#include "Apollo/Renderer/SubTexture2D.h"

#include <vector>

namespace Apollo
{
	class SpriteAnimation
	{
	public:
		SpriteAnimation() = default;
		SpriteAnimation(std::vector<Ref<SubTexture2D>> frames, float secondsPerFrame);

		void Update(float seconds, bool playing);
		void Reset();

		const Ref<SubTexture2D>& GetFrame() const;
		uint32_t GetFrameIndex() const { return m_FrameIndex; }
		uint32_t GetFrameCount() const { return (uint32_t)m_Frames.size(); }
	private:
		std::vector<Ref<SubTexture2D>> m_Frames;
		float m_SecondsPerFrame = 0.12f;
		float m_Time = 0.0f;
		uint32_t m_FrameIndex = 0;
	};
}
