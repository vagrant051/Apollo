#include "Apollo.h"

#include "imgui/imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer :public Apollo::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_SquarePosition(0.0f)
	{
		m_VertexArray.reset(Apollo::VertexArray::Create());

		float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Apollo::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Apollo::VertexBuffer::Create(vertices, sizeof(vertices)));

		Apollo::BufferLayout layout = {
						{ Apollo::ShaderDataType::Float3, "a_Position" },
						{ Apollo::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0 , 1 , 2 };

		std::shared_ptr<Apollo::IndexBuffer> indexBuffer;
		indexBuffer.reset(Apollo::IndexBuffer::Create(indices, 3));

		m_VertexArray->SetIndexBuffer(indexBuffer);

		// 着色器代码
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(Apollo::Shader::Create(vertexSrc, fragmentSrc));

		///////////////////////////////////////////////////////////////

		m_SquareVertexArray.reset(Apollo::VertexArray::Create());

		float squareVertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f
		};

		std::shared_ptr<Apollo::VertexBuffer> squareVertexBuffer;
		squareVertexBuffer.reset(Apollo::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		Apollo::BufferLayout squareLayout = {
						{ Apollo::ShaderDataType::Float3, "a_Position" },
						{ Apollo::ShaderDataType::Float2, "a_Texture" }
		};
		squareVertexBuffer->SetLayout(squareLayout);

		m_SquareVertexArray->AddVertexBuffer(squareVertexBuffer);

		uint32_t squareIndices[6] = { 0 , 1 , 2 ,
									  1 , 3 , 2 };

		std::shared_ptr<Apollo::IndexBuffer> squareIndexBuffer;
		squareIndexBuffer.reset(Apollo::IndexBuffer::Create(squareIndices, 6));

		m_SquareVertexArray->SetIndexBuffer(squareIndexBuffer);

		// 着色器代码
		std::string squareVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string squareFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0f);
			}
		)";

		m_SquareShader.reset(Apollo::Shader::Create(squareVertexSrc, squareFragmentSrc));

		///////////////////////////////////////////////////////////////

		std::string textShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_Texture;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec2 v_Texture;

			void main()
			{
				v_Texture = a_Texture;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string textShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			uniform vec3 u_Color;

			in vec2 v_Texture;

			void main()
			{
				color = vec4(v_Texture, 0.0f, 1.0f);
			}
		)";

		m_TextureShader.reset(Apollo::Shader::Create(textShaderVertexSrc, textShaderFragmentSrc));
	}
	void OnUpdate(Apollo::Timestep ts) override
	{
		//APOLLO_TRACE("DeltaTime: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliSeconds());

		if (Apollo::Input::IsKeyPressed(APOLLO_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraSpeed * ts;
		}
		else if (Apollo::Input::IsKeyPressed(APOLLO_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraSpeed * ts;
		}

		if (Apollo::Input::IsKeyPressed(APOLLO_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraSpeed * ts;
		}
		else if (Apollo::Input::IsKeyPressed(APOLLO_KEY_UP))
		{
			m_CameraPosition.y += m_CameraSpeed * ts;
		}

		if (Apollo::Input::IsKeyPressed(APOLLO_KEY_A))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts;
		}
		else if (Apollo::Input::IsKeyPressed(APOLLO_KEY_D))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		}

		/*if (Apollo::Input::IsKeyPressed(APOLLO_KEY_L))
		{
			m_SquarePosition.x += m_SquareMoveSpeed * ts;
		}
		else if (Apollo::Input::IsKeyPressed(APOLLO_KEY_J))
		{
			m_SquarePosition.x -= m_SquareMoveSpeed * ts;
		}

		if (Apollo::Input::IsKeyPressed(APOLLO_KEY_I))
		{
			m_SquarePosition.y += m_SquareMoveSpeed * ts;
		}
		else if (Apollo::Input::IsKeyPressed(APOLLO_KEY_K))
		{
			m_SquarePosition.y -= m_SquareMoveSpeed * ts;
		}*/

		Apollo::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Apollo::RenderCommand::Clear();

		m_Camera.SetRotation(m_CameraRotation);
		m_Camera.SetPosition(m_CameraPosition);

		Apollo::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Apollo::OpenGLShader>(m_SquareShader)->Bind();
		std::dynamic_pointer_cast<Apollo::OpenGLShader>(m_SquareShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				glm::mat4 transform = scale * glm::translate(glm::mat4(1.0f), glm::vec3(1.1 * (float)i, 1.1 * (float)j, 0.0f));

				Apollo::Renderer::Submit(m_SquareShader, m_SquareVertexArray, transform);
			}
		}

		Apollo::Renderer::Submit(m_TextureShader, m_SquareVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Apollo::Renderer::Submit(m_Shader, m_VertexArray);

		Apollo::Renderer::EndScene();

	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("SquareColor", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Apollo::Event& event) override
	{
		
	}

private:
	Apollo::Ref<Apollo::Shader> m_Shader;
	Apollo::Ref<Apollo::VertexArray> m_VertexArray;

	Apollo::Ref<Apollo::Shader> m_SquareShader, m_TextureShader;
	Apollo::Ref<Apollo::VertexArray> m_SquareVertexArray;

	Apollo::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition = { 0.0f , 0.0f , 0.0f };
	float m_CameraSpeed = 1.0f;

	float m_CameraRotation = 0.0f;  
	float m_CameraRotationSpeed = 10.0f;

	glm::vec3 m_SquarePosition;
	float m_SquareMoveSpeed = 3.0f;

	glm::vec3 m_SquareColor = glm::vec3(0.2f, 0.3f, 0.6f);
};

class Sandbox : public Apollo::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Apollo::Application* Apollo::createApplication()
{
	return new Sandbox();
}