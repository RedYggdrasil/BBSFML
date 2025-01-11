#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"

bool RSfml::LineLineIntersect(const sf::Vector2f InLine1A, const sf::Vector2f InLine1B, const sf::Vector2f InLine2A, const sf::Vector2f InLine2B, sf::Vector2f& OutResult)
{
	OutResult = sf::Vector2f(0.f, 0.f);

	// Direction vectors of the lines
	sf::Vector2f lDir1 = InLine1B - InLine1A;
	sf::Vector2f lDir2 = InLine2B - InLine2A;

	// Cross product to determine if lines are parallel
	float crossProduct = lDir1.x * lDir2.y - lDir1.y * lDir2.x;
	if (std::fabs(crossProduct) < std::numeric_limits<float>::epsilon())
	{
		return false;
	}

	// Using determinants to find intersection point
	float t = ((InLine2A.x - InLine1A.x) * lDir2.y - (InLine2A.y - InLine1A.y) * lDir2.x) / crossProduct;
	OutResult = InLine1A + t * lDir1;
	return true;
}

void RSfml::ExtendAABB(sf::FloatRect& InOutAABB, const sf::Vector2f& InNewPoint)
{
	float minX = std::min(InOutAABB.position.x, InNewPoint.x);
	float minY = std::min(InOutAABB.position.y, InNewPoint.y);
	float maxX = std::max(InOutAABB.position.x + InOutAABB.size.x, InNewPoint.x);
	float maxY = std::max(InOutAABB.position.y + InOutAABB.size.y, InNewPoint.y);
	InOutAABB.position = sf::Vector2f(minX, minY);
	InOutAABB.size = sf::Vector2f(maxX - minX, maxY - minY);
}

void RSfml::ExtendAABB(sf::FloatRect& InOutAABB, const sf::Vector2f* InNewPoints, const size_t InPointsSize)
{
	float minX = InOutAABB.position.x;
	float minY = InOutAABB.position.y;
	float maxX = InOutAABB.position.x + InOutAABB.size.x;
	float maxY = InOutAABB.position.y + InOutAABB.size.y;

	for (size_t i = 0; i < InPointsSize; ++i)
	{
		const sf::Vector2f& point = InNewPoints[i];
		minX = std::min(minX, point.x);
		minY = std::min(minY, point.y);
		maxX = std::max(maxX, point.x);
		maxY = std::max(maxY, point.y);
	}
	InOutAABB.position = sf::Vector2f(minX, minY);
	InOutAABB.size = sf::Vector2f(maxX - minX, maxY - minY);
}

void RSfml::ExtendAABB(sf::FloatRect& InOutAABB, const sf::FloatRect& InOtherAABB)
{
	float minX = std::min(InOutAABB.position.x,						InOtherAABB.position.x);
	float minY = std::min(InOutAABB.position.y,						InOtherAABB.position.y);
	float maxX = std::max(InOutAABB.position.x + InOutAABB.size.x,	InOtherAABB.position.x + InOutAABB.size.x);
	float maxY = std::max(InOutAABB.position.y + InOutAABB.size.y,	InOtherAABB.position.y + InOutAABB.size.y);
	InOutAABB.position = sf::Vector2f(minX, minY);
	InOutAABB.size = sf::Vector2f(maxX - minX, maxY - minY);
}

bool RSfml::RMATH_UNIT_TEST()
{
	bool bAllSucessfull = true;

	sf::Vector2f res;

	if (!RSfml::LineLineIntersect({ 0.f, 0.f }, { 5.f, 5.f }, { 2.f, 5.f }, { 2.f, -5.f }, res))
	{
		RLog::Log(LOG_DISPLAY, "This should have succeeded !");
		bAllSucessfull = false;
	}
	if (RSfml::LineLineIntersect({ 0.f, 0.f }, { 1.f, 1.f }, { 1.f, 1.f }, { 2.f, 2.f }, res))
	{
		RLog::Log(LOG_DISPLAY, "This should never succeed");
		bAllSucessfull = false;
	}
	if (!RSfml::LineLineIntersect({ -1.f, -1.f }, { 1.f, 1.f }, { -1.f, 1.f }, { 1.f, -1.f }, res))
	{
		RLog::Log(LOG_DISPLAY, "This should have succeeded !");
		bAllSucessfull = false;
	}

	return bAllSucessfull;
}
