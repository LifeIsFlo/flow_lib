#include "flow_util.h"

namespace flow::util
{
    void string_split(std::string_view in, std::vector<std::string_view>& out, char sep)
    {
        out.clear();
        size_t i = 0;
        while (i < in.size())
        {
            while (i < in.size() && in[i] == sep)
                ++i;
            size_t start = i;
            while (i < in.size() && in[i] != sep)
                ++i;
            if (i > start)
                out.push_back(in.substr(start, i - start));
        }
    }
}
