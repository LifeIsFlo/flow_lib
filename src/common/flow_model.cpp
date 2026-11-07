#include "flow_model.h"

namespace flow
{
    Mesh* Model::GetMesh(const std::string& meshName)
    {
        if (!meshes.contains(meshName))
            return nullptr;
        return meshes[meshName].get();
    }

    void Model::AddMesh(std::string_view meshName, std::vector<unsigned int> indices, std::vector<Vertex> vertices)
    {
        auto mesh = std::make_unique<Mesh>(std::move(indices), std::move(vertices));
        mesh->name = std::string(meshName);
        meshes[mesh->name] = std::move(mesh);
    }
}