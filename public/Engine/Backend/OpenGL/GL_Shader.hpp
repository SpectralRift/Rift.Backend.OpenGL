#pragma once

#include <Engine/Core/Runtime/Graphics/IShader.hpp>

namespace engine::backend::ogl {
    struct GLShader : public core::runtime::graphics::IShader {
        GLShader() : m_ShaderHandle(-1) {}

        bool Compile() override;

        void Destroy() override;

        void SetSource(std::string_view source, core::runtime::graphics::ShaderType type) override;

        std::string GetSource() override;

        std::string GetCompileLog() override;

        bool IsCompiled() override;

        bool UseCompiledShader(const std::span<unsigned char>& data, core::runtime::graphics::ShaderType type) override {
            return false;
        }

        std::span<unsigned char> GetCompiledShader() override {
            return {};
        }

        core::runtime::graphics::ShaderCapsFlags GetImplCapabilities() const override {
            return core::runtime::graphics::ShaderCapsFlags::SHADER_CAPS_NONE;
        }

        unsigned int GetHandle() const {
            return m_ShaderHandle;
        };
    protected:
        unsigned int m_ShaderHandle;
    };
}