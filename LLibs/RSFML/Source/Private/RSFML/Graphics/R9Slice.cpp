#include "RSFML/Graphics/R9Slice.h"
#include "RSFML/Tools/RMath.h"
#include "RSFML/Tools/RLog.h"

using RSfml::RectTopLeft, RSfml::RectTopRight, RSfml::RectBottomLeft, RSfml::RectBottomRight, RSfml::RectFromTopLeftBottomRight;


static float SnapToLowerStep(const float InValue, const float InStep)
{
    if (InStep <= 1) 
        return InValue;

    // Calculate the log base step of the value
    float logValue = std::log(InValue) / std::log(InStep);

    // Find the largest power of step less than or equal to the value
    float lowerStep = std::pow(InStep, std::floor(logValue));

    return lowerStep;
}


bool RSfml::Create9SliceSquareAutoScale(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Color& InVertexColor)
{
    sf::FloatRect fRectBrickTexPixel = InTextureAtlasData.RectFromBounds<float>();

    float xScale = (float)InWorldRect.size.x / fRectBrickTexPixel.size.x;
    float yScale = (float)InWorldRect.size.y / fRectBrickTexPixel.size.y;
    float selectedScale = std::min(xScale, yScale);
    return RSfml::Create9SliceSquare(InOutVertArray, InStartIndex, InTextureAtlasData, InWorldRect, sf::Vector2f(selectedScale, selectedScale), InVertexColor);
}

bool RSfml::Create9SliceSquareAutoScale(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, float InExpSteps, const sf::Color& InVertexColor)
{
    sf::FloatRect fRectBrickTexPixel = InTextureAtlasData.RectFromBounds<float>();

    float xScale = (float)InWorldRect.size.x / fRectBrickTexPixel.size.x;
    float yScale = (float)InWorldRect.size.y / fRectBrickTexPixel.size.y;
    float selectedScale = std::min(xScale, yScale);
    selectedScale = SnapToLowerStep(selectedScale, InExpSteps);
    return RSfml::Create9SliceSquare(InOutVertArray, InStartIndex, InTextureAtlasData, InWorldRect, sf::Vector2f(selectedScale, selectedScale), InVertexColor);

}

