#pragma once

#include <renderer.hpp>
#include <textrenderer.hpp>
#include <input.hpp>
#include <texturesmanager.hpp>
#include <fontmanager.hpp>
#include <textqueue.hpp>

class Application;

extern bool ISFULLSCREEN;
extern Renderer *RENDERER; ///< The main renderer
extern TextRenderer *TEXT_RENDERER; ///< The text renderers
extern Input *INPUT; ///< The input manager
extern TexturesManager *TEXTURES_MANAGER; ///< The textures manager
extern FontManager *FONT_MANAGER; ///< The font manager
extern TextQueue *TEXT_QUEUE; ///< The text queue

namespace Window{
    extern int InitGlfwWindow(const char *window_name,bool resizable);
    extern void DeinitGlfwWindow();

    extern void ToggleFullScreen();
    extern void ToggleVSync();
    extern void SetResizable(bool resizable);

    extern bool GetKeyState(int key,int state);
    extern bool GetMouseButtonState(int button,int state);
    extern void GetMousePos(double *x,double *y);

    extern void ShowMetrics();
    extern void PrintDebugInfo();

    extern void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
    extern void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height);
    extern void GLAPIENTRY ErrorCallback(int error,const char *description);

    extern Application* CreateApplication();

    extern GLFWwindow *Window;

    extern float Width,Height;
    extern float BaseWidth,BaseHeight;
    extern float FullscreenWidth,FullscreenHeight;

    extern float FPS;
    extern float CurrentFrameTime;
    extern float LastFrameTime;
    extern float DeltaTime;
    extern float Alpha; //for interpolation

    extern bool IsFullscreen;
    extern bool IsVSync;
    extern bool ShowMetrics_;

    extern bool FramebufferUpdate;
    extern bool ProjUpdate;
    extern bool TextProjUpdate;
    #ifdef EDITOR
        extern bool SceneFramebufferUpdate;

        extern int DrawCalls;
        extern int VertexCount;
    #endif

    extern const float MAX_WIDTH;
    extern float MAX_HEIGHT;
};