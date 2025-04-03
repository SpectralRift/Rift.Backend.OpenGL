cmake_minimum_required(VERSION 3.14)
project(RiftEngine_Backend_OpenGL VERSION 0.1.1)

find_package(PkgConfig REQUIRED)

OPTION(GL_BACKEND_USE_GL "Include OpenGL Support" ON)
OPTION(GL_BACKEND_USE_ES "Include OpenGL ES Support" OFF)
OPTION(GL_BACKEND_FORCE_GL "Force OpenGL API to be used instead of OpenGL ES" OFF)
OPTION(GL_BACKEND_USE_EGL "Include EGL Support" OFF)
OPTION(GL_BACKEND_USE_EGL_LOADER "Use EGL Loader (provided by GLAD)" OFF)
OPTION(GL_BACKEND_USE_LOADER "Use OpenGL Loader (provided by GLAD)" ON)

set(RiftEngine_Backend_OpenGL_Sources
        private/Engine/Backend/OpenGL/GL_Backend.cpp
        private/Engine/Backend/OpenGL/GL_Shader.cpp
        private/Engine/Backend/OpenGL/GL_ShaderProgram.cpp
        private/Engine/Backend/OpenGL/GL_Texture.cpp
        private/Engine/Backend/OpenGL/GL_VertexBuffer.cpp)

set(RiftEngine_Backend_OpenGL_Libraries RiftEngine_Core_Runtime)

if (GL_BACKEND_USE_EGL)
    list(APPEND RiftEngine_Backend_OpenGL_Sources private/Engine/Platform/Universal/Graphics/U_EGL_Context.cpp)
endif ()

add_library(
        RiftEngine_Backend_OpenGL
        STATIC
        ${RiftEngine_Backend_OpenGL_Sources}
)

target_include_directories(
        RiftEngine_Backend_OpenGL
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

add_subdirectory("${PROJECT_SOURCE_DIR}/third_party/glad2/cmake" glad2)

if (GL_BACKEND_USE_EGL)
    pkg_search_module(EGL REQUIRED egl)

    if (NOT EGL_FOUND)
        message(FATAL_ERROR "Could not find EGL package.")
    endif ()

    target_include_directories(RiftEngine_Backend_OpenGL PUBLIC ${EGL_INCLUDE_DIRS})
    include_directories(${EGL_INCLUDE_DIRS})
    link_directories(${EGL_LIBRARY_DIRS})

    list(APPEND RiftEngine_Backend_OpenGL_Libraries ${EGL_LIBRARIES})
endif ()

if(GL_BACKEND_USE_GL)
    message("OpenGL support enabled")
    target_compile_definitions(RiftEngine_Backend_OpenGL PUBLIC GL_WITH_CORE)

    if (GL_BACKEND_USE_LOADER)
        message("GL Loader support enabled")

        glad_add_library(glad_gl_core STATIC REPRODUCIBLE LOADER API gl:core=4.6)
        target_compile_definitions(RiftEngine_Backend_OpenGL PUBLIC GL_WITH_LOADER)
        list(APPEND RiftEngine_Backend_OpenGL_Libraries glad_gl_core)
    endif()
endif ()

if (GL_BACKEND_USE_ES)
    message("OpenGL ES support enabled")
    target_compile_definitions(RiftEngine_Backend_OpenGL PUBLIC GL_WITH_GLES)

    if(GL_BACKEND_FORCE_GL)
        target_compile_definitions(RiftEngine_Backend_OpenGL PUBLIC GL_FORCE_API)
    endif()

    if (GL_BACKEND_USE_EGL_LOADER)
        message("EGL Loading will be provided by GLAD")

        glad_add_library(glad_egl STATIC REPRODUCIBLE LOADER API egl=1.5)
        target_compile_definitions(RiftEngine_Backend_OpenGL PUBLIC GL_WITH_EGL_LOADER)
        list(APPEND RiftEngine_Backend_OpenGL_Libraries glad_egl)
    else ()
        pkg_search_module(GLESv2 REQUIRED glesv2)

        if (NOT GLESv2_FOUND)
            message(FATAL_ERROR "Could not find GLESv2 package.")
        endif ()

        target_include_directories(RiftEngine_Backend_OpenGL PUBLIC ${GLESv2_INCLUDE_DIRS})
        include_directories(${GLESv2_INCLUDE_DIRS})
        link_directories(${GLESv2_LIBRARY_DIRS})

        list(APPEND RiftEngine_Backend_OpenGL_Libraries ${GLESv2_LIBRARIES})
    endif ()
endif ()

if (WIN32)
    list(APPEND RiftEngine_Backend_OpenGL_Libraries opengl32)
endif ()

target_link_libraries(RiftEngine_Backend_OpenGL ${RiftEngine_Backend_OpenGL_Libraries})