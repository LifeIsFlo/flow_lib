#include "flow_dxutil.h"
#include <memory>

namespace flow::win32::util
{
    DirectX::XMMATRIX CreateMatrix(const flow::Vector3& position, const flow::Vector3& rotation, const flow::Vector3& scale)
    {
        auto scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

        auto xRot = DirectX::XMMatrixRotationX(rotation.x);
        auto yRot = DirectX::XMMatrixRotationY(rotation.y);
        auto zRot = DirectX::XMMatrixRotationZ(rotation.z);

        auto rot = xRot * yRot * zRot;

        auto positionMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

        return scaleMatrix * rot * positionMatrix;
    }

    DirectX::XMMATRIX CreateInverseMatrix(const flow::Vector3& position, const flow::Vector3& rotation, const flow::Vector3& scale)
    {
        DirectX::XMVECTOR det;

        return DirectX::XMMatrixInverse(&det, CreateMatrix(position, rotation, scale));
    }

    DirectX::XMMATRIX CreateProjectionMatrixOrthographic(const float& viewWidth, const float& viewHeight, const float& nearPlane, const float& farPlane)
    {
        auto projectionMatrix = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearPlane, farPlane);

        return projectionMatrix;
    }
    DirectX::XMMATRIX CreateProjectionMatrixPerspective(const float& fov, const float& aspectRatio,
                                                        const float& nearPlane, const float& farPlane)
    {
        auto projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);

        return projectionMatrix;
    }

    float4x4a XMMATRIXToFloat4x4A(const DirectX::XMMATRIX& xmMatrix)
    {
        return std::bit_cast<float4x4a>(xmMatrix);
    }

    std::wstring u16string_to_wstring(const std::u16string& u16_string)
    {
        return std::wstring(u16_string.begin(), u16_string.end());
    }
}