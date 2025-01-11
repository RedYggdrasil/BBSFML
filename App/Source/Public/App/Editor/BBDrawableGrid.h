#pragma once
#include "App/Defines/pch.h"

namespace BB
{
	class BBDrawableGrid : public sf::Drawable, public sf::Transformable
	{
	protected:
		sf::FloatRect m_currentGridRect;
		sf::Vector2f m_currentGridStep;
		sf::Vector2f m_lineWidth;
		sf::Color m_vertexColor;
		sf::VertexArray m_vertices;
	public:
		BBDrawableGrid(const sf::FloatRect& InGridRect, const sf::Vector2f InGridStep, const sf::Vector2f InLineWidth, const sf::Color& InColor);
		void SetGridRect(const sf::FloatRect& InGridRect, bool bInRedraw = true);
		void SetGridStep(const sf::Vector2f InGridStep, bool bInRedraw = true);
		void SetLineWidth(const sf::Vector2f InLineWidth, bool bInRedraw = true);
		void SetColor(const sf::Color& InColor, bool bInRedraw = true);
		void SetAlpa(const uint8_t InNewAlpha, bool bInRedraw = true);

		void RecompteVertexColor();
		void RecompteVertexArray();

		inline sf::FloatRect GetGridRect() const { return m_currentGridRect; }
		inline sf::Vector2f GetGridStep() const { return m_currentGridStep; }
		inline sf::Color GetColor() const { return m_vertexColor; }
		~BBDrawableGrid();
	public:
		//Drawable interface
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const R_PURE_OVERRIDE;
	};
}