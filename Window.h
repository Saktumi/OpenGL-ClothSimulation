#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "Land.h"
#include "shader.h"
#include "Camera.h"
#include "Cloth.h"


class Window
{
private:
	static bool pause;
	static bool wireMode;
	static bool cullingMode;

	static glm::vec3 wind;

public:
	static int width;
	static int height;
	static const char* windowTitle;

	static Land* land;
	static Cloth* cloth;

	static GLuint shaderProgram;

	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	static void idleCallback();
	static void displayCallback(GLFWwindow*);

	static void resetCamera();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);
};

#endif
