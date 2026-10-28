#include "Apollo.h"

#include "imgui/imgui.h"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class SandboxLayer : public Apollo::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_FloorTexture = Apollo::Texture2D::Create("assets/textures/floor.png");
		m_BlockTexture = Apollo::Texture2D::Create("assets/textures/block.png");

		m_Obstacles = {
			{ 1.50f, 0.90f },
			{ 1.50f, 1.15f },
			{ 1.50f, 1.40f },
			{ 1.50f, 1.65f },
			{ 1.50f, 1.90f },
			{ 2.80f, 2.40f },
			{ 3.05f, 2.40f },
			{ 3.30f, 2.40f },
			{ 3.55f, 2.40f },
			{ 4.40f, 1.10f },
			{ 4.40f, 1.35f },
			{ 4.40f, 1.60f }
		};
	}

	void OnAttach() override
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	}

	void OnUpdate(Apollo::Timestep ts) override
	{
		m_FrameSeconds = m_FrameSeconds * 0.9f + ts.GetSeconds() * 0.1f;

		const ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard)
		{
			glm::vec2 input(0.0f);
			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_W))
				input.y += 1.0f;
			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_S))
				input.y -= 1.0f;
			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_A))
				input.x -= 1.0f;
			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_D))
				input.x += 1.0f;

			if (input.x != 0.0f || input.y != 0.0f)
			{
				input = glm::normalize(input) * m_PlayerSpeed * ts.GetSeconds();
				TryMove(input.x, 0.0f);
				TryMove(0.0f, input.y);
			}

			float worldWidth = s_GridX * s_TileSize;
			float worldHeight = s_GridY * s_TileSize;
			m_PlayerPos.x = Clamp(m_PlayerPos.x, m_PlayerHalf, worldWidth - m_PlayerHalf);
			m_PlayerPos.y = Clamp(m_PlayerPos.y, m_PlayerHalf, worldHeight - m_PlayerHalf);

			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_Q))
				m_CameraRotation += m_CameraRotateSpeed * ts.GetSeconds();
			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_E))
				m_CameraRotation -= m_CameraRotateSpeed * ts.GetSeconds();
		}

		m_ReachedGoal = Overlaps(m_PlayerPos, m_PlayerHalf, m_GoalPos, m_GoalHalf);

		Apollo::RenderCommand::SetClearColor({ 0.08f, 0.08f, 0.1f, 1.0f });
		Apollo::RenderCommand::Clear();

		m_Camera.SetPosition({ m_PlayerPos.x, m_PlayerPos.y, 0.0f });
		m_Camera.SetRotation(m_CameraRotation);

		Apollo::Renderer2D::SetFlushEveryQuad(m_FlushEveryQuad);
		Apollo::Renderer2D::BeginScene(m_Camera);

		for (int y = 0; y < s_GridY; y++)
		{
			for (int x = 0; x < s_GridX; x++)
			{
				glm::vec2 center((x + 0.5f) * s_TileSize, (y + 0.5f) * s_TileSize);
				Apollo::Renderer2D::DrawQuad(center, { s_TileSize, s_TileSize }, m_FloorTexture);
			}
		}

		for (const glm::vec2& obstacle : m_Obstacles)
			Apollo::Renderer2D::DrawQuad(obstacle, { s_TileSize, s_TileSize }, m_BlockTexture);

		Apollo::Renderer2D::DrawQuad(m_GoalPos, { s_TileSize, s_TileSize }, { 0.2f, 0.85f, 0.95f, 1.0f });
		Apollo::Renderer2D::DrawQuad(m_PlayerPos, { m_PlayerHalf * 2.0f, m_PlayerHalf * 2.0f }, { 0.95f, 0.82f, 0.2f, 1.0f });

		Apollo::Renderer2D::EndScene();
	}

	void OnImGuiRender() override
	{
		auto stats = Apollo::Renderer2D::GetStats();
		float fps = m_FrameSeconds > 0.0f ? 1.0f / m_FrameSeconds : 0.0f;

		ImGui::Begin("Apollo");
		ImGui::Text(u8"\u5e27\u65f6\u95f4 %.2f ms   %.0f FPS", m_FrameSeconds * 1000.0f, fps);
		ImGui::Text("Draw calls: %u", stats.DrawCalls);
		ImGui::Text(u8"\u56db\u8fb9\u5f62: %u", stats.QuadCount);
		ImGui::Text(u8"\u5355\u6279\u6700\u591a\u7eb9\u7406: %u", stats.TextureCount);
		ImGui::Checkbox(u8"\u5173\u95ed\u5408\u6279\uff08\u6bcf\u4e2a\u56db\u8fb9\u5f62\u4e00\u6b21\u7ed8\u5236\uff09", &m_FlushEveryQuad);
		ImGui::Separator();
		ImGui::Text(u8"WASD \u79fb\u52a8\uff0cQ/E \u65cb\u8f6c\u76f8\u673a");
		ImGui::Text(u8"\u9ec4\u8272\u662f\u89d2\u8272\uff0c\u9752\u8272\u683c\u5b50\u662f\u7ec8\u70b9");
		if (m_ReachedGoal)
			ImGui::Text(u8"\u5230\u8fbe\u7ec8\u70b9");
		ImGui::End();
	}

private:
	static float Clamp(float value, float minValue, float maxValue)
	{
		if (value < minValue)
			return minValue;
		if (value > maxValue)
			return maxValue;
		return value;
	}

	static bool Overlaps(const glm::vec2& a, float aHalf, const glm::vec2& b, float bHalf)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		if (dx < 0.0f) dx = -dx;
		if (dy < 0.0f) dy = -dy;
		return dx < aHalf + bHalf && dy < aHalf + bHalf;
	}

	bool HitsObstacle(const glm::vec2& center) const
	{
		for (const glm::vec2& obstacle : m_Obstacles)
		{
			if (Overlaps(center, m_PlayerHalf, obstacle, m_ObstacleHalf))
				return true;
		}
		return false;
	}

	void TryMove(float dx, float dy)
	{
		glm::vec2 next = m_PlayerPos + glm::vec2(dx, dy);
		if (!HitsObstacle(next))
			m_PlayerPos = next;
	}

private:
	static constexpr int s_GridX = 24;
	static constexpr int s_GridY = 16;
	static constexpr float s_TileSize = 0.25f;

	Apollo::OrthographicCamera m_Camera;
	Apollo::Ref<Apollo::Texture2D> m_FloorTexture;
	Apollo::Ref<Apollo::Texture2D> m_BlockTexture;

	std::vector<glm::vec2> m_Obstacles;
	glm::vec2 m_PlayerPos = { 0.55f, 0.55f };
	glm::vec2 m_GoalPos = { 5.15f, 3.40f };

	float m_PlayerHalf = 0.09f;
	float m_ObstacleHalf = 0.12f;
	float m_GoalHalf = 0.12f;
	float m_PlayerSpeed = 1.6f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotateSpeed = 70.0f;
	float m_FrameSeconds = 0.0f;
	bool m_FlushEveryQuad = false;
	bool m_ReachedGoal = false;
};

class Sandbox : public Apollo::Application
{
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
	}
};

Apollo::Application* Apollo::createApplication()
{
	return new Sandbox();
}
