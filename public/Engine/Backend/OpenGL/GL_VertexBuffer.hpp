#pragma once

#include <Engine/Core/Runtime/Graphics/IVertexBuffer.hpp>

namespace engine::backend::ogl {
    struct GLVertexBuffer : public core::runtime::graphics::IVertexBuffer {
        bool Create() override;

        void Destroy() override;

        void Bind() override;

        void Unbind() override;

        void Draw() override;

        void Upload(
                const std::vector<core::runtime::graphics::Vertex> &data,
                core::runtime::graphics::PrimitiveType type,
                core::runtime::graphics::BufferUsageHint usage
        ) override;

        size_t Size() override;

        core::runtime::graphics::PrimitiveType GetPrimitiveType() override;

        std::vector<core::runtime::graphics::Vertex> Download() override;
    protected:
        unsigned int m_VaoHandle;
        unsigned int m_VboHandle;
        size_t m_VertexCount;
        core::runtime::graphics::BufferUsageHint m_UsageHint;
        core::runtime::graphics::PrimitiveType m_PrimType;
    };
}