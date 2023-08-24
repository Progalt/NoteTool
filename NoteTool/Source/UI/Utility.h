#pragma once
#include <vector>
#include "../Renderer/Renderer.h"

#include "Widget.h"

#include "../Font.h"

namespace gui
{
	struct Shape
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	inline Shape GenerateQuad(Vector2f min, Vector2f max, Vector2f uvMin, Vector2f uvMax, Colour col)
	{
		Shape output;
		output.indices = { 0, 1, 3, 1, 2, 3 };

		output.vertices = {
			{ min, uvMin, col },
			{ {max.x, min.y}, { uvMax.x, uvMin.y}, col},
			{ max, uvMax, col },
			{ {min.x, max.y}, { uvMin.x, uvMax.y}, col},
		};

		return output;
	}

	inline Shape GenerateRoundedQuad(Vector2f rmin, Vector2f rmax, Colour col, float rounding)
	{
		uint32_t detail = 8;

		float halfWidth = ((rmax.x - rmin.x) / 2.0f);
		float halfHeight = ((rmax.y - rmin.y) / 2.0f);

		Vector2f center = rmin + Vector2f{ halfWidth, halfHeight };

		float rectWidth = halfWidth - rounding;

		// basic center rect

		std::vector<Vertex> vertices;

		Vector2f min = { center.x - (float)rectWidth, center.y - (float)halfHeight };
		Vector2f max = { center.x + (float)rectWidth, center.y + (float)halfHeight };

		vertices.push_back({ { min.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { min.x, max.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, max.y }, { 0.0f, 0.0f }, col });

		std::vector<unsigned int> indices = { 0, 1, 2, 1, 2, 3 };


		// Draw the 2 side rectangles


		min = { center.x - (float)halfWidth, center.y - (float)halfHeight + (float)rounding };
		max = { center.x - (float)halfWidth + (float)rounding, center.y + (float)halfHeight - (float)rounding };

		uint32_t vertexOffset = 4;

		vertices.push_back({ { min.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { min.x, max.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, max.y }, { 0.0f, 0.0f }, col });

		indices.push_back(vertexOffset + 0);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 3);

		min = { center.x + (float)halfWidth, center.y - (float)halfHeight + (float)rounding };
		max = { center.x + (float)halfWidth - (float)rounding, center.y + (float)halfHeight - (float)rounding };

		vertices.push_back({ { min.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, min.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { min.x, max.y }, { 0.0f, 0.0f }, col });
		vertices.push_back({ { max.x, max.y }, { 0.0f, 0.0f }, col });

		vertexOffset = 8;

		indices.push_back(vertexOffset + 0);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 1);
		indices.push_back(vertexOffset + 2);
		indices.push_back(vertexOffset + 3);

		min = { center.x - (float)rectWidth, center.y - (float)halfHeight };
		max = { center.x + (float)rectWidth, center.y + (float)halfHeight };


		// Lets draw the rounded sections

		// this allows it to be done in loops

		// radians
		float cornerAngleBase[4] =
		{
			0.0,
			(M_PI / 2.0),
			(M_PI),
			((M_PI / 2.0) * 3.0)
		};

		Vector2f cornerCenters[4] =
		{
			{ max.x, max.y - (float)rounding },
			{ max.x, min.y + (float)rounding },
			{ min.x, min.y + (float)rounding },
			{ min.x, max.y - (float)rounding }
		};

		// loop through each corner and create the rounded edges

		for (uint32_t b = 0; b < 4; b++)
		{

			vertexOffset = vertices.size();

			vertices.push_back({ { cornerCenters[b].x, cornerCenters[b].y }, {0.0f, 0.0f}, col });

			// Top left corner
			for (uint32_t i = 0; i < detail + 1; i++)
			{
				// pi/2 radians or 90 degrees

				float angle = i * ((M_PI / 2.0) / static_cast<float>(detail));

				float s = sin(angle + cornerAngleBase[b]) * rounding;
				float c = cos(angle + cornerAngleBase[b]) * rounding;

				vertices.push_back({ { cornerCenters[b].x + (float)s, cornerCenters[b].y + (float)c}, {0.0f, 0.0f}, col });
			}

			for (uint32_t i = 1; i < detail + 1; i++)
			{
				indices.push_back(vertexOffset + 0);
				indices.push_back(vertexOffset + i);
				indices.push_back(vertexOffset + i + 1);
			}
		}

		Shape out;
		out.vertices = vertices;
		out.indices = indices;

		return out;
	}

	inline void RenderText(DrawList& list, const std::string& Text, Font* font)
	{

	}
}