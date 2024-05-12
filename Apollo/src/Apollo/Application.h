#pragma once

#include "core.h"
#include "Apollo/Windows.h"

#include "Apollo/Events/Event.h"
#include "Apollo/Events/ApplicationEvent.h"
#include "Apollo/LayerStack.h"

#include "Apollo/Imgui/ImguiLayer.h"

namespace Apollo
{
	class APOLLO_API Application
	{
	public:
		Application();
		virtual ~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		void run();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		ImguiLayer* m_ImGuiLayer; // ÓµÓÐImGuiLayer¿ØÖÆÈ¨
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* createApplication();
}