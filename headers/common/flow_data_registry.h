#pragma once
#include "flow_fwd.h"
#include <memory>
#include <string>

namespace flow
{
    template <typename T>
    struct DataRegistry
    {
        std::unordered_map<std::u16string, std::unique_ptr<T>> registry;

        std::unique_ptr<T>& operator[](const std::string_view& dataLocation)
        {
            std::u16string wideKey(dataLocation.begin(), dataLocation.end());
            return registry[wideKey];
        }

        bool contains(const std::string_view& dataLocation) const
        {
            std::u16string wideKey(dataLocation.begin(), dataLocation.end());
            return registry.contains(wideKey);
        }

        void clear()
        {
            registry.clear();
        }

        auto begin()
        {
            return registry.begin();
        }
        auto end()
        {
            return registry.end();
        }

        auto begin() const
        {
            return registry.cbegin();
        }
        auto end() const
        {
            return registry.cend();
        }
    };
}
