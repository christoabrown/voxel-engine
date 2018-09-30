#include "../stdafx.h"
#include "window.h"
#include "input.h"
#include "../renderer.h"

int main()
{
    if(!glfwInit())
    {
        std::cout << "ERROR::GLFW:: Init failed" << std::endl;
        return(-1);
    }
    Window &win = Window::getInstance();
    if(!win.init())
    {
        std::cout << "ERROR::WINDOW:: Init failed" << std::endl;
        return(-1);
    }
    std::thread renderThread(setupGL);
    while(!glfwWindowShouldClose(Window::getGLFW()))
    {
        glfwPollEvents();
    }
    renderThread.join();
    return(0);
}