bool RSfml::Create9SliceSquare(sf::VertexArray& InOutVertArray, const size_t InStartIndex, const RSfml::RGDXTPAtlasTextureData& InTextureAtlasData, const sf::FloatRect& InWorldRect, const sf::Vector2f& InBorderScaling, const sf::Color& InVertexColor)
{
    const sf::Vector2f& InAtlasTextureSize = sf::Vector2f((float)InTextureAtlasData.ParentTextureSize[0], (float)InTextureAtlasData.ParentTextureSize[1]);

    if (InOutVertArray.getVertexCount() < (InStartIndex + RSfml::NINE_SLICE_VERT_COUNT))
    {
        InOutVertArray.resize(InStartIndex + RSfml::NINE_SLICE_VERT_COUNT);
    }
    //if (InOutVertArray.getVertexCount() < (InStartIndex + RSfml::NINE_SLICE_VERT_COUNT))
    //{
    //    RLog::Log(LOG_ERROR, "Not enought vertex space for a 9 slice !, needed {}, got {}", InStartIndex + RSfml::NINE_SLICE_VERT_COUNT, InOutVertArray.getVertexCount());
    //    return false;
    //}

    sf::FloatRect fRectBrickWorld = sf::FloatRect
    (
        (sf::Vector2f)InWorldRect.position,
        (sf::Vector2f)InWorldRect.size
    );

    sf::FloatRect fRectBrickTexPixel = InTextureAtlasData.RectFromBounds<float>();

    sf::FloatRect fRectBrickTexNrlzd = sf::FloatRect
    (
        sf::Vector2f(fRectBrickTexPixel.position.x / InAtlasTextureSize.x, fRectBrickTexPixel.position.y / InAtlasTextureSize.y),
        sf::Vector2f(fRectBrickTexPixel.size.x / InAtlasTextureSize.x, fRectBrickTexPixel.size.y / InAtlasTextureSize.y)
    );

    const sf::IntRect BrickTextureRect = InTextureAtlasData.RectFromBounds<int>();
    const sf::Vector2f fBordureToWorldSize = InBorderScaling; // RSfml::div((sf::Vector2f)InWorldRect.size, fRectBrickTexPixel.size);

    float fSplitXLeft = (float)InTextureAtlasData.Split[0];     float fSplitXLeftNrlzd = (fSplitXLeft / InAtlasTextureSize.x);
    float fSplitXRight = (float)InTextureAtlasData.Split[1];    float fSplitXRightNrlzd = (fSplitXRight / InAtlasTextureSize.x);
    float fSplitYUp = (float)InTextureAtlasData.Split[2];       float fSplitYUpNrlzd = (fSplitYUp / InAtlasTextureSize.y);
    float fSplitYDown = (float)InTextureAtlasData.Split[3];     float fSplitYDownNrlzd = (fSplitYDown / InAtlasTextureSize.y);

    float fSplitXLeftWorldS     = ( fSplitXLeft  * fBordureToWorldSize.x );
    float fSplitYUpWorldS       = ( fSplitYUp    * fBordureToWorldSize.y );
    float fSplitXRightWorldS    = ( fSplitXRight * fBordureToWorldSize.x );
    float fSplitYDownWorldS     = ( fSplitYDown  * fBordureToWorldSize.y );

    sf::Vector2f fWorldCenterSize    = sf::Vector2f(InWorldRect.size.x - fSplitXLeftWorldS - fSplitXRightWorldS, InWorldRect.size.y - fSplitYUpWorldS - fSplitYDownWorldS);
    sf::Vector2f fTextureCenterSize  = sf::Vector2f(fRectBrickTexPixel.size.x - fSplitXLeft - fSplitXRight,      fRectBrickTexPixel.size.y - fSplitYUp - fSplitYDown);


    const sf::Vector2f fSplitToWorldSize = RSfml::div(fWorldCenterSize, fTextureCenterSize);

    sf::FloatRect fRectSplitWorld = sf::FloatRect
    (
        sf::Vector2f(fRectBrickWorld.position.x + fSplitXLeftWorldS, fRectBrickWorld.position.y + fSplitYUpWorldS),
        sf::Vector2f(fRectBrickWorld.size.x - fSplitXLeftWorldS - fSplitXRightWorldS, fRectBrickWorld.size.y - fSplitYUpWorldS - fSplitYDownWorldS)

    );

    sf::FloatRect fRectSplitTexPixel = sf::FloatRect
    (
        sf::Vector2f(fRectBrickTexPixel.position.x + fSplitXLeft, fRectBrickTexPixel.position.y + fSplitYUp),
        sf::Vector2f(fRectBrickTexPixel.size.x - fSplitXLeft - fSplitXRight, fRectBrickTexPixel.size.y - fSplitYUp - fSplitYDown)
    );

    sf::FloatRect fRectSplitTexNrlzd = sf::FloatRect
    (
        sf::Vector2f(fRectBrickTexNrlzd.position.x + fSplitXLeftNrlzd, fRectBrickTexNrlzd.position.y + fSplitYUpNrlzd),
        sf::Vector2f(fRectBrickTexNrlzd.size.x - fSplitXLeftNrlzd - fSplitXRightNrlzd, fRectBrickTexNrlzd.size.y - fSplitYUpNrlzd - fSplitYDownNrlzd)
    );



    auto CreateSquare = [](const sf::Color& InInnerVertexColor, sf::VertexArray& InVertArray, const size_t InStartIndex, const sf::FloatRect& InWorldRect, const sf::FloatRect& InUVRect) -> void
        {
            //Triangle 0
            InVertArray[InStartIndex + 00].position     = RectTopLeft(InWorldRect);
            InVertArray[InStartIndex + 00].texCoords    = RectTopLeft(InUVRect);
            InVertArray[InStartIndex + 00].color = InInnerVertexColor;

            InVertArray[InStartIndex + 01].position = RectTopRight(InWorldRect);
            InVertArray[InStartIndex + 01].texCoords = RectTopRight(InUVRect);
            InVertArray[InStartIndex + 01].color = InInnerVertexColor;

            InVertArray[InStartIndex + 02].position = RectBottomLeft(InWorldRect);
            InVertArray[InStartIndex + 02].texCoords = RectBottomLeft(InUVRect);
            InVertArray[InStartIndex + 02].color = InInnerVertexColor;


            //Triangle 1
            InVertArray[InStartIndex + 03].position = RectTopRight(InWorldRect);
            InVertArray[InStartIndex + 03].texCoords = RectTopRight(InUVRect);
            InVertArray[InStartIndex + 03].color = InInnerVertexColor;

            InVertArray[InStartIndex + 04].position = RectBottomRight(InWorldRect);
            InVertArray[InStartIndex + 04].texCoords = RectBottomRight(InUVRect);
            InVertArray[InStartIndex + 04].color = InInnerVertexColor;

            InVertArray[InStartIndex + 05].position = RectBottomLeft(InWorldRect);
            InVertArray[InStartIndex + 05].texCoords = RectBottomLeft(InUVRect);
            InVertArray[InStartIndex + 05].color = InInnerVertexColor;


        };
    const sf::Vector2f briWPos = (sf::Vector2f)InWorldRect.position;
    std::array< float, 4> xPos = { RectTopLeft(fRectBrickWorld).x,      RectTopLeft(fRectSplitWorld).x,      RectTopRight(fRectSplitWorld).x,        RectTopRight(fRectBrickWorld).x };
    std::array< float, 4> yPos = { RectTopLeft(fRectBrickWorld).y,      RectTopLeft(fRectSplitWorld).y,      RectBottomLeft(fRectSplitWorld).y,      RectBottomLeft(fRectBrickWorld).y };

    std::array< float, 4> xUV = { RectTopLeft(fRectBrickTexPixel).x,    RectTopLeft(fRectSplitTexPixel).x,  RectTopRight(fRectSplitTexPixel).x,     RectTopRight(fRectBrickTexPixel).x };
    std::array< float, 4> yUV = { RectTopLeft(fRectBrickTexPixel).y,    RectTopLeft(fRectSplitTexPixel).y,  RectBottomLeft(fRectSplitTexPixel).y,   RectBottomLeft(fRectBrickTexPixel).y };


    std::array<std::array<sf::Vector2f, 4>, 4> verTex;
    // Vertexes
    //    X0    X1                X2      X3
    // Y0 x_____x_________________x_______x
    //    |00   |01               |02     |03
    // Y1 x-----x-----------------x-------x
    //    |04   |05               |06     |07
    // Y2 x-----x-----------------x-------x
    //    |08   |09               |10     |11
    // Y3 x_____x_________________x_______x
    //     12    13                14      15
    // Triangles
    // _________________________________
    // |00/01|      02/03      | 04/05 |
    // |-------------------------------|
    // |06/07|      08/09      | 10/11 |
    // |-------------------------------|
    // |12/13|      14/15      | 16/17 |
    // |_______________________________| 
    // 
    // Square
    // _________________________________
    // |  00 |        01       |   02  |
    // |-------------------------------|
    // |  03 |        04       |   05  |
    // |-------------------------------|
    // |  06 |        07       |   08  |
    // |_______________________________| 

    using v2f = sf::Vector2f;
    constexpr size_t VertexPerSquare = 6;
    ////////////////////////////////////////////////////////////
    ///////////////////////////Line 0///////////////////////////
    //Square 00
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 00),
        RectFromTopLeftBottomRight(v2f{ xPos[0], yPos[0] }, v2f{ xPos[1], yPos[1] }),
        RectFromTopLeftBottomRight(v2f{ xUV[0],  yUV[0] }, v2f{ xUV[1], yUV[1] })
    );
    //Square 01
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 01),
        RectFromTopLeftBottomRight(v2f{ xPos[1], yPos[0] }, v2f{ xPos[2], yPos[1] }),
        RectFromTopLeftBottomRight(v2f{ xUV[1],  yUV[0] }, v2f{ xUV[2],  yUV[1] })
    );
    //Square 02
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 02),
        RectFromTopLeftBottomRight(v2f{ xPos[2], yPos[0] }, v2f{ xPos[3], yPos[1] }),
        RectFromTopLeftBottomRight(v2f{ xUV[2],  yUV[0] }, v2f{ xUV[3],  yUV[1] })
    );
    ///////////////////////////Line 0///////////////////////////
    ////////////////////////////////////////////////////////////
    ///////////////////////////Line 1///////////////////////////
    //Square 03
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 03),
        RectFromTopLeftBottomRight(v2f{ xPos[0], yPos[1] }, v2f{ xPos[1], yPos[2] }),
        RectFromTopLeftBottomRight(v2f{ xUV[0],  yUV[1] }, v2f{ xUV[1],  yUV[2] })
    );
    //Square 04
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 04),
        RectFromTopLeftBottomRight(v2f{ xPos[1], yPos[1] }, v2f{ xPos[2], yPos[2] }),
        RectFromTopLeftBottomRight(v2f{ xUV[1],  yUV[1] }, v2f{ xUV[2],  yUV[2] })
    );
    //Square 05
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 05),
        RectFromTopLeftBottomRight(v2f{ xPos[2], yPos[1] }, v2f{ xPos[3], yPos[2] }),
        RectFromTopLeftBottomRight(v2f{ xUV[2],  yUV[1] }, v2f{ xUV[3],  yUV[2] })
    );
    ///////////////////////////Line 1///////////////////////////
    ////////////////////////////////////////////////////////////
    ///////////////////////////Line 2///////////////////////////
    //Square 06
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 06),
        RectFromTopLeftBottomRight(v2f{ xPos[0], yPos[2] }, v2f{ xPos[1], yPos[3] }),
        RectFromTopLeftBottomRight(v2f{ xUV[0],  yUV[2] }, v2f{ xUV[1],  yUV[3] })
    );
    //Square 07
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 07),
        RectFromTopLeftBottomRight(v2f{ xPos[1], yPos[2] }, v2f{ xPos[2], yPos[3] }),
        RectFromTopLeftBottomRight(v2f{ xUV[1],  yUV[2] }, v2f{ xUV[2],  yUV[3] })
    );
    //Square 08
    CreateSquare
    (InVertexColor, /*InOut*/InOutVertArray, InStartIndex + (VertexPerSquare * 8),
        RectFromTopLeftBottomRight(v2f{ xPos[2], yPos[2] }, v2f{ xPos[3], yPos[3] }),
        RectFromTopLeftBottomRight(v2f{ xUV[2],  yUV[2] }, v2f{ xUV[3],  yUV[3] })
    );
    ///////////////////////////Line 2///////////////////////////
    ////////////////////////////////////////////////////////////

    return true;
}
