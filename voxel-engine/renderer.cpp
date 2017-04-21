#include "stdafx.h"
#include "renderer.h"
#include "shader.h"
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "lib\stb_image.h"
#include "input.h"
#include "safe_queue.h"
#include "engine_win32.h"
#include "world.h"
#include "block.h"
#include "vbo_arena.h"
#include "chunk_vao_manager.h"
#include "textRenderer.h"

#include <chrono>
#include <stdio.h>

typedef std::chrono::high_resolution_clock Clock;

HGLRC HRC;

SafeQueue<std::shared_ptr<ChunkMesh>> chunkMeshQueue;
SafeQueue<std::shared_ptr<ChunkMesh>> chunkUnmeshQueue;
GLuint shaderProgram, skyProgram, skyVAO;
std::chrono::duration<float> deltaTime;
auto lastFrame = Clock::now();
ChunkVaoManager chunkVaoManager;
GLuint textures[3];
unsigned long long int frameCount = 0;
double fpsInc = 0;
double frameRate = 0;

bool CreateGLContext(WinInfo *winInfo) 
{
	///Create temporary window to load extensions
	HWND tempWnd = CreateWindowW(winInfo->className, L"Voxel-Engine-Init", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, winInfo->hInstance, nullptr);
	HDC tempHDC = GetDC(tempWnd);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(tempHDC, &pfd);
	if (pixelFormat == 0) return false;

	BOOL result = SetPixelFormat(tempHDC, pixelFormat, &pfd);
	if (!result) return false;

	///Oldstyle context for glewInit
	HGLRC tempContext = wglCreateContext(tempHDC);
	wglMakeCurrent(tempHDC, tempContext);

	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		MessageBox(NULL, _T("GLEW is not enabled!"), NULL, MB_OK);
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);
	ReleaseDC(tempWnd, tempHDC);
	DestroyWindow(tempWnd);
	int formatList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_SAMPLE_BUFFERS_ARB, 1,
		WGL_SAMPLES_ARB, 8,
		0
	};
	///Set multisample pixel format
	int pixelFormatARB;
	UINT numFormats;
	BOOL formatARBp = wglChoosePixelFormatARB(winInfo->hdc, formatList, NULL, 1, &pixelFormatARB, &numFormats);
	BOOL wglFormat = SetPixelFormat(winInfo->hdc, pixelFormatARB, &pfd);
	if (wglFormat == 0) return false;

	int attributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};
	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		HRC = wglCreateContextAttribsARB(winInfo->hdc, 0, attributes);
	}
	else
	{
		HRC = tempContext;
	}
	if (!HRC) return false;
	else
		wglMakeCurrent(winInfo->hdc, HRC);

	return true;
}

GLuint loadTexture(std::string file)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, bpp;
	unsigned char *image = stbi_load(file.c_str(), &width, &height, &bpp, 3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return(texture);
}

void setData()
{
	glGenVertexArrays(1, &skyVAO);
	
	textures[0] = loadTexture("dirt.png");
	textures[1] = loadTexture("grass.png");
	textures[2] = loadTexture("stone.png");
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	chunkVaoManager = ChunkVaoManager();
}

void addChunkMeshQueue(std::shared_ptr<ChunkMesh> newChunk)
{
	chunkMeshQueue.enqueue(newChunk);
}

void addInstances()
{
	while (!chunkMeshQueue.empty())
	{
		std::shared_ptr<ChunkMesh> newChunk = chunkMeshQueue.dequeue();
		chunkVaoManager.addChunkMesh(newChunk.get());
	}
}

void removeChunkMeshQueue(std::shared_ptr<ChunkMesh> chunkMesh)
{
	chunkUnmeshQueue.enqueue(chunkMesh);
}

void removeChunkMeshes()
{
	while (!chunkUnmeshQueue.empty())
	{
		std::shared_ptr<ChunkMesh> chunkMesh = chunkUnmeshQueue.dequeue();
		if(chunkMesh->meshData.size() > 0)
			chunkVaoManager.deleteChunkMesh(chunkMesh.get());
	}
}

