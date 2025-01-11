#include "RSFML/Tools/Textures/RGDXTPSprite.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Graphics/R9Slice.h"
#include "RSFML/Graphics/RRepeatable.h"
#include "RSFML/Graphics/RSimpleSprite.h"
#include <stdexcept>

using namespace RSfml;

RGDXTPSprite::RGDXTPSprite()
	:sf::Drawable(), sf::Transformable(),
	m_initialized(false), m_bVerticesDirty(false), m_bAutoUpdateVerticeArray(true),
	m_spriteType(RGDXTPSpriteType::Strench), m_texture(nullptr),
	m_textureData(), m_vertices(), m_color(sf::Color::White),
	m_originPercent(sf::Vector2f(0.5f, 0.5f)), m_worldSize(sf::Vector2f())
{
}

RGDXTPSprite::RGDXTPSprite(const RGDXTPSpriteType InSpriteType, const sf::Texture& InTexture, const RGDXTPAtlasTextureData& InTextureData, const sf::Color& InVertexColor)
	:RGDXTPSprite(InSpriteType, InTexture, InTextureData, InTextureData.RectFromBounds<float>().size, sf::Vector2f(0.5f, 0.5f), InVertexColor)
{ }

RSfml::RGDXTPSprite::RGDXTPSprite(const RGDXTPSpriteType InSpriteType, const sf::Texture & InTexture, const RGDXTPAtlasTextureData & InTextureData, const sf::FloatRect & InWorldRect, const sf::Vector2f InVertexOrigin, const sf::Color& InVertexColor)
	:sf::Drawable(), sf::Transformable(),
	m_bVerticesDirty(true), m_bAutoUpdateVerticeArray(true),
	m_spriteType(InSpriteType), m_texture(&InTexture),
	m_textureData(InTextureData), m_vertices(), m_color(InVertexColor),
	m_originPercent(InVertexOrigin), m_worldSize(InWorldRect.size)
{
	m_bAutoUpdateVerticeArray = false;
	sf::Vector2f originOffset = RSfml::mul(m_worldSize, m_originPercent);
	SetPosition(InWorldRect.position + originOffset);
	if ((bool)m_texture && !m_textureData.ImageName.empty())
	{
		RecomputeVertexArray(true);
	}
	else
	{
		m_initialized = false;
	}
	m_bAutoUpdateVerticeArray = true;
}

RSfml::RGDXTPSprite::RGDXTPSprite(const RGDXTPSpriteType InSpriteType, const sf::Texture& InTexture, const RGDXTPAtlasTextureData& InTextureData, const sf::Vector2f InWorldSize, const sf::Vector2f InVertexOrigin, const sf::Color& InVertexColor)
	:sf::Drawable(), sf::Transformable(),
	m_bVerticesDirty(true), m_bAutoUpdateVerticeArray(true),
	m_spriteType(InSpriteType), m_texture(&InTexture),
	m_textureData(InTextureData), m_vertices(), m_color(InVertexColor),
	m_originPercent(InVertexOrigin), m_worldSize(InWorldSize)
{
	m_bAutoUpdateVerticeArray = false;
	if ((bool)m_texture && !m_textureData.ImageName.empty())
	{
		RecomputeVertexArray(true);
	}
	else
	{
		m_initialized = false;
	}
	m_bAutoUpdateVerticeArray = true;
}

void RSfml::RGDXTPSprite::SetTRSOrigin(const sf::Vector2f InTRSOrigin)
{
	((sf::Transformable*)this)->setOrigin(InTRSOrigin);
}

void RSfml::RGDXTPSprite::SetVertOriginPercent(const sf::Vector2f InVertOrigin)
{
	if (m_originPercent != InVertOrigin)
	{
		m_originPercent = InVertOrigin;
		m_bVerticesDirty = true;
		if (m_bAutoUpdateVerticeArray)
		{
			RecomputeVertexArray(true);
		}
	}
}

void RSfml::RGDXTPSprite::SetVertWorldSize(const sf::Vector2f InSize)
{
	if (m_worldSize != InSize)
	{
		m_worldSize = InSize;
		m_bVerticesDirty = true;
		if (m_bAutoUpdateVerticeArray)
		{
			RecomputeVertexArray(true);
		}
	}
}

void RSfml::RGDXTPSprite::SetVertWorldSizeAndPositionFromWorldRect(const sf::FloatRect InWorldRect)
{
	m_worldSize = InWorldRect.size;
	sf::Vector2f originOffset = RSfml::mul(m_worldSize, m_originPercent);
	SetPosition(InWorldRect.position + originOffset);
	m_bVerticesDirty = true;
	if (m_bAutoUpdateVerticeArray)
	{
		RecomputeVertexArray(true);
	}
}

void RSfml::RGDXTPSprite::SetPosition(const sf::Vector2f InPosition)
{
	((sf::Transformable*)this)->setPosition(InPosition);
}

void RSfml::RGDXTPSprite::SetRotation(const sf::Angle InAngle)
{
	((sf::Transformable*)this)->setRotation(InAngle);
}

void RSfml::RGDXTPSprite::SetScale(const sf::Vector2f InScale)
{
	((sf::Transformable*)this)->setScale(InScale);
}

void RSfml::RGDXTPSprite::SetAlpha(const uint8_t InNewAlpha, const bool InApplyToVertArr)
{
	sf::Color newColor = m_color;
	newColor.a = InNewAlpha;
	SetColor(newColor, InApplyToVertArr);
}

