#pragma once
#include "Widget.h"
#include "Utility.h"

namespace gui
{
	class ShapeWidget : public Widget
	{
	public:

		virtual ~ShapeWidget()
		{

		}

		void GenerateVertexList(DrawList& drawList) override
		{
			if (!m_Visible)
				return;

			Vector2f position = m_GlobalBounds.position;
			Shape shape;
			switch (m_Type)
			{
			case Type::Circle:

				shape = gui::GenerateCircle(position, circle.radius, m_Colour);

				m_GlobalBounds.size = rectangle.size;


				break;
			case Type::Rectangle:

				shape = gui::GenerateQuad(position, position + rectangle.size, {}, {}, m_Colour);

				m_GlobalBounds.size = rectangle.size;

				break;
			case Type::RoundedRectangle:

				shape = gui::GenerateRoundedQuad(position, position + rectangle.size, m_Colour, rectangle.rounding);

				m_GlobalBounds.size = rectangle.size;

				break;
			}

			drawList.Add(shape.vertices, shape.indices);
		}

		void SetAsCircle(float radius)
		{
			m_Type = Type::Circle;
			circle.radius = radius;
		}

		void SetAsRectangle(Vector2f size)
		{
			m_Type = Type::Rectangle;
			rectangle.size = size;
			m_GlobalBounds.size = rectangle.size;
		}

		void SetAsRoundedRectangle(Vector2f size, float rounding)
		{
			m_Type = Type::RoundedRectangle;
			rectangle.size = size;
			rectangle.rounding = rounding;
			m_GlobalBounds.size = rectangle.size;
		}

	private:

		enum class Type
		{
			None,
			Circle,
			Rectangle,
			RoundedRectangle
		};

		Type m_Type = Type::None;


		struct
		{
			float radius;
		} circle;

		struct
		{
			Vector2f size;
			float rounding;
		} rectangle;


	};
}