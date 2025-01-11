#include "RSFML/Tools/Textures/RGDXTPAtlasReader.h"
#include <iostream> 
#include <fstream> 
#include <sstream>
#include <array>
#include <algorithm>

using namespace RSfml;

static bool FillFileData(std::ifstream& InOutFileStream, std::string& InOutLineBuff, RGDXTPAtlasData& InOutAtlasData);
static bool FillTextureData(std::ifstream& InOutFileStream, std::string& InOutLineBuff, RGDXTPAtlasTextureData& InOutTextureData);

/// <summary>
/// parse a string of ','  separated uint values
/// </summary>
/// <typeparam name="TUint">Uint type</typeparam>
/// <param name="InValue">the string value to parse</param>
/// <param name="InOutArray">The array to fill</param>
/// <param name="arraySize">max array size</param>
/// <returns>The number of read value</returns>
template <typename TUint = uint32_t>
size_t ReadUintArray(const std::string& InValue, TUint* InOutArray, const size_t arraySize)
{
    std::stringstream valueStream(InValue);
    std::string localBuffer;
    size_t index = 0;
    while (std::getline(valueStream, localBuffer, ',') && index < arraySize)
    {
        InOutArray[index] = (static_cast<TUint>(std::stoi(localBuffer)));
        ++index;
    }
    return index;
}

template <typename TUint = uint32_t>
bool ReadUintArrayBMatchSize(const std::string& InValue, TUint* InOutArray, const size_t arraySize)
{
    return ReadUintArray<TUint>(InValue, InOutArray, arraySize) == arraySize;
}


bool RGDXTPAtlasReader::ReadFile(const std::string_view InFilePath, RGDXTPAtlasData& OutAtlasData)
{
    OutAtlasData = RSfml::RGDXTPAtlasData::DEFAULT;
    bool bAllSucessfull = true;
    std::string filePath = std::string(InFilePath);
    std::ifstream file(filePath);
    if (!file) { return bAllSucessfull = false; }

    std::string line;
    if (!FillFileData(/*InOut*/file, /*InOut*/line, /*InOut*/OutAtlasData))
    {
        return bAllSucessfull = false;
    }
    while (!line.empty())
    {
        RGDXTPAtlasTextureData textureData;
        if (FillTextureData(/*InOut*/file, /*InOut*/line, /*InOut*/textureData))
        {
            std::copy(std::begin(OutAtlasData.Size), std::end(OutAtlasData.Size), std::begin(textureData.ParentTextureSize));
            OutAtlasData.TextureDatas.emplace(textureData.ImageName, textureData);
            
        }
        else
        {
            bAllSucessfull = false;
        }
    }
	return true;
}



static bool FillFileData(std::ifstream& InOutFileStream, std::string& InOutLineBuff, RGDXTPAtlasData& InOutAtlasData)
{
    do
    {
        if (InOutLineBuff.empty())
        {
            continue;
        }

        if
            (
                InOutLineBuff.find(".png") != std::string::npos
                ||
                InOutLineBuff.find(".jpg") != std::string::npos
             )
        {
            InOutAtlasData.ImageFileName = InOutLineBuff;

            while (std::getline(InOutFileStream, InOutLineBuff))
            {
                auto splitter = InOutLineBuff.find(":");
                if (splitter == std::string::npos)
                {
                    //Reached end of FileData property, return;
                    return true;
                }
                std::string parameter = InOutLineBuff.substr(0, splitter); 
                std::string value = InOutLineBuff.substr(splitter + 1);
                //std::stringstream valueStream(value);
                //std::string localBuffer;

                if (parameter == "size")
                {
                    bool bSucess = ReadUintArrayBMatchSize<uint32_t>(value, InOutAtlasData.Size, _countof(InOutAtlasData.Size));
                }
                if (parameter == "repeat")
                {
                    InOutAtlasData.Repeat = value;
                }

                continue;
            }
            continue;
        }
    } while (std::getline(InOutFileStream, InOutLineBuff));
    return true;
}

static bool FillTextureData(std::ifstream& InOutFileStream, std::string& InOutLineBuff, RGDXTPAtlasTextureData& InOutTextureData)
{
    InOutTextureData  = RSfml::RGDXTPAtlasTextureData::DEFAULT;
    InOutTextureData.ImageName = InOutLineBuff;
    while (std::getline(InOutFileStream, InOutLineBuff))
    {
        auto splitter = InOutLineBuff.find(":");
        if (splitter == std::string::npos)
        {
            //Reached end of TextureData property, return;
            return true;
        }
        std::string parameter = InOutLineBuff.substr(0, splitter);
        const std::string value = InOutLineBuff.substr(splitter + 1);

        if (parameter == "index")
        {
            InOutTextureData.Index = static_cast<uint32_t>(std::stoi(value));
        }
        else if (parameter == "bounds")
        {
            bool bSucess = ReadUintArrayBMatchSize<uint32_t>(value, InOutTextureData.Bounds, _countof(InOutTextureData.Bounds));
        }
        else if (parameter == "split")
        {
            bool bSucess = ReadUintArrayBMatchSize<uint32_t>(value, InOutTextureData.Split, _countof(InOutTextureData.Split));
        }
        else if (parameter == "pad")
        {
            bool bSucess = ReadUintArrayBMatchSize<uint32_t>(value, InOutTextureData.Pad, _countof(InOutTextureData.Pad));
        }
        else 
        {
            continue;
        }
    }
    return true;
}


//struct SpriteInfo {
//    std::string name;
//    int x, y, width, height;
//};
//
//class TextureAtlas {
//public:
//
//    bool loadFromFile(const std::string& filename) 
//    {
//        
//
//
//            SpriteInfo sprite;
//            sprite.name = line;
//            std::getline(file, line); // rotate: false
//            std::getline(file, line); // xy: x, y
//            std::istringstream xyStream(line.substr(4));
//            xyStream >> sprite.x;
//            xyStream.ignore(1); // Ignore the comma
//            xyStream >> sprite.y;
//
//            std::getline(file, line); // size: width, height
//            std::istringstream sizeStream(line.substr(6));
//            sizeStream >> sprite.width;
//            sizeStream.ignore(1); // Ignore the comma
//            sizeStream >> sprite.height;
//
//            sprites[sprite.name] = sprite;
//
//            // Skip other unnecessary lines (orig, offset, index)
//            for (int i = 0; i < 3; ++i) {
//                std::getline(file, line);
//            }
//        }
//
//        return true;
//    }
//
//    const SpriteInfo* getSpriteInfo(const std::string& name) const {
//        auto it = sprites.find(name);
//        if (it != sprites.end()) {
//            return &it->second;
//        }
//        return nullptr;
//    }
//};
//