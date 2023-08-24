#pragma once

#include <vector>
#include <string>
#include "../Renderer/Renderer.h"
#include "../Theme.h"
#include "../Maths/Rect.h"

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
		};

		std::vector<Call> drawcalls;

		void Add(std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices, GPUTexture* tex = nullptr)
		{
			if (m_CurrentCall == nullptr)
			{
				drawcalls.push_back(Call());

				m_CurrentCall = &drawcalls[drawcalls.size() - 1ui64];
				m_CurrentCall->texture = tex;

				m_CurrentCall->firstIndex = 0;
			}

			if (m_CurrentCall->texture != tex)
			{
				drawcalls.push_back(Call());

				m_CurrentCall = &drawcalls[drawcalls.size() - 1ui64];
				m_CurrentCall->texture = tex;

				m_CurrentCall->firstIndex = indices.size();
			}

			size_t offset = vertices.size();

			std::vector<uint32_t> offsetIndices;

			for (auto& idx : newIndices)
				offsetIndices.push_back(idx + offset);

			vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
			indices.insert(indices.end(), offsetIndices.begin(), offsetIndices.end());

			m_CurrentCall->indexCount += indices.size();
		}

	private:

		Call* m_CurrentCall;
		
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
			OnEvent();

			for (auto& child : m_Children)
				child->HandleEvents();
		}

		template<typename _Ty, class ... Args>
		_Ty* NewChild(Args... args)
		{
			Widget* w = new _Ty(args...);

			m_Children.push_back(w);
			w->m_Parent = this;
			w->m_Theme = m_Theme;

			return static_cast<_Ty*>(w);
		}

		void SetName(const std::string& name) { m_Name = name; }

		void SetTheme(Theme* theme) { m_Theme = theme; }

		void SetBounds(FloatRect bounds) 
		{ 
			m_Bounds = bounds; 
			m_GlobalBounds = m_Bounds;
			if (m_Parent)
				m_GlobalBounds.position += m_Parent->GetBounds().position;
		}

		FloatRect GetBounds() { return m_GlobalBounds; }

	protected:

		bool m_Hovered = false;

		FloatRect m_Bounds;
		FloatRect m_GlobalBounds;


		void GenerateChildVertexLists(DrawList& drawList)
		{
			for (auto& child : m_Children)
				child->GenerateVertexList(drawList);
		}

		std::vector<Widget*> m_Children;
		Widget* m_Parent;

		std::string m_Name;

		Theme* m_Theme;
	};
}