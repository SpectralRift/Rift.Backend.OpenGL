#include <stdexcept>
#include <sstream>
#include <vector>

#include <Engine/GLHeader.hpp>
#include <Engine/Backend/OpenGL/GL_Shader.hpp>

#include <Engine/Runtime/Logger.hpp>

namespace engine::backend::ogl {
    static runtime::Logger g_LoggerGLShader("GLShader");

    GLenum GL_MapShaderType(core::runtime::graphics::ShaderType type) {
        switch (type) {
            case core::runtime::graphics::ShaderType::SHADER_TYPE_VERTEX:
                return GL_VERTEX_SHADER;
            case core::runtime::graphics::ShaderType::SHADER_TYPE_FRAGMENT:
                return GL_FRAGMENT_SHADER;
            default:
                g_LoggerGLShader.Log(runtime::LOG_LEVEL_WARNING, "Failed to map shader type. Defaulting to vertex shader type!");
                return GL_VERTEX_SHADER;
        }
    }

    bool GLShader::Compile() {
        if (m_ShaderHandle == -1) {
            g_LoggerGLShader.Log(runtime::LOG_LEVEL_ERROR, "Shader handle is not initialized. Please call SetSource first.");
            return false;
        }

        glCompileShader(m_ShaderHandle);

        GLint compileStatus = GL_FALSE;
        glGetShaderiv(m_ShaderHandle, GL_COMPILE_STATUS, &compileStatus);

        return compileStatus == GL_TRUE;
    }

    void GLShader::Destroy() {
        if (m_ShaderHandle != -1) {
            glDeleteShader(m_ShaderHandle);
            m_ShaderHandle = -1;
        }
    }

    void GLShader::SetSource(std::string_view source, core::runtime::graphics::ShaderType type) {
        if (m_ShaderHandle == -1) {
            m_ShaderHandle = glCreateShader(GL_MapShaderType(type));
        }

        const char *sourceCStr = source.data();
        glShaderSource(m_ShaderHandle, 1, &sourceCStr, nullptr);
    }

    std::string GLShader::GetSource() {
        if (m_ShaderHandle == -1) {
            g_LoggerGLShader.Log(runtime::LOG_LEVEL_ERROR, "Shader handle is not initialized. Please call SetSource first.");
            return {};
        }

        GLint length = 0;
        glGetShaderiv(m_ShaderHandle, GL_SHADER_SOURCE_LENGTH, &length);

        if (length == 0) {
            return "";
        }

        std::vector<char> source(length);
        glGetShaderSource(m_ShaderHandle, length, nullptr, source.data());

        return {source.data()};
    }

    std::string GLShader::GetCompileLog() {
        if (m_ShaderHandle == -1) {
            g_LoggerGLShader.Log(runtime::LOG_LEVEL_ERROR, "Shader handle is not initialized. Please call SetSource first.");
            return {};
        }

        GLint logLength = 0;
        glGetShaderiv(m_ShaderHandle, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength == 0) {
            return "";
        }

        std::vector<char> log(logLength);
        glGetShaderInfoLog(m_ShaderHandle, logLength, nullptr, log.data());

        return {log.data()};
    }

    bool GLShader::IsCompiled() {
        if (m_ShaderHandle == -1) {
            g_LoggerGLShader.Log(runtime::LOG_LEVEL_ERROR, "Shader handle is not initialized. Please call SetSource first.");
            return false;
        }

        GLint compileStatus = GL_FALSE;
        glGetShaderiv(m_ShaderHandle, GL_COMPILE_STATUS, &compileStatus);

        return compileStatus == GL_TRUE;
    }

}
