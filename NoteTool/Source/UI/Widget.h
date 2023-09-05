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
			uint32_t firstIndex, indexCount, firstVertex;
			IntRect scissor;
		};

		std::vector<Call> drawcalls;

		void SetScissor(IntRect scissor)
		{
			m_CurrentScissor = scissor;
		}

		IntRect GetCurrentScissor()
		{
			return m_CurrentScissor;
		}

		void Add(std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices, GPUTexture* tex = nullptr)
		{
			if (m_CurrentCall == nullptr)
			{
				drawcalls.push_back(Call());

				m_CurrentCall = &drawcalls[drawcalls.size() - 1ui64];
				m_CurrentCall->texture = tex;
				m_CurrentCall->scissor = m_CurrentScissor;
				m_CurrentCall->firstVertex = vertices.size();

				m_CurrentCall->firstIndex = 0;
			}

			if (m_CurrentCall->texture != tex || m_CurrentCall->scissor != m_CurrentScissor)
			{
				drawcalls.push_back(Call());

				m_CurrentCall = &drawcalls[drawcalls.size() - 1ui64];
				m_CurrentCall->texture = tex;
				m_CurrentCall->scissor = m_CurrentScissor;
				m_CurrentCall->firstVertex = vertices.size();

				m_CurrentCall->firstIndex = indices.size();
			}

			size_t offset = vertices.size();



			std::vector<uint32_t> offsetIndices(newIndices.size());
			std::copy(newIndices.begin(), newIndices.end(), offsetIndices.begin());

			for (auto& idx : offsetIndices)
				idx += offset;

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

		void RemoveChildren(uint32_t first = 0)
		{
			for (uint32_t i = first; i < m_Children.size(); i++)
			{
				delete m_Children[i];
			}

			m_Children.erase(m_Children.begin() + first, m_Children.end());
		}

		template<typename _Ty>
		void RemoveChild(_Ty* widget)
		{
			bool found = false;
			uint32_t i = 0;
			for (i < m_Children.size(); i++;)
			{
				if (static_cast<_Ty*>(m_Children[i]) == widget)
				{
					found = true;
					break;
				}
			}

			if (found)
			{
				delete m_Children[i];
				m_Children.erase(m_Children.begin() + i);
			}
		}

		void HandleEvents()
		{
			// TODO: Add more anchor points


			if (EventHandler::resizeEvent)
			{
				HandleResize();

			}



			/*if (m_Parent)
			{
				if (m_Parent->GetBounds().Contains(m_GlobalBounds))
					OnEvent();
			}
			else */

			if (m_Visible)
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

		void SetOldBounds(FloatRect bounds)
		{
			m_OldBounds = bounds;
		}

		void SetBounds(FloatRect bounds) 
		{ 
			m_Bounds = bounds; 
			m_GlobalBounds = m_Bounds;
			if (m_Parent)
				m_GlobalBounds.position += m_Parent->GetBounds().position - m_Parent->m_VisibleOffset;

		}

		void SetPosition(Vector2f pos)
		{
			//SetBounds({ pos.x, pos.y, m_Bounds.w, m_Bounds.h });
			m_Bounds.position = pos;
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

		void SetAnchor(Anchor anchor) { m_Anchor = anchor; CalculateAnchorOffsets(); }

		void RecalculateAllBounds()
		{
			SetBounds(m_Bounds);

			/*if (m_Parent)
			{
				if (m_Bounds.x + m_Bounds.w > m_Parent->GetBounds().w)
				{
					float dif = m_Parent->GetBounds().w - (m_Bounds.x + m_Bounds.w);
					m_Bounds.w += dif;
				}
			}*/


			for (auto& child : m_Children)
				child->RecalculateAllBounds();
		}

		void SetVisible(bool vis) { m_Visible = vis; }

		void SetLockSize(bool lock) { m_LockSize = lock; }
		void SetLockPosition(bool lock) { m_LockPosition = lock; }

	protected:

		Vector2f m_MinOffsetFromAnchor;		// Position
		Vector2f m_MaxOffsetFromAnchor;		// Position + Size
	
		Vector2f GetAnchorPosition()
		{
			if (!m_Parent)
				return Vector2f();

			FloatRect parentBounds = m_Parent->GetBounds();

			Vector2f anchorPosition{};

			// Get anchor position
			switch (m_Anchor)
			{
			case Anchor::Centre:

				anchorPosition = parentBounds.size / 2.0f;

				break;
			case Anchor::TopLeft:

				anchorPosition = { 0.0f, 0.0f };

				break;
			case Anchor::TopRight:

				anchorPosition = { parentBounds.w, 0.0f };

				break;
			case Anchor::BottomRight:

				anchorPosition = parentBounds.size;

				break;
			case Anchor::BottomLeft:

				anchorPosition = { 0.0f, parentBounds.h };

				break;
			case Anchor::CentreLeft:
				anchorPosition = { 0.0f, parentBounds.h / 2.0f };
				break;
			case Anchor::BottomCentre:
				anchorPosition = { parentBounds.w / 2.0f, parentBounds.h };
				break;
			}

			return anchorPosition;
		}

		void CalculateAnchorOffsets()
		{
			Vector2f anchorPosition = GetAnchorPosition();

			// Get distance from out two min max points to anchor

			m_MinOffsetFromAnchor = m_Bounds.position - anchorPosition;
			m_MaxOffsetFromAnchor = m_Bounds.position + m_Bounds.size - anchorPosition;

		}

		void HandleResize()
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
		
			case Anchor::BottomRight:
			case Anchor::BottomLeft:
			case Anchor::BottomCentre:
			case Anchor::TopLeft:
			case Anchor::TopRight:
			case Anchor::CentreLeft:
			

				if (m_Parent)
				{
					Vector2f anchor = GetAnchorPosition();

					if (!m_LockPosition)
					{
						m_Bounds.position = anchor + m_MinOffsetFromAnchor;
						
					}

					if (!m_LockSize)
					{
						m_Bounds.size = (anchor + m_MaxOffsetFromAnchor) - m_Bounds.position;

						
					}
					RecalculateAllBounds();
				}

				break;
			}

		}

		bool m_Hovered = false;

		bool m_Visible = true;

		FloatRect m_Bounds;
		FloatRect m_GlobalBounds;
		FloatRect m_OldBounds;

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
		Colour m_Colour = { 1.0f, 1.0f, 1.0f, 1.0f };

		float m_Transparency = 1.0f;

		float m_GlobalTransparency = 1.0f;

		Anchor m_Anchor = Anchor::None;

		Vector2f m_VisibleOffset = Vector2f(0.0f, 0.0f);

		Vector2f m_OffsetToAnchor = Vector2f(0.0f, 0.0f);

		bool m_LockPosition = false;
		bool m_LockSize = false;
	};
}