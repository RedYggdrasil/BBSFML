#pragma once
#include "RSFML/Defines/RSFML.h"
#include "RSFML/Tools/Textures/RGDXTPAtlasData.h"

#include "RSFML/Tools/RCollection.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>


namespace RSfml
{
	enum class RGDXTPSpriteType : uint8_t
	{
		Strench = 0,
		NineSlice = 1,
		Loop = 2
	};
	class RGDXTPSprite : public sf::Drawable, public sf::Transformable
	{
	private:
		bool m_initialized;
		bool m_bVerticesDirty;
		bool m_bAutoUpdateVerticeArray;
		RGDXTPSpriteType m_spriteType;
		const sf::Texture* m_texture;
		RGDXTPAtlasTextureData m_textureData;
		sf::VertexArray m_vertices;

		sf::Color m_color;
		sf::Vector2f m_originPercent;
		sf::Vector2f m_worldSize;

	public:
		RGDXTPSprite();
		RGDXTPSprite(const RGDXTPSpriteType InSpriteType, const sf::Texture& InTexture, const RGDXTPAtlasTextureData& InTextureData, const sf::Color& InVertexColor = sf::Color::White);
		RGDXTPSprite(const RGDXTPSpriteType InSpriteType, const sf::Texture& InTexture, const RGDXTPAtlasTextureData& InTextureData, const sf::FloatRect& InWorldRect, const sf::Vector2f InVertexOrigin, const sf::Color& InVertexColor = sf::Color::White);
		RGDXTPSprite(const RGDXTPSpriteType InSpriteType, const sf::Texture& InTexture, const RGDXTPAtlasTextureData& InTextureData, const sf::Vector2f InWorldSize, const sf::Vector2f InVertexOrigin, const sf::Color& InVertexColor = sf::Color::White);

	public:
		/// <summary>
		/// SFML original setorigin method, dumb
		/// </summary>
		/// <param name="InTRSOrigin"></param>
		void SetTRSOrigin(const sf::Vector2f InTRSOrigin);
		/// <summary>
		/// Actually set origin by moving the vertice array
		/// </summary>
		/// <param name="InVertOrigin"></param>
		void SetVertOriginPercent(const sf::Vector2f InVertOrigin);

		void SetVertWorldSize(const sf::Vector2f InSize);


		void SetVertWorldSizeAndPositionFromWorldRect(const sf::FloatRect InWorldRect);

		void SetPosition(const sf::Vector2f InPosition);
		void SetRotation(const sf::Angle InAngle);
		void SetScale(const sf::Vector2f InScale);
		void SetAlpha(const uint8_t InNewAlpha, const bool InApplyToVertArr = true);
		void SetColor(const sf::Color& InNewColor, const bool InApplyToVertArr = true);
	public:
		void SetAutoRecomputeValue(const bool bInAutoRecomputeValue);
		bool RecomputeVertexArray(bool bInForceRecompute = false);

	public:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const R_PURE_OVERRIDE;
	public:
		friend class RGDXTPMetaSprite;
	};

	class RGDXTPMetaSprite : public sf::Drawable, public sf::Transformable
	{
	private:
		bool m_initialized;
		bool m_bVerticesDirty;
		bool m_bAutoUpdateVerticeArray;
		const sf::Texture* m_texture;
		sf::VertexArray m_vertices;
		std::vector<RGDXTPSprite> m_sprites;
		sf::FloatRect m_verticeRect;
	public:
		RGDXTPMetaSprite();
		RGDXTPMetaSprite(std::vector<RGDXTPSprite>&& InSprites);


	public:

		void SetPosition(const sf::Vector2f InPosition);
		void SetRotation(const sf::Angle InAngle);
		void SetScale(const sf::Vector2f InScale);
	public:
		bool RecomputeVertexArray(bool bInForceRecompute = false);

	public:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const R_PURE_OVERRIDE;
	private:
		void SetInnerSpritesAutoRecomputeValue(const bool bInAutoRecomputeValue);
	};
};