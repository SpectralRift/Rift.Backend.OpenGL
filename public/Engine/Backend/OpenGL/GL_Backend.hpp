#pragma once

#include <Engine/Core/Runtime/Graphics/IGraphicsBackend.hpp>

namespace engine::backend::ogl {
    struct GLBackend : public core::runtime::graphics::IGraphicsBackend {
        bool Initialize() override;

        void Shutdown() override;

        std::string GetName() const override;

        std::string GetIdentifier() const override;

        void SetViewport(core::math::Vector2 pos, core::math::Vector2 size) override;

        void SetScissor(core::math::Vector2 start, core::math::Vector2 size) override;

        void EnableFeatures(core::runtime::graphics::BackendFeature featuresMask) override;

        void DisableFeatures(core::runtime::graphics::BackendFeature featuresMask) override;

        core::runtime::graphics::BackendFeature GetActiveFeatures() override;

        void Clear(core::runtime::graphics::Color color) override;

        std::unique_ptr<core::runtime::graphics::IVertexBuffer> CreateVertexBuffer() override;

        std::unique_ptr<core::runtime::graphics::IShader> CreateShader() override;

        std::unique_ptr<core::runtime::graphics::IShaderProgram> CreateShaderProgram() override;

        std::unique_ptr<core::runtime::graphics::ITexture> CreateTexture() override;

    protected:
        uint32_t m_ActiveFeatures = 0;
    };
}
