#include "Level.h"

#include "Apollo/Log.h"

#include <fstream>
#include <sstream>

namespace
{
	std::string Trim(const std::string& text)
	{
		size_t begin = 0;
		while (begin < text.size() && (text[begin] == ' ' || text[begin] == '\t'))
			begin++;

		size_t end = text.size();
		while (end > begin && (text[end - 1] == ' ' || text[end - 1] == '\t'))
			end--;

		return text.substr(begin, end - begin);
	}

	bool IsMapRow(const std::string& text)
	{
		if (text.empty())
			return false;

		for (char cell : text)
		{
			if (cell != '.' && cell != '#' && cell != '@' && cell != 'G' && cell != 'g')
				return false;
		}
		return true;
	}
}

bool Level::Load(const std::string& path)
{
	std::ifstream file(path);
	if (!file)
	{
		APOLLO_ERROR("Failed to open level: {0}", path);
		return false;
	}

	float tileSize = 0.25f;
	std::vector<std::string> rows;
	bool sawMap = false;
	int lineNumber = 0;
	std::string line;
	while (std::getline(file, line))
	{
		lineNumber++;
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		std::string trimmed = Trim(line);
		if (trimmed.empty() || trimmed.rfind("//", 0) == 0)
			continue;

		if (!sawMap && trimmed.rfind("tile", 0) == 0)
		{
			std::istringstream stream(trimmed.substr(4));
			float size = 0.0f;
			stream >> size;
			if (!stream || size <= 0.0f)
			{
				APOLLO_ERROR("Level {0}:{1}: tile size must be positive", path, lineNumber);
				return false;
			}
			tileSize = size;
			continue;
		}

		if (!IsMapRow(trimmed))
		{
			APOLLO_ERROR("Level {0}:{1}: expected a row of . # @ G", path, lineNumber);
			return false;
		}

		if (!rows.empty() && trimmed.size() != rows.front().size())
		{
			APOLLO_ERROR("Level {0}:{1}: row width is {2}, expected {3}", path, lineNumber, trimmed.size(), rows.front().size());
			return false;
		}

		sawMap = true;
		rows.push_back(trimmed);
	}

	if (rows.empty())
	{
		APOLLO_ERROR("Level {0} has no tiles", path);
		return false;
	}

	int width = (int)rows.front().size();
	int height = (int)rows.size();
	glm::vec2 player(0.0f);
	glm::vec2 goal(0.0f);
	bool hasPlayer = false;
	bool hasGoal = false;
	std::vector<glm::vec2> obstacles;

	for (int row = 0; row < height; row++)
	{
		int worldY = height - 1 - row;
		for (int x = 0; x < width; x++)
		{
			char cell = rows[row][x];
			glm::vec2 center((x + 0.5f) * tileSize, (worldY + 0.5f) * tileSize);
			if (cell == '#')
			{
				obstacles.push_back(center);
			}
			else if (cell == '@')
			{
				if (hasPlayer)
				{
					APOLLO_ERROR("Level {0} has more than one player", path);
					return false;
				}
				hasPlayer = true;
				player = center;
			}
			else if (cell == 'G' || cell == 'g')
			{
				if (hasGoal)
				{
					APOLLO_ERROR("Level {0} has more than one goal", path);
					return false;
				}
				hasGoal = true;
				goal = center;
			}
		}
	}

	if (!hasPlayer || !hasGoal)
	{
		APOLLO_ERROR("Level {0} needs one @ and one G", path);
		return false;
	}

	m_Width = width;
	m_Height = height;
	m_TileSize = tileSize;
	m_Player = player;
	m_Goal = goal;
	m_Obstacles = std::move(obstacles);
	APOLLO_INFO("Loaded level {0} ({1}x{2}, {3} walls)", path, m_Width, m_Height, m_Obstacles.size());
	return true;
}
