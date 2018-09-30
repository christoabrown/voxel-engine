#include "window.h"
#include "input.h"

Window::Window()
{
    width = 800;
    height = 600;
}

Window::~Window()
{
    glfwTerminate();
}

Window& Window::getInstance()
{
    static Window instance;
    return(instance);
}

void Window::glfwErrorCallback(int error, const char* description)
{
    std::cout << "ERROR::GLFW::" << description << std::endl;
}

void Window::glfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window::getInstance().setWidth(width);
    Window::getInstance().setHeight(height);
    Window::getInstance().setResized(true);
}

bool Window::init()
{
    Window &initWin = Window::getInstance();
    glfwSetErrorCallback(Window::glfwErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWin = glfwCreateWindow(width, height, "voxel-engine", nullptr, nullptr);
    if(!glfwWin)
    {
        return(false);
    }

    glfwSetFramebufferSizeCallback(glfwWin, Window::glfwFrameBufferSizeCallback);
    glfwSetKeyCallback(glfwWin, Input::keyCallback);
    //glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(glfwWin, Input::mousePosCallback);
    glfwSetMouseButtonCallback(glfwWin, Input::mouseButtonCallback);
       
    return(true);
}

bool Window::initGL()
{
    glfwMakeContextCurrent(Window::getGLFW());
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "ERROR::INITGL::GLAD FAILED TO LOAD" << std::endl;
        return(false);
    }
    int w, h;
    glfwGetFramebufferSize(Window::getGLFW(), &w, &h);
    Window::setWidth(w);
    Window::setHeight(h);
    Window::setResized(true);
    glViewport(0, 0, Window::getWidth(), Window::getHeight());

    ///DEFAULT GL STUFF
    glClearColor(211 / 255.0f, 224 / 255.0f, 232 / 255.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glfwSwapInterval(0);
    return(true);
}

GLFWwindow* Window::getGLFW()
{
    return(Window::getInstance().glfwWin);
}

int Window::getWidth()
{
    return(Window::getInstance().width);
}

int Window::getHeight()
{
    return(Window::getInstance().height);
}

bool Window::getResized()
{
    return(Window::getInstance().resized);
}

void Window::_setWidth(int _width) {width = _width;}
void Window::setWidth(int _width)
{
    Window::getInstance()._setWidth(_width);
}

void Window::_setHeight(int _height) {height = _height;}
void Window::setHeight(int _height)
{
    Window::getInstance()._setHeight(_height);
}

void Window::_setResized(bool _resized) {resized = _resized;}
void Window::setResized(bool _resized)
{
    Window::getInstance()._setResized(_resized);
}

