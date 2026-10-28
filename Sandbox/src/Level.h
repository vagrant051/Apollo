#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Level
{
public:
	bool Load(const std::string& path);

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	float GetTileSize() const { return m_TileSize; }
	const glm::vec2& GetPlayer() const { return m_Player; }
	const glm::vec2& GetGoal() const { return m_Goal; }
	const std::vector<glm::vec2>& GetObstacles() const { return m_Obstacles; }

private:
	int m_Width = 0;
	int m_Height = 0;
	float m_TileSize = 0.25f;
	glm::vec2 m_Player = { 0.0f, 0.0f };
	glm::vec2 m_Goal = { 0.0f, 0.0f };
	std::vector<glm::vec2> m_Obstacles;
};
