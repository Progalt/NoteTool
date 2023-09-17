#pragma once
#include "Widget.h"
#include "../Font.h"
#include "Utility.h"

namespace gui
{
	class Text : public Widget
	{
	public:

		Text()
		{
			SetLockSize(true);
		}

		virtual ~Text()
		{
			m_Texture.Destroy();
		}

		void GenerateVertexList(DrawList& drawList) override;

		void SetString(const std::string& str);

		void SetFont(Font* font);

		const std::string GetString()
		{
			return m_String;
		}

	private:

		std::string m_String;
		Font* m_Font;

		GPUTexture m_Texture;
		Image m_Image;
		bool m_RerenderText = true;

		void RasterizeText();
	};
}