#include "App/Game/Physics/BBCollider.h"
#include "RSFML/Tools/RLog.h"
#include "RSFML/Tools/RMath.h"
//HPPs are fully inlined inplace compiled
#include "App/Game/Physics/BBOverlapFuncs.hpp"

using namespace BB;

static const OrientedFloatRect DEFAULT_BOUNDING_ORIENTED_BOX = OrientedFloatRect();

static const OrientedBox DEFAULT_ORIENTED_BOX = OrientedBox();

static const FloatCircle DEFAULT_BOUNDING_SPHERE =
{
	{ 0.f, 0.f }, 0.f
};


BBCollider::BBCollider()
:Type(BBColliderType::RectCollider), BoxData(DEFAULT_ORIENTED_BOX)
{
}

BBCollider::BBCollider(BBColliderType InType)
:Type(InType)
{
	if (Type == BBColliderType::SphereCollider)
	{
		SphereData = DEFAULT_BOUNDING_SPHERE;
	}
	
	else if (Type == BBColliderType::RectCollider)
	{
		BoxData = DEFAULT_ORIENTED_BOX;
	}
	else
	{
		BoxData = DEFAULT_ORIENTED_BOX;
		RLog::Log(LOG_ERROR, "Unimplement case in 'BBCollider::BBCollider'");
	}
}

BB::BBCollider::BBCollider(const OrientedBox& InBoxData)
	:Type(BBColliderType::RectCollider), BoxData(InBoxData)
{
}

//BBCollider::BBCollider(const OrientedFloatRect& InRectData)
//:Type(BBColliderType::RectCollider), RectData(InRectData)
//{
//}

BBCollider::BBCollider(const FloatCircle& InSphereData)
:Type(BBColliderType::SphereCollider), SphereData(InSphereData)
{
}

BBCollider BBCollider::TransformCopy(const sf::Transform& InTransform) const
{
	BBCollider result = *this;
	switch (this->Type)
	{
	case BBColliderType::SphereCollider:
		result.SphereData.Transform(InTransform);
		break;
	case BBColliderType::RectCollider:
		result.BoxData.Transform(InTransform);
		break;
	default:
		RLog::Log(LOG_ERROR, "Unimplement case in 'RCollider::TransformCopy'");
	}
	return result;
}

bool BB::BBCollider::OverlapWith(const BBCollider& InOther) const
{
	return BBOvrlps::ResolveOverlap(this, &InOther);
}

void BB::BBCollider::FillSATAxesAndVertices(std::vector<sf::Vector2f>& InOutAxisArray, std::vector<sf::Vector2f>& InOutVertices) const
{
	switch (this->Type)
	{
	case BBColliderType::SphereCollider:
		//This should never been called, sphere sat are handled by a diffent function
		RLog::Log(LOG_ERROR, "Unimplement Sphere SAT Axis ! in 'BBCollider::FillSATAxesAndVertices'");
		break;
	case BBColliderType::RectCollider:
		BoxData.FillSATAxesAndVertices(InOutAxisArray, InOutVertices);
		break;
	default:
		RLog::Log(LOG_ERROR, "Unimplement case in 'RCollider::FillSATAxesAndVertices'");
		break;
	}
}

sf::FloatRect BB::BBCollider::ComputeBoundBox() const
{
	switch (this->Type)
	{
	case BBColliderType::SphereCollider:
		return AABBFromCircle(this->SphereData);
	case BBColliderType::RectCollider:
		return AABBFromOrientedBox(this->BoxData);
	default:
		RLog::Log(LOG_ERROR , "Unimplement case in 'BBCollider::ComputeBoundBox'");
	}
	return sf::FloatRect({ 0.f, 0.f }, { 0.f, 0.f });
}

void BB::BBCollider::ExtendBoundBox(sf::FloatRect* InOutBoundingBox) const
{
	switch (this->Type)
	{
	case BBColliderType::SphereCollider:
		ExtendAABBWithCircle(/*InOut*/*InOutBoundingBox, this->SphereData);
		break;
	case BBColliderType::RectCollider:
		ExtendAABBWithOrientedBox(/*InOut*/*InOutBoundingBox, this->BoxData);
		break;
	default:
		RLog::Log(LOG_ERROR, "Unimplement case in 'BBCollider::ExtendBoundBox'");
		break;
	}
}

OrientedFloatRect::OrientedFloatRect()
: OrientedFloatRect(sf::FloatRect{ { 0.f, 0.f }, { 0.f, 0.f } }, sf::degrees(0.f))
{
}

BB::OrientedFloatRect::OrientedFloatRect(sf::FloatRect InRect, sf::Angle InAngle)
	:m_floatRect(InRect), m_rotation(InAngle), m_transformedPoints{ sf::Vector2f{0.f, 0.f }, sf::Vector2f{0.f, 0.f }, sf::Vector2f{0.f, 0.f }, sf::Vector2f{0.f, 0.f } }
{
	ComputeTransformed();
}

