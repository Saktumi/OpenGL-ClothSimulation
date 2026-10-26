#include "main.h"

//错误回调函数
void error_callback(int error,const char* description)
{
	std::cerr << description << std::endl;
}

//设置回调函数
void setup_callbacks(GLFWwindow* window)
{   
    //设置错误回调，窗口大小回调，按键回调
	glfwSetErrorCallback(error_callback);
	glfwSetWindowSizeCallback(window, Window::resizeCallback);
	glfwSetKeyCallback(window, Window::keyCallback);


	//设置鼠标按键和光标位置回调
	glfwSetMouseButtonCallback(window, Window::mouse_callback);
	glfwSetCursorPosCallback(window, Window::cursor_callback);
}

//设置openGL的默认参数
void setup_opengl_settings()
{
	glEnable(GL_DEPTH_TEST); //允许深度测试
	glDepthFunc(GL_LEQUAL); //更改深度比较模式，用z比较
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //多边形填充模式，用于填充正面与背面
	glClearColor(0.0, 0.0, 0.0, 0.0); //将清屏颜色设为黑色
}

int main()
{  
	glfwSetErrorCallback(error_callback);
	GLFWwindow* window = Window::createWindow(800, 600);
	if (!window) return EXIT_FAILURE;
	
	//设置回调函数
	setup_callbacks(window);
	//初始设置
	setup_opengl_settings();

	//初始化着色器程序
	if (!Window::initializeProgram())
	{
		Window::cleanUp();
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}
	//初始化渲染对象与指针
	if (!Window::initializeObjects())
	{
		Window::cleanUp();
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	while (!glfwWindowShouldClose(window))
	{
		Window::idleCallback();//空闲回调函数
		Window::displayCallback(window);//渲染回调函数
	}
    
	Window::cleanUp();
	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
