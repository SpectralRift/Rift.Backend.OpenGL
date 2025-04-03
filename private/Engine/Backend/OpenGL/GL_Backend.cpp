#include <cstdio>

#include <Engine/GLHeader.hpp>

#include <Engine/Backend/OpenGL/GL_Backend.hpp>
#include <Engine/Backend/OpenGL/GL_VertexBuffer.hpp>
#include <Engine/Backend/OpenGL/GL_Shader.hpp>
#include <Engine/Backend/OpenGL/GL_Texture.hpp>
#include <Engine/Backend/OpenGL/GL_ShaderProgram.hpp>

#include <Engine/Runtime/Logger.hpp>

namespace engine::backend::ogl {
    static runtime::Logger g_LoggerGLBackend("GLBackend");
    
    bool GLBackend::Initialize() {
#ifdef GL_WITH_LOADER
        auto version = gladLoaderLoadGL();
        g_LoggerGLBackend.Log(runtime::LOG_LEVEL_INFO, "Initialized backend instance of OpenGL %d.%d", GLAD_VERSION_MAJOR(version),
               GLAD_VERSION_MINOR(version));

        return version != 0;
#else
        return true;
#endif
    }

    void GLBackend::Shutdown() {
#ifdef GL_WITH_LOADER
        gladLoaderUnloadGL();
#endif
    }

    std::string GLBackend::GetName() const {
        return "OpenGL";
    }

    std::string GLBackend::GetIdentifier() const {
        return "opengl";
    }

    void GLBackend::SetViewport(core::math::Vector2 pos, core::math::Vector2 size) {
        glViewport(static_cast<GLint>(pos.x),
                   static_cast<GLint>(pos.y),
                   static_cast<GLsizei>(size.x),
                   static_cast<GLsizei>(size.y));
    }

    void GLBackend::SetScissor(core::math::Vector2 start, core::math::Vector2 size) {
        GLint viewport[4]; // x, y, width, height
        glGetIntegerv(GL_VIEWPORT, viewport);

        // in OpenGL Y axis is inverted, so we need to read the current viewport to apply stuff
        glScissor(static_cast<GLint>(start.x),
                  viewport[3] - static_cast<GLint>(start.y + size.y),
                  static_cast<GLsizei>(size.x),
                  static_cast<GLsizei>(size.y));
    }

    void GLBackend::Clear(core::runtime::graphics::Color color) {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    std::unique_ptr<core::runtime::graphics::IVertexBuffer> GLBackend::CreateVertexBuffer() {
        return std::make_unique<ogl::GLVertexBuffer>();
    }

    std::unique_ptr<core::runtime::graphics::IShader> GLBackend::CreateShader() {
        return std::make_unique<ogl::GLShader>();
    }

    std::unique_ptr<core::runtime::graphics::IShaderProgram> GLBackend::CreateShaderProgram() {
        return std::make_unique<ogl::GLShaderProgram>();
    }

    std::unique_ptr<core::runtime::graphics::ITexture> GLBackend::CreateTexture() {
        return std::make_unique<ogl::GLTexture>();
    }

    void GLBackend::EnableFeatures(core::runtime::graphics::BackendFeature featuresMask) {
        if (featuresMask & core::runtime::graphics::BACKEND_FEATURE_SCISSOR_TEST) {
            glEnable(GL_SCISSOR_TEST);
        }

        if (featuresMask & core::runtime::graphics::BACKEND_FEATURE_ALPHA_BLENDING) {
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }

        m_ActiveFeatures |= featuresMask;
    }

    void GLBackend::DisableFeatures(core::runtime::graphics::BackendFeature featuresMask) {
        if (featuresMask & core::runtime::graphics::BACKEND_FEATURE_SCISSOR_TEST) {
            glDisable(GL_SCISSOR_TEST);
        }

        if (featuresMask & core::runtime::graphics::BACKEND_FEATURE_ALPHA_BLENDING) {
            glDisable(GL_BLEND);
        }

        m_ActiveFeatures &= ~featuresMask;
    }

    core::runtime::graphics::BackendFeature GLBackend::GetActiveFeatures() {
        return static_cast<core::runtime::graphics::BackendFeature>(m_ActiveFeatures);
    }
}