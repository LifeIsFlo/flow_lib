#pragma once
#include <string>

namespace flow
{
    struct Vector2
    {
        float x = 0;
        float y = 0;

        static float Distance(Vector2 first, Vector2 second);

        float Magnitude();

        Vector2 operator+(const Vector2& added) const
        {
            return {x + added.x, y + added.y};
        }

        Vector2 operator+=(const Vector2& added)
        {
            x += added.x;
            y += added.y;
            return *this;
        }

        Vector2 operator-(const Vector2& removed) const
        {
            return {x - removed.x, y - removed.y};
        }

        Vector2 operator-=(const Vector2& removed)
        {
            x -= removed.x;
            y -= removed.y;
            return *this;
        }

        std::string ToString()
        {
            return std::to_string(x) + ", " + std::to_string(y);
        }

        static const Vector2 ZERO;
        static const Vector2 ONE;
    };
}