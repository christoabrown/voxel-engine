#pragma once
#include "../stdafx.h"
#include <mutex>
/// Singleton class
class Window
{
private:
    Window();
    ~Window();
    /// Delete these two so we don't duplicate
    Window(Window const&);
    void operator=(Window const&);
public:
    static Window& getInstance(); 
    
private:
    GLFWwindow *glfwWin;
    /// This is actually framebuffer size!
    int width, height;
    bool resized;
    /// Sets internal
    void _setWidth(int _width);
    void _setHeight(int _height);
    void _setResized(bool _resized);
public:
    bool init();
    static bool initGL();
    /// Callbacks
    static void glfwErrorCallback(int error, const char* description);
    static void glfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height);
    /// Gets
    static GLFWwindow* getGLFW();
    static int getWidth();
    static int getHeight();
    static bool getResized();
    /// Sets
    static void setWidth(int _width);
    static void setHeight(int _height);
    static void setResized(bool _resized);
};