OrientedFloatRect OrientedFloatRect::TransformCopy(const sf::Transform& InTransform) const
{
	OrientedFloatRect result;
	sf::Vector2f newTopLeft			= InTransform.transformPoint(m_transformedPoints[0]);
	sf::Vector2f newTopRight		= InTransform.transformPoint(m_transformedPoints[1]);
	sf::Vector2f newBottomRight		= InTransform.transformPoint(m_transformedPoints[2]);
	sf::Vector2f newBottomLeft		= InTransform.transformPoint(m_transformedPoints[3]);

	sf::Vector2f newCenter;
	if (!RSfml::LineLineIntersect(newTopLeft, newBottomRight, newBottomLeft, newTopRight, /*Out*/newCenter))
	{
		RLog::Log(LOG_ERROR, "RectFloat has no center after transformation, this should not happen !");
		return result;
	}
	sf::Vector2f newTopLeftToTopRight = (newTopRight - newTopLeft);
	sf::Vector2f newBottomLeftToTopLeft = (newBottomLeft - newTopLeft);
	result.m_floatRect.size.x = newTopLeftToTopRight.length();
	result.m_floatRect.size.y = newBottomLeftToTopLeft.length();
	result.m_floatRect.position = newCenter - (result.m_floatRect.size * 0.5f);
	
	//Angle agaist +x (1, 0)
	result.m_rotation = newTopLeftToTopRight.angle();
	result.ComputeTransformed();
	return result;
}

void OrientedFloatRect::FillSATAxesAndVertices(std::vector<sf::Vector2f>& InOutAxisArray, std::vector<sf::Vector2f>& InOutVertices) const
{
	sf::Vector2f axisNrml = RSfml::AngleToVector(m_rotation);//.normalized(); //result already unit vector
	InOutAxisArray.reserve(InOutAxisArray.size() + 2);
	InOutAxisArray.push_back(axisNrml);
	InOutAxisArray.push_back(sf::Vector2f(-axisNrml.y, axisNrml.x));


	InOutVertices.reserve(InOutVertices.size() + 4);
	InOutVertices.push_back(m_transformedPoints[0]); //TopLeft
	InOutVertices.push_back(m_transformedPoints[1]); //TopRight
	InOutVertices.push_back(m_transformedPoints[2]); //BottomRight
	InOutVertices.push_back(m_transformedPoints[3]); //BottomLeft
}

OrientedFloatRect OrientedFloatRect::FromSizeAndAngleAtOrigin(const sf::Vector2f& InSize, const sf::Angle InRotation)
{

	return OrientedFloatRect(sf::FloatRect(-InSize * 0.5f, InSize), InRotation);
}

OrientedFloatRect OrientedFloatRect::FromCenterSizeAndAngle(const sf::Vector2f& InCenter, const sf::Vector2f& InSize, const sf::Angle InRotation)
{
	return OrientedFloatRect(sf::FloatRect(InCenter - (InSize * 0.5f), InSize), InRotation);
}

void BB::OrientedFloatRect::ComputeTransformed()
{
	sf::Transform trs;
	sf::Vector2f center = m_floatRect.getCenter();
	trs.translate(center);
	trs.rotate(m_rotation);
	trs.translate(-center);
	m_transformedPoints[0] = trs.transformPoint(RSfml::RectTopLeft(m_floatRect));
	m_transformedPoints[1] = trs.transformPoint(RSfml::RectTopRight(m_floatRect));
	m_transformedPoints[2] = trs.transformPoint(RSfml::RectBottomRight(m_floatRect));
	m_transformedPoints[3] = trs.transformPoint(RSfml::RectBottomLeft(m_floatRect));

}

FloatCircle FloatCircle::TransformCopy(const sf::Transform& InTransform) const
{
	FloatCircle result;
	result.Position = InTransform.transformPoint(this->Position);

	// Extract the scale factor
	sf::Vector2f scaleFactor = InTransform.transformPoint({1, 1}) - InTransform.transformPoint({0, 0});

	// This scale an average of 2 x and y, we could have taken max of x y instead
	result.Radius = this->Radius * std::sqrt(scaleFactor.x * scaleFactor.x + scaleFactor.y * scaleFactor.y) / std::sqrt(2.f);

	return result;
}

sf::FloatRect BB::AABBFromOrientedBox(const OrientedBox& InOrientedBox)
{
	const std::array<sf::Vector2f, 4>& transformedPoints = InOrientedBox.GetTransformedPoints();
	sf::FloatRect aabb = sf::FloatRect(transformedPoints[0], sf::Vector2f{ 0.f, 0.f });
	RSfml::ExtendAABB(/*InOut*/aabb, &transformedPoints[1], 3);
	return aabb;
}

void BB::ExtendAABBWithOrientedBox(sf::FloatRect& InOutAABB, const OrientedBox& InOrientedBox)
{
	const std::array<sf::Vector2f, 4>& transformedPoints = InOrientedBox.GetTransformedPoints();
	RSfml::ExtendAABB(/*InOut*/InOutAABB, &transformedPoints[0], 4);
}

