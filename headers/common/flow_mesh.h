#pragma once
#include "flow_vertex.h"
#include <span>
#include <string>
#include <vector>

namespace flow
{
    struct Mesh
    {
    private:
        std::vector<unsigned int> m_indices;
        std::vector<Vertex> m_vertices;

    public:
        std::string name;

    public:
        Mesh(std::vector<unsigned int> indices, std::vector<Vertex> vertices) : m_indices(std::move(indices)), m_vertices(std::move(vertices))
        {
        }
        ~Mesh();
        std::vector<Vertex>& GetVertices();
        std::vector<unsigned int>& GetIndices();
        unsigned int GetIndexCount() const;
    };
}