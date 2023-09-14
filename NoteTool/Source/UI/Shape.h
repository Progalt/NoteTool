#pragma once
#include "Widget.h"
#include "Utility.h"

namespace gui
{
	class ShapeWidget : public Widget
	{
	public:

		void GenerateVertexList(DrawList& drawList) override
		{
			Vector2f position = m_GlobalBounds.position;
			Shape shape;
			switch (m_Type)
			{
			case Type::Circle:
				break;
			case Type::Rectangle:

				shape = gui::GenerateQuad(position, position + rectangle.size, {}, {}, m_Colour);

				break;
			}

			drawList.Add(shape.vertices, shape.indices);
		}

		void SetAsCircle(float radius)
		{
			m_Type = Type::Circle;
		}

		void SetAsRectangle(Vector2f size)
		{
			m_Type = Type::Rectangle;
			rectangle.size = size;
		}

	private:

		enum class Type
		{
			None,
			Circle,
			Rectangle
		};

		Type m_Type = Type::None;


		struct
		{
			float radius;
		} circle;

		struct
		{
			Vector2f size;
		} rectangle;


	};
}