sf::FloatRect BB::AABBFromCircle(const FloatCircle& InFloatCircle)
{
	return sf::FloatRect
		(
			InFloatCircle.Position - sf::Vector2f(InFloatCircle.Radius, InFloatCircle.Radius), 
			sf::Vector2f(InFloatCircle.Radius, InFloatCircle.Radius) * 2.f
		);
}

void BB::ExtendAABBWithCircle(sf::FloatRect& InOutAABB, const FloatCircle& InFloatCircle)
{
	const std::array<sf::Vector2f, 2>& extendPoints = 
	{
		{
			InFloatCircle.Position - sf::Vector2f(InFloatCircle.Radius, InFloatCircle.Radius),
			InFloatCircle.Position + sf::Vector2f(InFloatCircle.Radius, InFloatCircle.Radius)
		}
	};
	RSfml::ExtendAABB(/*InOut*/InOutAABB, &extendPoints[0], 2);
}

BB::OrientedBox::OrientedBox()
	: m_center(0.f, 0.f), m_extends(0.f, 0.f), m_rotation(sf::degrees(0.f))
{
}

BB::OrientedBox::OrientedBox(const sf::Vector2f InCenter, const sf::Vector2f InExtends, sf::Angle InAngle)
	:m_center(InCenter), m_extends(InExtends), m_rotation(InAngle), m_transformedPoints{sf::Vector2f{0.f, 0.f}, sf::Vector2f{0.f, 0.f}, sf::Vector2f{0.f, 0.f}, sf::Vector2f{0.f, 0.f}}
{
	ComputeTransformed();
}

BB::OrientedBox::OrientedBox(sf::FloatRect InRect, sf::Angle InAngle)
	:OrientedBox(InRect.getCenter(), InRect.size * 0.5f, InAngle)
{
}

OrientedBox OrientedBox::TransformCopy(const sf::Transform& InTransform) const
{
	OrientedBox result;
	sf::Vector2f newTopLeft = InTransform.transformPoint(m_transformedPoints[0]);
	sf::Vector2f newTopRight = InTransform.transformPoint(m_transformedPoints[1]);
	sf::Vector2f newBottomRight = InTransform.transformPoint(m_transformedPoints[2]);
	sf::Vector2f newBottomLeft = InTransform.transformPoint(m_transformedPoints[3]);

	
	if (!RSfml::LineLineIntersect(newTopLeft, newBottomRight, newBottomLeft, newTopRight, /*Out*/result.m_center))
	{
		RLog::Log(LOG_ERROR, "RectFloat has no center after transformation, this should not happen !");
		return result;
	}
	sf::Vector2f newTopLeftToTopRight = (newTopRight - newTopLeft);
	sf::Vector2f newBottomLeftToTopLeft = (newBottomLeft - newTopLeft);
	result.m_extends.x = newTopLeftToTopRight.length() * 0.5f;
	result.m_extends.y = newBottomLeftToTopLeft.length() * 0.5f;

	//Angle agaist +x (1, 0)
	result.m_rotation = newTopLeftToTopRight.angle();
	result.ComputeTransformed();
	return result;
}

void OrientedBox::FillSATAxesAndVertices(std::vector<sf::Vector2f>& InOutAxisArray, std::vector<sf::Vector2f>& InOutVertices) const
{
	sf::Vector2f axisNrml = RSfml::AngleToVector(m_rotation);//.normalized(); //result already unit vector
	InOutAxisArray.reserve(InOutAxisArray.size() + 2);
	InOutAxisArray.push_back(axisNrml);
	InOutAxisArray.push_back(sf::Vector2f(-axisNrml.y, axisNrml.x));


	InOutVertices.reserve(InOutVertices.size() + 4);
	InOutVertices.push_back(m_transformedPoints[0]); //TopLeft
	InOutVertices.push_back(m_transformedPoints[1]); //TopRight
	InOutVertices.push_back(m_transformedPoints[2]); //BottomRight
	InOutVertices.push_back(m_transformedPoints[3]); //BottomLeft
}

OrientedBox OrientedBox::FromSizeAndAngleAtOrigin(const sf::Vector2f& InSize, const sf::Angle InRotation)
{

	return OrientedBox(sf::Vector2f(0.f, 0.f), InSize * 0.5f, InRotation);
}

OrientedBox OrientedBox::FromCenterSizeAndAngle(const sf::Vector2f& InCenter, const sf::Vector2f& InSize, const sf::Angle InRotation)
{
	return OrientedBox(InCenter, InSize * 0.5f, InRotation);
}

void BB::OrientedBox::ComputeTransformed()
{
	sf::Transform trs;
	//sf::Vector2f center = m_floatRect.getCenter();
	trs.translate(m_center);
	trs.rotate(m_rotation);
	trs.translate(-m_center);
	m_transformedPoints[0] = trs.transformPoint(RSfml::BoxTopLeft(m_center, m_extends));
	m_transformedPoints[1] = trs.transformPoint(RSfml::BoxTopRight(m_center, m_extends));
	m_transformedPoints[2] = trs.transformPoint(RSfml::BoxBottomRight(m_center, m_extends));
	m_transformedPoints[3] = trs.transformPoint(RSfml::BoxBottomLeft(m_center, m_extends));

}