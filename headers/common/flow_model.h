#pragma once
#include "flow_mesh.h"
#include <map>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace flow
{
    struct Model
    {
    private:
        std::map<std::string, std::unique_ptr<Mesh>> meshes;

    public:
        std::string name;

    public:
        Mesh* GetMesh(const std::string& meshName);
        void AddMesh(const std::string_view meshName, const std::vector<unsigned int> indices, const std::vector<Vertex> vertices);
    };
}