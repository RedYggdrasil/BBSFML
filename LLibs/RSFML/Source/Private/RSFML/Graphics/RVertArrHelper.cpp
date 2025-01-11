#include "RSFML/Graphics/RVertArrHelper.h"

bool RSfml::RotateVertices(sf::VertexArray& InOutVertArray, const size_t InVertStartIndex, const size_t InVertCount, const sf::Angle InRotation, const sf::Vector2f InOrigin)
{
	sf::Transform tr;
	tr.translate(InOrigin);
	tr.rotate(InRotation);
	tr.translate(-InOrigin);

	return TransformVertices(InOutVertArray, InVertStartIndex, InVertCount, tr);
}

bool RSfml::TransformVertices(sf::VertexArray& InOutVertArray, const size_t InVertStartIndex, const size_t InVertCount, const sf::Transform& InTransform)
{
	const size_t VertEnd = InVertStartIndex + InVertCount;

	if (VertEnd > InOutVertArray.getVertexCount())
	{ 
		return false; 
	}
	for (size_t i = InVertStartIndex; i < VertEnd; ++i)
	{
		sf::Vertex& vert = InOutVertArray[i];
		vert.position = InTransform.transformPoint(vert.position);
	}

	return true;
}