void RSfml::RGDXTPSprite::SetColor(const sf::Color& InNewColor, const bool InApplyToVertArr)
{
	m_color = InNewColor;
	if (InApplyToVertArr)
	{
		size_t count = m_vertices.getVertexCount();
		for (size_t i = 0; i < count; ++i)
		{
			m_vertices[i].color = m_color;
		}
	}
}

void RSfml::RGDXTPSprite::SetAutoRecomputeValue(const bool bInAutoRecomputeValue)
{
	m_bAutoUpdateVerticeArray = bInAutoRecomputeValue;
	if (m_bAutoUpdateVerticeArray && m_bVerticesDirty)
	{
		RecomputeVertexArray(true);
	}
}

bool RGDXTPSprite::RecomputeVertexArray(bool bInForceRecompute)
{
	if (!bInForceRecompute && !m_bVerticesDirty)
	{
		return true;
	}
	bool bAllSucessfull = true;
	sf::FloatRect verticesRect = sf::FloatRect
	(
		sf::Vector2f(0.f, 0.f) - RSfml::mul(m_worldSize, m_originPercent),
		m_worldSize
	);
	m_vertices.clear();
	m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	switch (m_spriteType)
	{
	case RSfml::RGDXTPSpriteType::Strench:
		bAllSucessfull = RSfml::CreateSimpleSprite(/*InOut*/m_vertices, /*StartIndex*/0, m_textureData, verticesRect, m_color) && bAllSucessfull;
		break;
	case RSfml::RGDXTPSpriteType::NineSlice:
		bAllSucessfull = RSfml::Create9SliceSquareAutoScale(/*InOut*/m_vertices, /*StartIndex*/0, m_textureData, verticesRect, 2.f, m_color) && bAllSucessfull;
		break;
	case RSfml::RGDXTPSpriteType::Loop:
		bAllSucessfull = RSfml::CreateRepeatable(m_vertices, 0, m_textureData, verticesRect, m_color) && bAllSucessfull;
		break;
	default:
		bAllSucessfull = false;
		assert(false && "Unhandled exception !");
		break;
	}
	m_initialized = bAllSucessfull;
	m_bVerticesDirty = !bAllSucessfull;
	return bAllSucessfull;
}

void RGDXTPSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

	// apply the texture
	states.texture = m_texture;

	// you may also override states.shader or states.blendMode if you want

	// draw the vertex array
	target.draw(m_vertices, states);
}

RGDXTPMetaSprite::RGDXTPMetaSprite()
	:RGDXTPMetaSprite(std::vector<RGDXTPSprite>())
{
}


RGDXTPMetaSprite::RGDXTPMetaSprite(std::vector<RGDXTPSprite>&& InSprites)
	:sf::Drawable(), sf::Transformable(), 
	m_initialized(false), m_bVerticesDirty(true), m_bAutoUpdateVerticeArray(false),
	m_texture(nullptr), m_vertices(),
	m_sprites(std::move(InSprites)), m_verticeRect()
{
	if (m_sprites.size() > 0)
	{
		m_texture = m_sprites[0].m_texture;
		RecomputeVertexArray();
	}
}

void RGDXTPMetaSprite::SetPosition(const sf::Vector2f InPosition)
{
	((sf::Transformable*)this)->setPosition(InPosition);
}

void RGDXTPMetaSprite::SetRotation(const sf::Angle InAngle)
{
	((sf::Transformable*)this)->setRotation(InAngle);
}

void RGDXTPMetaSprite::SetScale(const sf::Vector2f InScale)
{
	((sf::Transformable*)this)->setScale(InScale);
}

bool RGDXTPMetaSprite::RecomputeVertexArray(bool bInForceRecompute)
{
	bool bAllSucessfull = true;
	m_vertices.clear();
	m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);

	size_t nextVertexIndex = 0;
	for (size_t i = 0; i < m_sprites.size(); ++i)
	{
		RGDXTPSprite& sprite = m_sprites[i];
		sprite.RecomputeVertexArray(false);
		sf::Transform spriteTransform = sprite.getTransform();

		size_t nbNewVertices = sprite.m_vertices.getVertexCount();
		m_vertices.resize(nextVertexIndex + nbNewVertices);
		for (size_t spriteVertIndex = 0; spriteVertIndex < nbNewVertices; ++spriteVertIndex)
		{
			m_vertices[nextVertexIndex] = sprite.m_vertices[spriteVertIndex];
			m_vertices[nextVertexIndex].position = spriteTransform.transformPoint(m_vertices[nextVertexIndex].position);
			++nextVertexIndex;
		}
	}
	if (!m_texture && nextVertexIndex > 0)
	{
		m_texture = m_sprites[0].m_texture;
	}
	bAllSucessfull = (nextVertexIndex > 0) && bAllSucessfull;
	m_verticeRect = m_vertices.getBounds();

	m_initialized = bAllSucessfull;
	m_bVerticesDirty = !bAllSucessfull;
	return bAllSucessfull;
}

void RGDXTPMetaSprite::SetInnerSpritesAutoRecomputeValue(const bool bInAutoRecomputeValue)
{
	for (size_t i = 0; i < m_sprites.size(); ++i)
	{
		m_sprites[i].SetAutoRecomputeValue(bInAutoRecomputeValue);
	}
}

void RGDXTPMetaSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

	// apply the texture
	states.texture = m_texture;

	// you may also override states.shader or states.blendMode if you want

	// draw the vertex array
	target.draw(m_vertices, states);
}