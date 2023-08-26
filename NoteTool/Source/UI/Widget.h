#pragma once

#include <vector>
#include <string>
#include "../Renderer/Renderer.h"
#include "../Maths/Rect.h"
#include "EventHandler.h"

namespace gui
{
	class DrawList
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		struct Call
		{
			GPUTexture* texture;
			uint32_t firstIndex, indexCount;
			IntRect scissor;
		};

		std::vector<Call> drawcalls;

		void SetScissor(IntRect scissor)
		{
			m_CurrentScissor = scissor;
		}

		void Add(std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices, GPUTexture* tex = nullptr)
		{
			if (m_CurrentCall == nullptr)
			{
				drawcalls.push_back(Call());

				m_CurrentCall = &drawcalls[drawcalls.size() - 1ui64];
				m_CurrentCall->texture = tex;
				m_CurrentCall->scissor = m_CurrentScissor;

				m_CurrentCall->firstIndex = 0;
			}

			if (m_CurrentCall->texture != tex || m_CurrentCall->scissor != m_CurrentScissor)
			{
				drawcalls.push_back(Call());

				m_CurrentCall = &drawcalls[drawcalls.size() - 1ui64];
				m_CurrentCall->texture = tex;
				m_CurrentCall->scissor = m_CurrentScissor;

				m_CurrentCall->firstIndex = indices.size();
			}

			size_t offset = vertices.size();

			std::vector<uint32_t> offsetIndices;

			for (auto& idx : newIndices)
				offsetIndices.push_back(idx + offset);

			vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
			indices.insert(indices.end(), offsetIndices.begin(), offsetIndices.end());

			m_CurrentCall->indexCount += newIndices.size();
		}

	private:

		Call* m_CurrentCall;
		IntRect m_CurrentScissor = IntRect();
		
	};

	enum class Alignment
	{
		Left, 
		Centre,
		Right
	};

	enum class Anchor
	{
		None, 
		TopLeft, 
		TopCentre,
		TopRight,
		CentreLeft,
		Centre, 
		CentreRight, 
		BottomLeft, 
		BottomCentre, 
		BottomRight
	};

	class Widget
	{
	public:

		~Widget()
		{
			for (auto& child : m_Children)
				delete child;
		}

		virtual void GenerateVertexList(DrawList& drawList) = 0;

		virtual void OnEvent() { }

		void HandleEvents()
		{
			// TODO: Add more anchor points

			if (EventHandler::resizeEvent)
			{
				switch (m_Anchor)
				{
				case Anchor::Centre:
				{
					if (m_Parent)
					{
						Vector2f widgetCentre = m_Bounds.position + (m_Bounds.size / 2.0f);

						Vector2f parentCentre = Vector2f(m_Parent->m_Bounds.w, m_Parent->m_Bounds.h) / 2.0f;

						Vector2f distanceToCentre = parentCentre - widgetCentre;


						m_Bounds.position += distanceToCentre;
						RecalculateAllBounds();

					}
				}
				break;
				}
			}

			/*if (m_Parent)
			{
				if (m_Parent->GetBounds().Contains(m_GlobalBounds))
					OnEvent();
			}
			else */
				OnEvent();

			for (auto& child : m_Children)
			{
				if (child->m_Visible)
				{
					child->HandleEvents();
				}
			}
		}

		template<typename _Ty, class ... Args>
		_Ty* NewChild(Args... args)
		{
			Widget* w = new _Ty(args...);

			m_Children.push_back(w);
			w->m_Parent = this;

			return static_cast<_Ty*>(w);
		}

		void SetName(const std::string& name) { m_Name = name; }


		void SetBounds(FloatRect bounds) 
		{ 
			m_Bounds = bounds; 
			m_GlobalBounds = m_Bounds;
			if (m_Parent)
				m_GlobalBounds.position += m_Parent->GetBounds().position - m_Parent->m_VisibleOffset;

			//RecalculateAllBounds();
		}

		void SetPosition(Vector2f pos)
		{
			SetBounds({ pos.x, pos.y, m_Bounds.x, m_Bounds.y });
			RecalculateAllBounds();
		}

		FloatRect GetBounds() { return m_GlobalBounds; }

		void SetRounding(float rounding) { m_Rounding = rounding; }

		void SetColour(Colour col) { m_Colour = col; }


		void SetTransparency(float transparency)
		{
			m_Transparency = transparency;

			// Force calculate the global transparency
			GetTransparency();
		}

		float GetTransparency()
		{
			m_GlobalTransparency = m_Transparency;
			if (m_Parent)
			{
				float parentGlobalTransparency = m_Parent->GetTransparency();

				m_GlobalTransparency *= parentGlobalTransparency;
			}

			return m_GlobalTransparency;
		}

		void SetAnchor(Anchor anchor) { m_Anchor = anchor; }

		void RecalculateAllBounds()
		{
			SetBounds(m_Bounds);

			for (auto& child : m_Children)
				child->RecalculateAllBounds();
		}

		void SetVisible(bool vis) { m_Visible = vis; }

	protected:

		bool m_Hovered = false;

		bool m_Visible = true;

		FloatRect m_Bounds;
		FloatRect m_GlobalBounds;

		bool m_HandleChildEventsOutsideBounds = false;

		void GenerateChildVertexLists(DrawList& drawList)
		{
			for (auto& child : m_Children)
				child->GenerateVertexList(drawList);
		}

		std::vector<Widget*> m_Children;
		Widget* m_Parent;

		std::string m_Name;


		float m_Rounding = 0.0f;
		Colour m_Colour;

		float m_Transparency = 1.0f;

		float m_GlobalTransparency = 1.0f;

		Anchor m_Anchor = Anchor::None;

		Vector2f m_VisibleOffset = Vector2f(0.0f, 0.0f);
	};
}