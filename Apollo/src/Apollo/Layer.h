#pragma once
#include "Apollo/core.h"
#include "Apollo/Events/Event.h"
#include "Apollo/core/Timestep.h"

namespace Apollo
{
	class APOLLO_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();
		virtual void OnAttach() {};
		virtual void OnDetach() {};
		virtual void OnUpdate(Timestep ts) {};
		virtual void OnImGuiRender() {};
		virtual void OnEvent(Event& event) {};
		inline const std::string& GetName() const { return m_debugName; }
	protected:
		std::string m_debugName;
	};
}