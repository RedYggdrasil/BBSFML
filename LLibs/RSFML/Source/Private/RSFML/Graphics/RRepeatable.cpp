#include "RSFML/Graphics/RRepeatable.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"



bool RSfml::CreateRepeatable(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Color& InVertexColor)
{
	constexpr size_t vertexPerSquare = 6;
	const sf::IntRect iTextureRect = InTextureAtlasData.RectFromBounds<int>();
	const sf::FloatRect fTextureRect = (sf::FloatRect)iTextureRect;


	auto CreateSquare = [](const sf::Color& InInnerVertexColor, sf::VertexArray& InVertArray, const size_t InStartIndex, 
		const float InPosXStart, const float InPosXEnd, const float InPosYStart, const float InPosYEnd,
		const float InUV_XStart, const float InUV_XEnd, const float InUV_YStart, const float InUV_YEnd
		) -> void
		{
			//Triangle 0
			InVertArray[InStartIndex + 00].position		= sf::Vector2f(InPosXStart, InPosYStart);	//RectTopLeft(InWorldRect);
			InVertArray[InStartIndex + 00].texCoords	= sf::Vector2f(InUV_XStart, InUV_YStart);	//RectTopLeft(InUVRect);
			InVertArray[InStartIndex + 00].color		= InInnerVertexColor;

			InVertArray[InStartIndex + 01].position		= sf::Vector2f(InPosXEnd, InPosYStart);		//RectTopRight(InWorldRect);
			InVertArray[InStartIndex + 01].texCoords	= sf::Vector2f(InUV_XEnd, InUV_YStart);		//RectTopRight(InUVRect);
			InVertArray[InStartIndex + 01].color		= InInnerVertexColor;

			InVertArray[InStartIndex + 02].position		= sf::Vector2f(InPosXStart, InPosYEnd);		//RectBottomLeft(InWorldRect);
			InVertArray[InStartIndex + 02].texCoords	= sf::Vector2f(InUV_XStart, InUV_YEnd);		//RectBottomLeft(InUVRect);
			InVertArray[InStartIndex + 02].color		= InInnerVertexColor;


			//Triangle 1
			InVertArray[InStartIndex + 03].position		= sf::Vector2f(InPosXEnd, InPosYStart);		//RectTopRight(InWorldRect);
			InVertArray[InStartIndex + 03].texCoords	= sf::Vector2f(InUV_XEnd, InUV_YStart);		//RectTopRight(InUVRect);
			InVertArray[InStartIndex + 03].color = InInnerVertexColor;

			InVertArray[InStartIndex + 04].position		= sf::Vector2f(InPosXEnd, InPosYEnd);		//RectBottomRight(InWorldRect);
			InVertArray[InStartIndex + 04].texCoords	= sf::Vector2f(InUV_XEnd, InUV_YEnd);		//RectBottomRight(InUVRect);
			InVertArray[InStartIndex + 04].color = InInnerVertexColor;

			InVertArray[InStartIndex + 05].position		= sf::Vector2f(InPosXStart, InPosYEnd);		//RectBottomLeft(InWorldRect);
			InVertArray[InStartIndex + 05].texCoords	= sf::Vector2f(InUV_XStart, InUV_YEnd);		//RectBottomLeft(InUVRect);
			InVertArray[InStartIndex + 05].color = InInnerVertexColor;


		};

	//sf::FloatRect fRectTexNrlzd = sf::FloatRect
	//(
	//	sf::Vector2f(fTextureRect.position.x / InAtlasTextureSize.x, fTextureRect.position.y / InAtlasTextureSize.y),
	//	sf::Vector2f(fTextureRect.size.x / InAtlasTextureSize.x, fTextureRect.size.y / InAtlasTextureSize.y)
	//);

	const sf::Vector2f fSquareNumber = RSfml::div(InWorldRect.size, fTextureRect.size);
	const sf::Vector2i iSquareNumber = sf::Vector2i(static_cast<int>(std::ceil(fSquareNumber.x)), static_cast<int>(std::ceil(fSquareNumber.y)));

	const size_t totalVertexNeeded = vertexPerSquare * (iSquareNumber.x * iSquareNumber.y);
	if (InOutVertArray.getVertexCount() < (InStartIndex + totalVertexNeeded))
	{
		InOutVertArray.resize(InStartIndex + totalVertexNeeded);
	}
	for (size_t squareX = 0; squareX < iSquareNumber.x; ++squareX)
	{
		const bool bIsLastSquareX = (squareX + 1) == iSquareNumber.x;
		float squareXEndPercent;
		if (bIsLastSquareX)
		{
			squareXEndPercent = std::fmod(fSquareNumber.x, 1.f);
			if (squareXEndPercent < std::numeric_limits<float>::epsilon())
			{
				squareXEndPercent = 1.f;
			}
		}
		else
		{
			squareXEndPercent = 1.f;
		}

		const float squareX_UVStart = fTextureRect.position.x;
		const float squareX_UVEnd = fTextureRect.position.x + (fTextureRect.size.x * squareXEndPercent);

		const float squareX_PosStart = InWorldRect.position.x + (squareX * fTextureRect.size.x);
		const float squareX_PosEnd = squareX_PosStart + (fTextureRect.size.x * squareXEndPercent);

		for (size_t squareY = 0; squareY < iSquareNumber.y; ++squareY)
		{
			const size_t squareIndex = (squareX * iSquareNumber.y) + squareY;

			const bool bIsLastSquareY = (squareY + 1) == iSquareNumber.y;
			float squareYEndPercent;

			if (bIsLastSquareY)
			{
				squareYEndPercent = std::fmod(fSquareNumber.y, 1.f);
				if (squareYEndPercent < std::numeric_limits<float>::epsilon())
				{
					squareYEndPercent = 1.f;
				}
			}
			else
			{
				squareYEndPercent = 1.f;
			}

			const float squareY_UVStart = fTextureRect.position.y;
			const float squareY_UVEnd = fTextureRect.position.y + (fTextureRect.size.y * squareYEndPercent);
			

			const float squareY_PosStart = InWorldRect.position.y + (squareY * fTextureRect.size.y);
			const float squareY_PosEnd = squareY_PosStart + (fTextureRect.size.y * squareYEndPercent);

			CreateSquare
			(
				InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (squareIndex * vertexPerSquare),
				squareX_PosStart, squareX_PosEnd, squareY_PosStart, squareY_PosEnd,
				squareX_UVStart, squareX_UVEnd, squareY_UVStart, squareY_UVEnd
			);
		}
	}
	return true;
}
