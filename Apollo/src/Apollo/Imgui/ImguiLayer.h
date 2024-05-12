#pragma once

#include "Apollo/Layer.h"

#include "Apollo/Events/ApplicationEvent.h"
#include "Apollo/Events/KeyEvent.h"
#include "Apollo/Events/MouseEvent.h"

namespace Apollo
{
	class APOLLO_API ImguiLayer : public Layer
	{
	public:
		ImguiLayer();
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};
}