void drawScene(WinInfo *winInfo)
{
	auto currentFrame = Clock::now();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if(!chunkMeshQueue.empty() )
		addInstances();
	if (!chunkUnmeshQueue.empty())
		removeChunkMeshes();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glDisable(GL_BLEND);
	/// Sky shader stuff
	glUseProgram(skyProgram);
	glUniform3fv(1, 1, glm::value_ptr(getForward()));
	glUniform1f(2, winInfo->screenHeight);
	glUniform1f(3, winInfo->screenWidth);
	glUniform1f(4, getFov());
	glBindVertexArray(skyVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	///
	glUseProgram(shaderProgram);

	const GLuint MODEL_LOC = 16;
	const GLuint VIEW_LOC = 17;
	const GLuint PROJECTION_LOC = 18;
	
	glm::mat4 model;
	glm::mat4 view = GetViewMatrix();
	glm::mat4 projection = GetProjectionMatrix((GLfloat)winInfo->screenWidth, (GLfloat)winInfo->screenHeight);
	glUniformMatrix4fv(MODEL_LOC, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(VIEW_LOC, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(PROJECTION_LOC, 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	chunkVaoManager.drawChunks();
	
}

void renderFPS(TextRenderer &textRender, int x, int y)
{
	const double F_INTERVAL = 60.0;
	fpsInc += (1 / DeltaTime()) * (1 / F_INTERVAL);
	if (frameCount % (int)F_INTERVAL == 0)
	{
		frameRate = fpsInc;
		fpsInc = 0;
	}
	textRender.renderText("C:/Windows/Fonts/LiberationMono-Regular.ttf",std::to_string((int)frameRate) + " fps", x, y, 0.35, glm::vec3(1, 1, 1));
}

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		///TODO: make this write to a file
		std::cout << message << std::endl;
	}
}

void closeGL()
{
	glDeleteProgram(shaderProgram);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(HRC);
}

void setupGL(WinInfo *winInfo)
{
	CreateGLContext(winInfo);
	glClearColor(211 / 255.0f, 224 / 255.0f, 232 / 255.0f, 1.0f);
	shaderProgram = Shader("shaders/default.vert", "shaders/default.frag");
	skyProgram = Shader("shaders/sky.vert", "shaders/sky.frag");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(openglCallbackFunction, nullptr);

	glCullFace(GL_BACK);
	wglSwapIntervalEXT(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	setData();

	///Begin world
	std::shared_ptr<World> world = std::make_shared<World>(winInfo);
	std::shared_ptr<WorldGen> worldGen = std::make_shared<WorldGen>(winInfo, world);
	std::shared_ptr<WorldMesher> worldMesher = std::make_shared<WorldMesher>(winInfo, world);
	world->worldGen = worldGen;
	world->worldMesher = worldMesher;
	///]
	GLuint textShader = Shader("msdf_default.vert", "msdf_default.frag");
	TextRenderer textRender = TextRenderer(textShader);
	
	chunkVaoManager.initVaoPool();
	while (!winInfo->shuttingDown)
	{
		HandleInput(winInfo);
		if (winInfo->resized) 
		{ 
			glViewport(0, 0, winInfo->screenWidth, winInfo->screenHeight); winInfo->resized = false; 
			glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(winInfo->screenWidth), 0.0f, static_cast<GLfloat>(winInfo->screenHeight));
			glUseProgram(textShader);
			glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		}
		drawScene(winInfo);
		renderFPS(textRender, 2, winInfo->screenHeight - 20);
		SwapBuffers(winInfo->hdc);
		frameCount++;
	}
	closeGL();
}

///Time between frames in seconds
GLfloat DeltaTime()
{
	std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(deltaTime);
	return (ms.count() / 1000000.0f);
}