#include <Engine/GLHeader.hpp>

#include <Engine/Backend/OpenGL/GL_Shader.hpp>
#include <Engine/Backend/OpenGL/GL_ShaderProgram.hpp>

#include <Engine/Runtime/Logger.hpp>

namespace engine::backend::ogl {
    static runtime::Logger g_LoggerGLShaderProgram("GLShaderProgram");
    
    bool GLShaderProgram::Link() {
        if (m_ProgramHandle == -1) {
            m_ProgramHandle = glCreateProgram();
        }

        g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_DEBUG, "Beginning to attach the shaders to the program...");

        for (auto sh: m_Shaders) {
            auto glShader = dynamic_cast<GLShader *>(sh.get());

            if (!glShader) {
                g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_ERROR, "The attached shader is NOT of GLShader type!");
                return false;
            }

            if (!glShader->IsCompiled()) {
                g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_WARNING, "The attached shader is not compiled! Compiling...");
                if (!glShader->Compile()) {
                    g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_ERROR, "One of the shaders failed to compile!\nReason: %s",
                           glShader->GetCompileLog().c_str());
                    return false;
                }
            }

            g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_DEBUG, "Attaching shader to program...");
            glAttachShader(m_ProgramHandle, glShader->GetHandle());
        }

        g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_DEBUG, "Linking shader program...");
        glLinkProgram(m_ProgramHandle);

        GLint linkStatus;
        glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == GL_TRUE) {
            // cleanup; destroy shader objects
            for (auto& sh: m_Shaders) {
                auto glShader = dynamic_cast<GLShader *>(sh.get());

                if (!glShader) {
                    g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_ERROR, "The attached shader is NOT of GLShader type!");
                    return false;
                }

                glShader->Destroy();
            }

            m_Shaders.clear();
        } else {
            g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_ERROR, "Shader Program linking failed!\nReason: %s", GetLinkLog().c_str());
        }

        return linkStatus == GL_TRUE;
    }

    void GLShaderProgram::Destroy() {
        if (m_ProgramHandle != -1) {
            glDeleteProgram(m_ProgramHandle);
            m_ProgramHandle = -1;
        }
    }

    void GLShaderProgram::Bind() {
        if (m_ProgramHandle != -1) {
            glUseProgram(m_ProgramHandle);
        }
    }

    void GLShaderProgram::Unbind() {
        glUseProgram(0);
    }

    void GLShaderProgram::AddShader(std::unique_ptr<core::runtime::graphics::IShader> shader) {
        if (!shader) {
            g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_ERROR, "Failed to add shader to the program: shader cannot be NULL!");
            return;
        }

        auto glShader = dynamic_cast<GLShader *>(shader.get());

        if (!glShader) {
            g_LoggerGLShaderProgram.Log(runtime::LOG_LEVEL_ERROR, "Shader object type is NOT GLShader!");
            return;
        }

        m_Shaders.emplace_back(std::move(shader));
    }

    std::string GLShaderProgram::GetLinkLog() {
        GLint logLength;
        glGetProgramiv(m_ProgramHandle, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetProgramInfoLog(m_ProgramHandle, logLength, nullptr, log.data());
            return {log.begin(), log.end()};
        }

        return "";
    }

    bool GLShaderProgram::IsLinked() {
        GLint linkStatus;
        glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &linkStatus);
        return linkStatus == GL_TRUE;
    }

    void GLShaderProgram::SetUniformMat4(std::string_view name, const glm::mat4 &mat) {
        Bind();
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramHandle, name.data()), 1, GL_FALSE, &mat[0][0]);
    }

    void GLShaderProgram::SetUniformI(std::string_view name, int val) {
        Bind();
        glUniform1i(glGetUniformLocation(m_ProgramHandle, name.data()), val);
    }
}
