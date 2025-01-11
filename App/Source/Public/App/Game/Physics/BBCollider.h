#pragma once
#include "RSFML/Tools/REnum.h"
#include "App/Defines/pch.h"
#include <vector>

namespace BB
{

	enum class BBColliderType : uint8_t
	{
		RENUM_MIN_VAL(SphereCollider, 0),
		RENUM_MAX_VAL(RectCollider, 1)
	};
	RS_DEFINE_ENUM(BBColliderType);

	struct OrientedBox
	{
	private:
		sf::Vector2f m_center;
		sf::Vector2f m_extends;
		sf::Angle m_rotation;
		/// <summary>
		/// Clockwork order : TopLeft - TopRight - BottomRight - BottomLeft
		/// </summary>
		std::array<sf::Vector2f, 4> m_transformedPoints;
	public:
		OrientedBox();
		OrientedBox(const sf::Vector2f InCenter, const sf::Vector2f InExtends, sf::Angle InAngle);
		OrientedBox(sf::FloatRect InRect, sf::Angle InAngle);
		void Transform(const sf::Transform& InTransform) { (*this) = TransformCopy(InTransform); };
		OrientedBox TransformCopy(const sf::Transform& InTransform) const;
		/// <summary>
		/// Return Axis and vertices for SAT
		/// </summary>
		/// <param name="InOutAxisArray">Axis Normal (perpendicular)</param>
		/// <param name="InOutVertices">Vertices</param>
		void FillSATAxesAndVertices(std::vector<sf::Vector2f>& InOutAxisArray, std::vector<sf::Vector2f>& InOutVertices) const;
		const std::array<sf::Vector2f, 4>& GetTransformedPoints() const { return m_transformedPoints; }
		static OrientedBox FromSizeAndAngleAtOrigin(const sf::Vector2f& InSize, const sf::Angle InRotation);
		static OrientedBox FromCenterSizeAndAngle(const sf::Vector2f& InCenter, const sf::Vector2f& InSize, const sf::Angle InRotation);

		//const sf::FloatRect& GetInnerRect() const { return m_floatRect; }
		sf::Angle GetInnerRotation() const { return m_rotation; }
	private:
		void ComputeTransformed();
	};

	struct OrientedFloatRect
	{
	private:
		sf::FloatRect m_floatRect;
		sf::Angle m_rotation;
		/// <summary>
		/// Clockwork order : TopLeft - TopRight - BottomRight - BottomLeft
		/// </summary>
		std::array<sf::Vector2f, 4> m_transformedPoints;
	public:
		OrientedFloatRect();
		OrientedFloatRect(sf::FloatRect InRect, sf::Angle InAngle);
		void Transform(const sf::Transform& InTransform) { (*this) = TransformCopy(InTransform); };
		OrientedFloatRect TransformCopy(const sf::Transform& InTransform) const;
		/// <summary>
		/// Return Axis and vertices for SAT
		/// </summary>
		/// <param name="InOutAxisArray">Axis Normal (perpendicular)</param>
		/// <param name="InOutVertices">Vertices</param>
		void FillSATAxesAndVertices(std::vector<sf::Vector2f>& InOutAxisArray, std::vector<sf::Vector2f>& InOutVertices) const;
		const std::array<sf::Vector2f, 4>& GetTransformedPoints() const { return m_transformedPoints; }
		static OrientedFloatRect FromSizeAndAngleAtOrigin(const sf::Vector2f& InSize, const sf::Angle InRotation);
		static OrientedFloatRect FromCenterSizeAndAngle(const sf::Vector2f& InCenter, const sf::Vector2f& InSize, const sf::Angle InRotation);

		const sf::FloatRect& GetInnerRect() const { return m_floatRect; }
		sf::Angle GetInnerRotation() const { return m_rotation; }
	private :
		void ComputeTransformed();
	};
	struct FloatCircle
	{
		sf::Vector2f Position;
		float Radius;
	public:
		void Transform(const sf::Transform& InTransform) { (*this) = TransformCopy(InTransform); };
		FloatCircle TransformCopy(const sf::Transform& InTransform) const;
	};

	struct BBCollider
	{
	public:
		BBColliderType Type;
		union
		{
			BB::OrientedBox BoxData;
			BB::FloatCircle SphereData;
		};
	public:
		BBCollider();
		BBCollider(BBColliderType InType);
		BBCollider(const OrientedBox& InBoxData);
		BBCollider(const FloatCircle& InSphereData);
	public:
		inline void Transform(const sf::Transform &InTransform) { (*this) = TransformCopy(InTransform); }
		BBCollider TransformCopy(const sf::Transform& InTransform) const;
		bool OverlapWith(const BBCollider& InOther) const;
		/// <summary>
		/// Return Axis and vertices for SAT
		/// </summary>
		/// <param name="InOutAxisArray">Axis Normal (perpendicular)</param>
		/// <param name="InOutVertices">Vertices</param>
		void FillSATAxesAndVertices(std::vector<sf::Vector2f>& InOutAxisArray, std::vector<sf::Vector2f>& InOutVertices) const;
	public:
		sf::FloatRect ComputeBoundBox() const;
		void ExtendBoundBox(sf::FloatRect* InOutBoundingBox) const;

#if DEBUG_PHYSIC
	public:
		bool DebugDraw(mds::IRProgramMemElem* InContext, ID3D12GraphicsCommandList7* InUploadCommandList) const;
		bool DebugDraw(RSRPhysicManager* InPhysicManager, ID3D12GraphicsCommandList7* InUploadCommandList) const;
		static bool DebugDrawOrientedBox(RSRPhysicManager* InPhysicManager, const DirectX::BoundingOrientedBox& InOrientedBox, ID3D12GraphicsCommandList7* InUploadCommandList);
		static bool DebugDrawSphere(RSRPhysicManager* InPhysicManager, const DirectX::BoundingSphere& InSphere, ID3D12GraphicsCommandList7* InUploadCommandList);
		static bool DebugDrawBox(RSRPhysicManager* InPhysicManager, const DirectX::BoundingBox& InBox, ID3D12GraphicsCommandList7* InUploadCommandList);
#endif
	};
	sf::FloatRect AABBFromOrientedBox(const OrientedBox& InOrientedBox);
	void ExtendAABBWithOrientedBox(sf::FloatRect& InOutAABB, const OrientedBox& InOrientedBox);
	sf::FloatRect AABBFromCircle(const FloatCircle& InFloatCircle);
	void ExtendAABBWithCircle(sf::FloatRect& InOutAABB, const FloatCircle& InFloatCircle);
	//struct alignas(16) RSRTrsCollider
	//{
	//public:
	//	alignas(16) DirectX::XMMATRIX Transform;
	//	RSRCollider Collider;
	//};
};