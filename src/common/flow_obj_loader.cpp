#include "flow_obj_loader.h"
#include "flow_io.h"
#include "flow_mesh.h"
#include "flow_model.h"
#include "flow_util.h"
#include <charconv>
#include <cstring>
#include <fast_float.h>
#include <numeric>
#include <string_view>

namespace flow
{

    inline const char* SkipSpaces(const char* p, const char* e)
    {
        while (p < e && *p == ' ')
            ++p;
        return p;
    }

    inline const char* ParseFloat(const char* p, const char* e, float& out)
    {
        p = SkipSpaces(p, e);
        auto r = fast_float::from_chars(p, e, out);
        return r.ec == std::errc{} ? r.ptr : p;
    }

    inline const char* ParseInt(const char* p, const char* e, int& out)
    {
        bool neg = false;
        if (p < e && *p == '-')
        {
            neg = true;
            ++p;
        }
        const char* start = p;
        int v = 0;
        while (p < e && static_cast<unsigned>(*p - '0') < 10)
        {
            v = v * 10 + (*p - '0');
            ++p;
        }
        if (p == start)
            return nullptr; // no digits
        out = neg ? -v : v;
        return p;
    }

    // "v", "v/vt", "v//vn" or "v/vt/vn"; advances p past the token, missing parts = 0
    inline bool ParseCorner(const char*& p, const char* e, int& v, int& vt, int& vn)
    {
        v = vt = vn = 0;
        const char* r = ParseInt(p, e, v);
        if (!r)
            return false;
        p = r;
        if (p < e && *p == '/')
        {
            ++p;
            if (p < e && *p != '/')
            {
                const char* r = ParseInt(p, e, vt);
                if (!r)
                    return false;
                p = r;
            }
            if (p < e && *p == '/')
            {
                ++p;
                const char* r = ParseInt(p, e, vn);
                if (!r)
                    return false;
                p = r;
            }
        }
        return true;
    }

    Model ObjLoader::LoadModel(const std::string& assetLocation)
    {
        Model model;
        bool modelFinished = false;

        std::ifstream f(assetLocation, std::ios::binary | std::ios::ate);
        size_t size = f.tellg();
        f.seekg(0);
        std::unique_ptr<char[]> buf(new char[size + 1]); // uninitialized
        f.read(buf.get(), size);
        buf[size] = '\0';

        const char* p = buf.get(); // file cursor, replaces `pos`
        const char* const fileEnd = p + size;

        // file-global pools, reserved once
        std::vector<Vector3> positions;
        positions.reserve(size / 150);
        std::vector<Vector3> normals;
        normals.reserve(size / 50);
        std::vector<Vector2> textureCoords;
        textureCoords.reserve(size / 50);
        std::vector<Vertex> poly;

        std::string_view lastNameCaptured;
        std::string meshName;

        int vi, ti, ni;
        float x = 0, y = 0, z = 0;

        while (!modelFinished)
        {
            bool meshFinished = false;
            std::vector<Vertex> vertices;
            vertices.reserve(size / 50);
            std::vector<unsigned int> indices;

            while (!meshFinished && p < fileEnd)
            {
                const char* s = p;
                const char* nl = static_cast<const char*>(memchr(p, '\n', fileEnd - p));
                const char* e = nl ? nl : fileEnd;
                p = nl ? nl + 1 : fileEnd;
                if (e > s && e[-1] == '\r')
                    --e;
                if (e - s < 2)
                    continue;

                switch (s[0])
                {
                case 'v': {
                    switch (s[1])
                    {
                    case ' ': {
                        const char* q = ParseFloat(s + 2, e, x);
                        q = ParseFloat(q, e, y);
                        ParseFloat(q, e, z);
                        positions.push_back({x, y, z});
                        break;
                    }
                    case 'n': {
                        const char* q = ParseFloat(s + 3, e, x);
                        q = ParseFloat(q, e, y);
                        ParseFloat(q, e, z);
                        normals.push_back({x, y, z});
                        break;
                    }
                    case 't': {
                        const char* q = ParseFloat(s + 3, e, x);
                        ParseFloat(q, e, y);
                        textureCoords.push_back({x, y});
                    }
                    }
                    break;
                }
                case 'f': {
                    if (s[1] != ' ')
                        break;
                    poly.clear();
                    const char* q = s + 2;
                    for (;;)
                    {
                        q = SkipSpaces(q, e);
                        if (q >= e)
                            break;
                        if (!ParseCorner(q, e, vi, ti, ni))
                            break;
                        // add bounds checks here if you load untrusted files
                        poly.push_back({positions[vi - 1],
                                        ni ? normals[ni - 1] : Vector3{},
                                        ti ? textureCoords[ti - 1] : Vector2{}});
                    }
                    for (size_t i = 1; i + 1 < poly.size(); ++i) // fan triangulation
                    {
                        vertices.push_back(poly[0]);
                        vertices.push_back(poly[i]);
                        vertices.push_back(poly[i + 1]);
                    }
                    break;
                }
                case 'o': {
                    std::string_view name(SkipSpaces(s + 1, e), 0);
                    name = std::string_view(name.data(), e - name.data());
                    if (!lastNameCaptured.empty())
                    {
                        meshName = lastNameCaptured;
                        meshFinished = true;
                    }
                    lastNameCaptured = name;
                    break;
                }
                }
            }

            // vertices.shrink_to_fit();
            indices.resize(vertices.size());
            std::iota(indices.begin(), indices.end(), 0u); // <numeric>
            if (!meshFinished)
                meshName = std::string(lastNameCaptured);
            model.AddMesh(meshName, std::move(indices), std::move(vertices));
            // flow::io::Console::WriteLine(meshName);
            modelFinished = !meshFinished;
        }
        model.name = assetLocation.substr(0, assetLocation.find_last_of('.'));
        return model;
    }
}
