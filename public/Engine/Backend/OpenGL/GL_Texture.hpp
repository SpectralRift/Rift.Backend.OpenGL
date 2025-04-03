#pragma once

#include <Engine/Core/Runtime/Graphics/ITexture.hpp>

namespace engine::backend::ogl {
    struct GLTexture : public core::runtime::graphics::ITexture {
        GLTexture() : m_TexHandle(-1) {}

        bool Create(const core::runtime::graphics::Bitmap &bitmap) override;

        void Destroy() override;

        core::runtime::graphics::Bitmap Download() override;

        core::math::Vector2 GetSize() override;

        void Bind(int samplerSlot) override;

        void Unbind() override;

        unsigned int GetHandle() const {
            return m_TexHandle;
        };
    protected:
        unsigned int m_TexHandle;
    };
}