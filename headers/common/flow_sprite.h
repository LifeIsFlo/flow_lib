#pragma once
#include "flow_color.h"
#include <vector>

namespace flow
{
    class Sprite
    {
        std::vector<Color> _colorData = {};
        unsigned int _width = 0;
        unsigned int _height = 0;

    public:
        Sprite(std::vector<Color>& colorData, unsigned int width, unsigned int height)
            : _colorData(colorData), _width(width), _height(height)
        {
        }

        unsigned int GetWidth()
        {
            return _width;
        }

        unsigned int GetHeight()
        {
            return _height;
        }

        std::vector<Color> GetData()
        {
            return _colorData;
        }
    };
}