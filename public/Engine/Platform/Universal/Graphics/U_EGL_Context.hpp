#pragma once

#include <Engine/Core/Runtime/Graphics/IGraphicsContext.hpp>
#include <Engine/EGLHeader.hpp>

namespace engine::platform::universal {
    struct UEGLContext : public core::runtime::graphics::IGraphicsContext {
        UEGLContext(core::runtime::IWindow *win);

        virtual ~UEGLContext() = default;

        bool Create() override;

        void Bind() override;

        void Discard() override;

        void Destroy() override;

        void Present() override;

        core::runtime::graphics::IGraphicsBackend *GetBackend() override;

        core::runtime::IWindow *GetOwnerWindow() override;

    protected:
        std::unique_ptr<core::runtime::graphics::IGraphicsBackend> m_Backend;
        core::runtime::IWindow *m_Window;
        EGLDisplay m_EGLDisplay;
        EGLSurface m_EGLSurface;
        EGLContext m_EGLContext;
    };
}