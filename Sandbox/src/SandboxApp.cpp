#include "Apollo.h"
#include "Level.h"

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
		m_Atlas = Apollo::Texture2D::Create("assets/textures/atlas.png");

		const glm::vec2 cell(32.0f, 32.0f);
		m_IdleSprite = Apollo::SubTexture2D::CreateFromCoords(m_Atlas, { 0.0f, 0.0f }, cell);
		std::vector<Apollo::Ref<Apollo::SubTexture2D>> walkFrames;
		for (int i = 1; i <= 3; i++)
			walkFrames.push_back(Apollo::SubTexture2D::CreateFromCoords(m_Atlas, { (float)i, 0.0f }, cell));
		m_PlayerAnimation = Apollo::SpriteAnimation(std::move(walkFrames), 0.12f);
		m_GoalSprite = Apollo::SubTexture2D::CreateFromCoords(m_Atlas, { 4.0f, 0.0f }, cell);

		m_LevelPath = "assets/levels/room.txt";
		ReloadLevel();
	}

	void OnAttach() override
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

		Apollo::FramebufferSpecification spec;
		spec.Width = 1280;
		spec.Height = 720;
		m_Framebuffer = Apollo::Framebuffer::Create(spec);
	}

	void OnUpdate(Apollo::Timestep ts) override
	{
		m_FrameSeconds = m_FrameSeconds * 0.9f + ts.GetSeconds() * 0.1f;

		const ImGuiIO& io = ImGui::GetIO();
		m_Moving = false;
		if (m_ViewportFocused || !io.WantCaptureKeyboard)
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

			m_Moving = input.x != 0.0f || input.y != 0.0f;
			if (input.x < 0.0f)
				m_FaceLeft = true;
			else if (input.x > 0.0f)
				m_FaceLeft = false;

			if (input.x != 0.0f || input.y != 0.0f)
			{
				input = glm::normalize(input) * m_PlayerSpeed * ts.GetSeconds();
				TryMove(input.x, 0.0f);
				TryMove(0.0f, input.y);
			}

			float worldWidth = m_Level.GetWidth() * m_Level.GetTileSize();
			float worldHeight = m_Level.GetHeight() * m_Level.GetTileSize();
			m_PlayerPos.x = Clamp(m_PlayerPos.x, m_PlayerHalf, worldWidth - m_PlayerHalf);
			m_PlayerPos.y = Clamp(m_PlayerPos.y, m_PlayerHalf, worldHeight - m_PlayerHalf);

			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_Q))
				m_CameraRotation += m_CameraRotateSpeed * ts.GetSeconds();
			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_E))
				m_CameraRotation -= m_CameraRotateSpeed * ts.GetSeconds();

			if (Apollo::Input::IsKeyPressed(APOLLO_KEY_R))
			{
				if (!m_ReloadHeld)
					ReloadLevel();
				m_ReloadHeld = true;
			}
			else
			{
				m_ReloadHeld = false;
			}
		}

		m_ReachedGoal = Overlaps(m_PlayerPos, m_PlayerHalf, m_GoalPos, m_GoalHalf);
		m_PlayerAnimation.Update(ts.GetSeconds(), m_Moving);

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
		{
			uint32_t width = (uint32_t)m_ViewportSize.x;
			uint32_t height = (uint32_t)m_ViewportSize.y;
			Apollo::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			if (spec.Width != width || spec.Height != height)
			{
				m_Framebuffer->Resize(width, height);
				float aspect = m_ViewportSize.x / m_ViewportSize.y;
				constexpr float orthoHeight = 1.8f;
				float orthoWidth = orthoHeight * aspect;
				m_Camera.SetProjection(-orthoWidth * 0.5f, orthoWidth * 0.5f, -orthoHeight * 0.5f, orthoHeight * 0.5f);
			}
		}

		m_Framebuffer->Bind();
		Apollo::RenderCommand::SetClearColor({ 0.08f, 0.08f, 0.1f, 1.0f });
		Apollo::RenderCommand::Clear();

		m_Camera.SetPosition({ m_PlayerPos.x, m_PlayerPos.y, 0.0f });
		m_Camera.SetRotation(m_CameraRotation);

		Apollo::Renderer2D::SetFlushEveryQuad(m_FlushEveryQuad);
		Apollo::Renderer2D::BeginScene(m_Camera);

		float tile = m_Level.GetTileSize();
		for (int y = 0; y < m_Level.GetHeight(); y++)
		{
			for (int x = 0; x < m_Level.GetWidth(); x++)
			{
				glm::vec2 center((x + 0.5f) * tile, (y + 0.5f) * tile);
				Apollo::Renderer2D::DrawQuad(center, { tile, tile }, m_FloorTexture);
			}
		}

		for (const glm::vec2& obstacle : m_Level.GetObstacles())
			Apollo::Renderer2D::DrawQuad(obstacle, { tile, tile }, m_BlockTexture);

		Apollo::Renderer2D::DrawQuad(m_GoalPos, { 0.36f, 0.36f }, m_GoalSprite);
		const Apollo::Ref<Apollo::SubTexture2D>& playerSprite = m_Moving ? m_PlayerAnimation.GetFrame() : m_IdleSprite;
		Apollo::Renderer2D::DrawQuad(m_PlayerPos, { 0.42f, 0.42f }, playerSprite, glm::vec4(1.0f), m_FaceLeft);

		Apollo::Renderer2D::EndScene();
		m_Framebuffer->Unbind();

		Apollo::RenderCommand::SetClearColor({ 0.12f, 0.12f, 0.12f, 1.0f });
		Apollo::RenderCommand::Clear();
	}

	void OnImGuiRender() override
	{
		auto stats = Apollo::Renderer2D::GetStats();
		float fps = m_FrameSeconds > 0.0f ? 1.0f / m_FrameSeconds : 0.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(u8"\u89c6\u53e3");
		m_ViewportFocused = ImGui::IsWindowFocused();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		if (viewportPanelSize.x > 0.0f && viewportPanelSize.y > 0.0f)
		{
			ImTextureID texture = (ImTextureID)(uintptr_t)m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image(texture, viewportPanelSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Apollo");
		ImGui::Text(u8"\u5e27\u65f6\u95f4 %.2f ms   %.0f FPS", m_FrameSeconds * 1000.0f, fps);
		ImGui::Text("Draw calls: %u", stats.DrawCalls);
		ImGui::Text(u8"\u56db\u8fb9\u5f62: %u", stats.QuadCount);
		ImGui::Text(u8"\u5355\u6279\u6700\u591a\u7eb9\u7406: %u", stats.TextureCount);
		ImGui::Checkbox(u8"\u5173\u95ed\u5408\u6279\uff08\u6bcf\u4e2a\u56db\u8fb9\u5f62\u4e00\u6b21\u7ed8\u5236\uff09", &m_FlushEveryQuad);
		ImGui::Separator();
		ImGui::Text(u8"WASD \u79fb\u52a8\uff0cQ/E \u65cb\u8f6c\u76f8\u673a");
		ImGui::Text(u8"\u5173\u5361 %s", m_LevelPath.c_str());
		ImGui::Text(u8"R \u91cd\u65b0\u8bfb\u53d6\u5173\u5361");
		ImGui::Text(m_Moving ? u8"\u72b6\u6001: \u8d70\u52a8" : u8"\u72b6\u6001: \u7ad9\u7acb");
		ImGui::Text(u8"\u56fe\u96c6\u5e27 %u", m_Moving ? m_PlayerAnimation.GetFrameIndex() + 1u : 0u);
		ImGui::Text(u8"\u89d2\u8272\u548c\u65d7\u5e1c\u5171\u7528\u4e00\u5f20\u56fe\u96c6");
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
		for (const glm::vec2& obstacle : m_Level.GetObstacles())
		{
			if (Overlaps(center, m_PlayerHalf, obstacle, m_ObstacleHalf))
				return true;
		}
		return false;
	}

	void ReloadLevel()
	{
		Level next;
		if (!next.Load(m_LevelPath))
			return;

		m_Level = std::move(next);
		m_PlayerPos = m_Level.GetPlayer();
		m_GoalPos = m_Level.GetGoal();
		m_CameraRotation = 0.0f;
		m_ReachedGoal = false;
		m_Moving = false;
		m_FaceLeft = false;
		m_PlayerAnimation.Reset();
	}

	void TryMove(float dx, float dy)
	{
		glm::vec2 next = m_PlayerPos + glm::vec2(dx, dy);
		if (!HitsObstacle(next))
			m_PlayerPos = next;
	}

private:
	Apollo::OrthographicCamera m_Camera;
	Apollo::Ref<Apollo::Framebuffer> m_Framebuffer;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	bool m_ViewportFocused = true;
	Apollo::Ref<Apollo::Texture2D> m_FloorTexture;
	Apollo::Ref<Apollo::Texture2D> m_BlockTexture;
	Apollo::Ref<Apollo::Texture2D> m_Atlas;
	Apollo::Ref<Apollo::SubTexture2D> m_IdleSprite;
	Apollo::Ref<Apollo::SubTexture2D> m_GoalSprite;
	Apollo::SpriteAnimation m_PlayerAnimation;

	std::string m_LevelPath;
	Level m_Level;
	glm::vec2 m_PlayerPos = { 0.0f, 0.0f };
	glm::vec2 m_GoalPos = { 0.0f, 0.0f };

	float m_PlayerHalf = 0.09f;
	float m_ObstacleHalf = 0.12f;
	float m_GoalHalf = 0.12f;
	float m_PlayerSpeed = 1.6f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotateSpeed = 70.0f;
	float m_FrameSeconds = 0.0f;
	bool m_FlushEveryQuad = false;
	bool m_ReachedGoal = false;
	bool m_Moving = false;
	bool m_FaceLeft = false;
	bool m_ReloadHeld = false;
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
