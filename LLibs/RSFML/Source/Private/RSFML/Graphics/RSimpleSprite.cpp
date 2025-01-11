#include "RSFML/Graphics/RSimpleSprite.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"

bool RSfml::CreateSimpleSprite(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Color& InVertexColor)
{
	auto CreateSquare = [](const sf::Color& InInnerVertexColor, sf::VertexArray& InVertArray, const size_t InStartIndex,
		const float InPosXStart, const float InPosXEnd, const float InPosYStart, const float InPosYEnd,
		const float InUV_XStart, const float InUV_XEnd, const float InUV_YStart, const float InUV_YEnd
		) -> void
		{
			//Triangle 0
			InVertArray[InStartIndex + 00].position = sf::Vector2f(InPosXStart, InPosYStart);	//RectTopLeft(InWorldRect);
			InVertArray[InStartIndex + 00].texCoords = sf::Vector2f(InUV_XStart, InUV_YStart);	//RectTopLeft(InUVRect);
			InVertArray[InStartIndex + 00].color = InInnerVertexColor;

			InVertArray[InStartIndex + 01].position = sf::Vector2f(InPosXEnd, InPosYStart);		//RectTopRight(InWorldRect);
			InVertArray[InStartIndex + 01].texCoords = sf::Vector2f(InUV_XEnd, InUV_YStart);		//RectTopRight(InUVRect);
			InVertArray[InStartIndex + 01].color = InInnerVertexColor;

			InVertArray[InStartIndex + 02].position = sf::Vector2f(InPosXStart, InPosYEnd);		//RectBottomLeft(InWorldRect);
			InVertArray[InStartIndex + 02].texCoords = sf::Vector2f(InUV_XStart, InUV_YEnd);		//RectBottomLeft(InUVRect);
			InVertArray[InStartIndex + 02].color = InInnerVertexColor;


			//Triangle 1
			InVertArray[InStartIndex + 03].position = sf::Vector2f(InPosXEnd, InPosYStart);		//RectTopRight(InWorldRect);
			InVertArray[InStartIndex + 03].texCoords = sf::Vector2f(InUV_XEnd, InUV_YStart);		//RectTopRight(InUVRect);
			InVertArray[InStartIndex + 03].color = InInnerVertexColor;

			InVertArray[InStartIndex + 04].position = sf::Vector2f(InPosXEnd, InPosYEnd);		//RectBottomRight(InWorldRect);
			InVertArray[InStartIndex + 04].texCoords = sf::Vector2f(InUV_XEnd, InUV_YEnd);		//RectBottomRight(InUVRect);
			InVertArray[InStartIndex + 04].color = InInnerVertexColor;

			InVertArray[InStartIndex + 05].position = sf::Vector2f(InPosXStart, InPosYEnd);		//RectBottomLeft(InWorldRect);
			InVertArray[InStartIndex + 05].texCoords = sf::Vector2f(InUV_XStart, InUV_YEnd);		//RectBottomLeft(InUVRect);
			InVertArray[InStartIndex + 05].color = InInnerVertexColor;


		};

	constexpr size_t vertexPerSquare = 6;
	const sf::IntRect iTextureRect = InTextureAtlasData.RectFromBounds<int>();
	const sf::FloatRect fTextureRect = (sf::FloatRect)iTextureRect;

	if (InOutVertArray.getVertexCount() < (InStartIndex + vertexPerSquare))
	{
		InOutVertArray.resize(InStartIndex + vertexPerSquare);
	}

	CreateSquare
	(
		InVertexColor, InOutVertArray, InStartIndex,
		InWorldRect.position.x, InWorldRect.position.x + InWorldRect.size.x, InWorldRect.position.y, InWorldRect.position.y + InWorldRect.size.y,
		fTextureRect.position.x, fTextureRect.position.x + fTextureRect.size.x, fTextureRect.position.y, fTextureRect.position.y + fTextureRect.size.y
	);
	return true;
}
