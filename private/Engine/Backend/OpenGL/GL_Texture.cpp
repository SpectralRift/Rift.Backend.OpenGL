#include <cstdio>

#include <Engine/GLHeader.hpp>

#include <Engine/Backend/OpenGL/GL_Texture.hpp>

namespace engine::backend::ogl {
    bool GLTexture::Create(const core::runtime::graphics::Bitmap &bitmap) {
        if (m_TexHandle != -1) {
            glDeleteTextures(1, &m_TexHandle);
        }

        glGenTextures(1, &m_TexHandle);
        glBindTexture(GL_TEXTURE_2D, m_TexHandle);

        auto size = bitmap.Size();
        auto pixels = bitmap.GetPixels();

        if (pixels.empty()) {
            printf("GLTexture: Bitmap data is empty.\n");
            return false;
        }

        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                static_cast<GLsizei>(size.x),
                static_cast<GLsizei>(size.y),
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                pixels.data()
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        return true;
    }

    core::runtime::graphics::Bitmap GLTexture::Download() {
        if (m_TexHandle == -1) {
            printf("GLTexture: Texture has not been created.\n");
            return {};
        }

        auto size = GetSize();
        std::vector<core::runtime::graphics::Color> pixels(size.x * size.y);

        // TODO: fix for OpenGL ES
        // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        return core::runtime::graphics::Bitmap(std::move(pixels), size);
    }

    core::math::Vector2 GLTexture::GetSize() {
        if (m_TexHandle == -1) {
            return {0, 0};
        }

        glBindTexture(GL_TEXTURE_2D, m_TexHandle);

        GLint width, height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        return {static_cast<float>(width), static_cast<float>(height)};
    }

    void GLTexture::Bind(int samplerSlot) {
        if (m_TexHandle == -1) {
            printf("GLTexture: Texture has not been created.\n");
            return;
        }

        glActiveTexture(GL_TEXTURE0 + samplerSlot);
        glBindTexture(GL_TEXTURE_2D, m_TexHandle);
    }

    void GLTexture::Unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::Destroy() {
        if (m_TexHandle != -1) {
            glDeleteTextures(1, &m_TexHandle);
            m_TexHandle = -1;
        }
    }
}
