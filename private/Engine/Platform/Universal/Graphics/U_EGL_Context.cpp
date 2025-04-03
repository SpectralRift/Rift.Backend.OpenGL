#include <Engine/Platform/Universal/Graphics/U_EGL_Context.hpp>

#include <Engine/Core/Runtime/IWindow.hpp>
#include <Engine/Backend/OpenGL/GL_Backend.hpp>

namespace engine::platform::universal {
    UEGLContext::UEGLContext(core::runtime::IWindow *win) : m_Window{win}, m_EGLDisplay(EGL_NO_DISPLAY),
                                                            m_EGLSurface(EGL_NO_SURFACE),
                                                            m_EGLContext(EGL_NO_CONTEXT) {}

#define CASE_STR( value ) case value: return #value;
    const char* eglGetErrorString( EGLint error )
    {
        switch( error )
        {
            CASE_STR( EGL_SUCCESS             )
            CASE_STR( EGL_NOT_INITIALIZED     )
            CASE_STR( EGL_BAD_ACCESS          )
            CASE_STR( EGL_BAD_ALLOC           )
            CASE_STR( EGL_BAD_ATTRIBUTE       )
            CASE_STR( EGL_BAD_CONTEXT         )
            CASE_STR( EGL_BAD_CONFIG          )
            CASE_STR( EGL_BAD_CURRENT_SURFACE )
            CASE_STR( EGL_BAD_DISPLAY         )
            CASE_STR( EGL_BAD_SURFACE         )
            CASE_STR( EGL_BAD_MATCH           )
            CASE_STR( EGL_BAD_PARAMETER       )
            CASE_STR( EGL_BAD_NATIVE_PIXMAP   )
            CASE_STR( EGL_BAD_NATIVE_WINDOW   )
            CASE_STR( EGL_CONTEXT_LOST        )
            default: return "Unknown";
        }
    }
#undef CASE_STR

    bool UEGLContext::Create() {
        const EGLint attribs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 8,
                EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;
        EGLint format;

#ifdef GL_WITH_EGL_LOADER
        if (!gladLoaderLoadEGL(nullptr)) {
            printf("UEGLContext: Could not load EGL\n");
            return false;
        } else {
            printf("UEGLContext: EGL loaded through GLAD\n");
        }
#endif

        if(m_Window->GetDeviceCtx() != nullptr) {
            if ((m_EGLDisplay = eglGetDisplay(static_cast<EGLNativeDisplayType>(m_Window->GetDeviceCtx()))) == EGL_NO_DISPLAY) {
                printf("UEGLContext: Failed to get display from device context!\n");
            } else {
                printf("UEGLContext: Got display from device context!\n");
                goto LBL_CONTINUE_EGL_INIT;
            }
        }

        else if ((m_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
            printf("UEGLContext: Failed to get default display!\n");
            return false;
        } else {
            printf("UEGLContext: Got default display!\n");
        }

LBL_CONTINUE_EGL_INIT:
        EGLint eglMajor, eglMinor;

        if (!eglInitialize(m_EGLDisplay, &eglMajor, &eglMinor)) {
            printf("UEGLContext: Failed to initialize EGL on default display!\n");
            return false;
        } else {
            printf("UEGLContext: Initialized EGL %i.%i on default display!\n", eglMajor, eglMinor);
        }

        if (!eglChooseConfig(m_EGLDisplay, attribs, &config, 1, &numConfigs)) {
            printf("UEGLContext: Failed to choose EGL config!\n");
            return false;
        } else {
            printf("UEGLContext: Successfully chose EGL config!\n");
        }

        if (!eglGetConfigAttrib(m_EGLDisplay, config, EGL_NATIVE_VISUAL_ID, &format)) {
            printf("UEGLContext: Failed to get EGL config format!\n");
            return false;
        } else {
            printf("UEGLContext: Successfully got EGL config format!\n");
        }

        if ((m_EGLSurface = eglCreateWindowSurface(m_EGLDisplay, config, static_cast<EGLNativeWindowType>(m_Window->GetHandle()), nullptr)) == EGL_NO_SURFACE) {
            printf("error %s\n", eglGetErrorString(eglGetError()));
            printf("UEGLContext: Failed to create EGL surface using a window handle! Trying with default window handle...\n");
            if ((m_EGLSurface = eglCreateWindowSurface(m_EGLDisplay, config, nullptr, nullptr)) == EGL_NO_SURFACE) {
                printf("error %s\n", eglGetErrorString(eglGetError()));
                printf("UEGLContext: Failed to create EGL surface using a default window handle!\n");
                return false;
            } else {
                printf("UEGLContext: Successfully created EGL surface using a default window handle!\n");
            }
        } else {
            printf("UEGLContext: Successfully created EGL surface using a window handle!\n");
        }

#if defined(GL_WITH_CORE) && defined(GL_FORCE_API)
        if(!eglBindAPI(EGL_OPENGL_API)) {
            printf("UEGLContext: Failed to bind OpenGL API!\n");
            return false;
        } else {
            printf("UEGLContext: Successfully bound OpenGL API!\n");
        }
#endif

        if (!(m_EGLContext = eglCreateContext(m_EGLDisplay, config, nullptr, nullptr))) {
            printf("UEGLContext: Failed to create EGL context!\n");
            return false;
        } else {
            printf("UEGLContext: Successfully created EGL context!\n");
        }

        return true;
    }

    void UEGLContext::Bind() {
        eglMakeCurrent(m_EGLDisplay, m_EGLSurface, m_EGLSurface, m_EGLContext);
    }

    void UEGLContext::Discard() {
        eglMakeCurrent(m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    void UEGLContext::Destroy() {
        Discard();
        eglDestroyContext(m_EGLDisplay, m_EGLContext);
        eglDestroySurface(m_EGLDisplay, m_EGLSurface);
        eglTerminate(m_EGLDisplay);
    }

    void UEGLContext::Present() {
        eglSwapBuffers(m_EGLDisplay, m_EGLSurface);
    }

    core::runtime::graphics::IGraphicsBackend *UEGLContext::GetBackend() {
        if (!m_Backend) {
            m_Backend = std::make_unique<backend::ogl::GLBackend>();

            Bind();
            if(!m_Backend->Initialize()) {
                printf("U_EGLContext: Failed to initialize the backend.\n");
                return nullptr;
            }
            Discard();
        }

        return m_Backend.get();
    }

    core::runtime::IWindow *UEGLContext::GetOwnerWindow() {
        return m_Window;
    }
}