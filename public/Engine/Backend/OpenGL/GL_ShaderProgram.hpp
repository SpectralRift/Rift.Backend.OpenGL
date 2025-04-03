#pragma once

#include <vector>

#include <Engine/Core/Runtime/Graphics/IShaderProgram.hpp>

namespace engine::backend::ogl {
    struct GLShaderProgram : public core::runtime::graphics::IShaderProgram {
        GLShaderProgram() : m_ProgramHandle(-1) {}

        bool Link() override;

        void Destroy() override;

        void Bind() override;

        void Unbind() override;

        void AddShader(std::unique_ptr<core::runtime::graphics::IShader> shader) override;

        void SetUniformMat4(std::string_view name, const glm::mat4 &mat) override;

        void SetUniformI(std::string_view name, int val) override;

        std::string GetLinkLog() override;

        bool IsLinked() override;

    protected:
        unsigned int m_ProgramHandle;
        std::vector<std::shared_ptr<core::runtime::graphics::IShader>> m_Shaders;
    };
}