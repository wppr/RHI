#pragma once

#include "ContextChoose.h"
#pragma comment(lib, "glfw3.lib")
#ifdef USE_OPENGL_CONTEXT
#include "GLContext.h"
#define GLFW_EXPOSE_NATIVE_WGL
#endif 

#ifdef USE_OPENGL_ES_CONTEXT
#include "WinEGLContext.h"
#define GLFW_EXPOSE_NATIVE_EGL
#endif 

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>

using namespace std;
class GLFWFrameWork
{
public:
	GLFWwindow* window = NULL;
	int w = 1024, h = 756;
	HWND hwnd;
	double LastTime;
	int samples = 4;
	bool pause = false;
	GLFWFrameWork()
	{
		
	} 
	void Init() {
		if (!glfwInit()) {
			fprintf(stderr, "ERROR: could not start GLFW3\n");
			exit(1);
		}	

		window = glfwCreateWindow(w, h, "Window", NULL, NULL);	
		if (!window) {
			fprintf(stderr, "ERROR: could not open window with GLFW3\n");
			glfwTerminate();
			exit(1);
		}
		glfwMakeContextCurrent(window);
		hwnd = glfwGetWin32Window(window);
		GLContext Context;
		Context.Init();

		glfwSwapInterval(0);
	}

	void Loop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();

			
			glfwSwapBuffers(window);

			//CalcFps();
		}

		glfwTerminate();

	}
};


