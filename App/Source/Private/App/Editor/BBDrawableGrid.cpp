#include "App/Editor/BBDrawableGrid.h"
#include "RSFML/Tools/RMath.h"

using namespace BB;

BBDrawableGrid::BBDrawableGrid(const sf::FloatRect& InGridRect, const sf::Vector2f InGridStep, const sf::Vector2f InLineWidth, const sf::Color& InColor)
	:sf::Drawable(), sf::Transformable(),
	m_currentGridRect(InGridRect), m_currentGridStep(InGridStep), m_lineWidth(InLineWidth), m_vertexColor(InColor)
{
	RecompteVertexArray();
}

void BBDrawableGrid::SetGridRect(const sf::FloatRect& InGridRect, bool bInRedraw)
{
	m_currentGridRect = InGridRect;
	if (bInRedraw)
	{
		RecompteVertexArray();
	}
}
void BBDrawableGrid::SetGridStep(const sf::Vector2f InGridStep, bool bInRedraw)
{
	m_currentGridStep = InGridStep;
	if (bInRedraw)
	{
		RecompteVertexArray();
	}
}
void BB::BBDrawableGrid::SetLineWidth(const sf::Vector2f InLineWidth, bool bInRedraw)
{
	m_lineWidth = InLineWidth;
	if (bInRedraw)
	{
		RecompteVertexArray();
	}
}
void BBDrawableGrid::SetColor(const sf::Color& InColor, bool bInRedraw)
{
	m_vertexColor = InColor;
	if (bInRedraw)
	{
		RecompteVertexColor();
	}
}
void BBDrawableGrid::SetAlpa(const uint8_t InNewAlpha, bool bInRedraw)
{
	sf::Color newColor = m_vertexColor;
	newColor.a = InNewAlpha;
	SetColor(newColor);
}
void BBDrawableGrid::RecompteVertexColor()
{
	size_t c = m_vertices.getVertexCount();
	for (size_t i = 0; i < c; ++i)
	{
		m_vertices[i].color = m_vertexColor;
	}
}

