#pragma once
#include "flow_float4x4.h"
#include "flow_vector3.h"
#include <DirectXMath.h>
#include <windows.h>

namespace flow::win32::util
{
    DirectX::XMMATRIX CreateMatrix(const flow::Vector3& position, const flow::Vector3& rotation,
                                   const flow::Vector3& scale);

    DirectX::XMMATRIX CreateInverseMatrix(const flow::Vector3& position, const flow::Vector3& rotation,
                                          const flow::Vector3& scale);
    DirectX::XMMATRIX CreateProjectionMatrixOrthographic(const float& viewWidth, const float& viewHeight,
                                                         const float& nearPlane, const float& farPlane);
    DirectX::XMMATRIX CreateProjectionMatrixPerspective(const float& fov, const float& aspectRatio,
                                                        const float& nearPlane, const float& farPlane);
    float4x4a XMMATRIXToFloat4x4A(const DirectX::XMMATRIX& xmMatrix);

    std::wstring u16string_to_wstring(const std::u16string& u16_string);
}