void BBDrawableGrid::RecompteVertexArray()
{
	constexpr size_t VERTEX_PER_QUAD_SHAPE = 6;
	constexpr size_t VERTEX_PER_SQUARE = VERTEX_PER_QUAD_SHAPE * 4;

	auto CreateQuadShape = [](const sf::Color& InInnerVertexColor, sf::VertexArray& InVertArray, const size_t InStartIndex,
		const sf::Vector2f vertTopLeft, const sf::Vector2f vertTopRight, const sf::Vector2f vertBottomLeft, const sf::Vector2f vertBottomRight
		) -> void
		{
			constexpr sf::Vector2f uvTexCoord = sf::Vector2f(0.f, 0.f);
			//Triangle 0
			InVertArray[InStartIndex + 00].position = vertTopLeft;	//RectTopLeft(InWorldRect);
			InVertArray[InStartIndex + 00].texCoords = uvTexCoord;	//RectTopLeft(InUVRect);
			InVertArray[InStartIndex + 00].color = InInnerVertexColor;

			InVertArray[InStartIndex + 01].position = vertTopRight;		//RectTopRight(InWorldRect);
			InVertArray[InStartIndex + 01].texCoords = uvTexCoord;		//RectTopRight(InUVRect);
			InVertArray[InStartIndex + 01].color = InInnerVertexColor;

			InVertArray[InStartIndex + 02].position = vertBottomLeft;		//RectBottomLeft(InWorldRect);
			InVertArray[InStartIndex + 02].texCoords = uvTexCoord;		//RectBottomLeft(InUVRect);
			InVertArray[InStartIndex + 02].color = InInnerVertexColor;


			//Triangle 1
			InVertArray[InStartIndex + 03].position = vertTopRight;		//RectTopRight(InWorldRect);
			InVertArray[InStartIndex + 03].texCoords = uvTexCoord;		//RectTopRight(InUVRect);
			InVertArray[InStartIndex + 03].color = InInnerVertexColor;

			InVertArray[InStartIndex + 04].position = vertBottomRight;		//RectBottomRight(InWorldRect);
			InVertArray[InStartIndex + 04].texCoords = uvTexCoord;		//RectBottomRight(InUVRect);
			InVertArray[InStartIndex + 04].color = InInnerVertexColor;

			InVertArray[InStartIndex + 05].position = vertBottomLeft;		//RectBottomLeft(InWorldRect);
			InVertArray[InStartIndex + 05].texCoords = uvTexCoord;		//RectBottomLeft(InUVRect);
			InVertArray[InStartIndex + 05].color = InInnerVertexColor;
		};

	auto CreateSquare = [&CreateQuadShape](const sf::Color& InInnerVertexColor, sf::VertexArray& InVertArray, const size_t InStartIndex,
		const sf::Vector2f InTopLeft, const sf::Vector2f InSize, const sf::Vector2f InBorderSize) -> void
		{
			//We are only drawing half the border there
			sf::Vector2f borderSize2D = sf::Vector2f(InBorderSize.x * 0.5f, InBorderSize.y * 0.5f);

			sf::FloatRect squareOuterRect = sf::FloatRect(InTopLeft, InSize);
			sf::FloatRect squareInnerRect = sf::FloatRect(InTopLeft + borderSize2D, InSize - (borderSize2D * 2.f));

			sf::Vector2f outerTopLeft = RSfml::RectTopLeft(squareOuterRect);
			sf::Vector2f outerTopRight = RSfml::RectTopRight(squareOuterRect);
			sf::Vector2f outerBottomLeft = RSfml::RectBottomLeft(squareOuterRect);
			sf::Vector2f outerBottomRight = RSfml::RectBottomRight(squareOuterRect);


			sf::Vector2f innerTopLeft = RSfml::RectTopLeft(squareInnerRect);
			sf::Vector2f innerTopRight = RSfml::RectTopRight(squareInnerRect);
			sf::Vector2f innerBottomLeft = RSfml::RectBottomLeft(squareInnerRect);
			sf::Vector2f innerBottomRight = RSfml::RectBottomRight(squareInnerRect);

			CreateQuadShape
				(
					InInnerVertexColor, InVertArray,
					InStartIndex + (VERTEX_PER_QUAD_SHAPE * 0),
					outerTopLeft, outerTopRight, innerTopLeft, innerTopRight
				);

			CreateQuadShape
			(
				InInnerVertexColor, InVertArray,
				InStartIndex + (VERTEX_PER_QUAD_SHAPE * 1),
				innerTopRight, outerTopRight, innerBottomRight, outerBottomRight
			);

			CreateQuadShape
			(
				InInnerVertexColor, InVertArray,
				InStartIndex + (VERTEX_PER_QUAD_SHAPE * 2),
				innerBottomLeft, innerBottomRight, outerBottomLeft, outerBottomRight
			);
			
			CreateQuadShape
			(
				InInnerVertexColor, InVertArray,
				InStartIndex + (VERTEX_PER_QUAD_SHAPE * 3),
				outerTopLeft, innerTopLeft, outerBottomLeft, innerBottomLeft
			);
		};

	sf::Vector2f nbSquaresFlt = RSfml::div(m_currentGridRect.size, m_currentGridStep);
	sf::Vector2i nbSquaresInt = sf::Vector2i
		(
			static_cast<int>(std::floor(nbSquaresFlt.x)),
			static_cast<int>(std::floor(nbSquaresFlt.y))
		);
	m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	m_vertices.resize(VERTEX_PER_SQUARE * nbSquaresInt.x * nbSquaresInt.y);
	for (size_t x = 0; x < nbSquaresInt.x; ++x)
	{
		for (size_t y = 0; y < nbSquaresInt.y; ++y)
		{
			size_t firstVerticeIdx = VERTEX_PER_SQUARE * ((x * nbSquaresInt.y) + y);
			sf::Vector2f topLeftPoint = m_currentGridRect.position + (RSfml::mul(m_currentGridStep, sf::Vector2f((float)x, (float)y)));
			CreateSquare(m_vertexColor, m_vertices, firstVerticeIdx, topLeftPoint, m_currentGridStep, m_lineWidth);
		}
	}

}
BBDrawableGrid::~BBDrawableGrid()
{
	//m_vertices.clear();
}
void BBDrawableGrid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

	// apply the texture
	states.texture = nullptr;

	// you may also override states.shader or states.blendMode if you want

	// draw the vertex array
	target.draw(m_vertices, states);
}