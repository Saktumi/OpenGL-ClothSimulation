#include "Window.h"
#include <chrono>

// 暂停指示器
bool Window::pause = false;
// 网格模式，0 代表面，1 代表线框
bool Window::wireMode = false;
// 剔除模式，0 关闭，1 开启
bool Window::cullingMode = false;

// 窗口属性
int Window::width;
int Window::height;
const char* Window::windowTitle = "My Cloth Simulation"; // 窗口标题

// 渲染对象
Land* Window::land;
Cloth* Window::cloth;

// 风力
glm::vec3 Window::wind = glm::vec3(0); // 默认风力方向

// 相机属性
Camera* Cam; // 相机对象

// 交互变量
bool LeftDown, RightDown; // 鼠标按钮按下状态
int MouseX, MouseY; // 鼠标位置

// 着色器程序的 id
GLuint Window::shaderProgram;

bool Window::initializeProgram()
{
	//创建一个包含顶点着色器和片段着色器的着色器程序
	shaderProgram = LoadShaders("shader.vert", "shader.frag");

	// 检查着色器程序是否成功创建。
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

//初始化着色器对象
bool Window::initializeObjects()
{
	// 创建地面和布料对象
	land = new Land(100, glm::vec3(0, -10, 0));
	cloth = new Cloth(50, 50, glm::vec3(0, 0, 0), land);
	cloth->blow(wind); // 给布料施加风力

	return true;
}

//清理资源
void Window::cleanUp()
{   
	//清理对象
	delete cloth;
	cloth = nullptr;
	delete land;
	land = nullptr;
	delete Cam;
	Cam = nullptr;
	
	//清理着色器对象
	glDeleteProgram(shaderProgram);
	shaderProgram = 0;
}

GLFWwindow* Window::createWindow(int width, int height)
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建 GLFW 窗口
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	if (!window)
	{  
		std::cerr << "there is no window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);//设置当前窗口上下文
	glewExperimental = GL_TRUE;
	GLenum glewStatus = glewInit();//提供现代化opengl功能；
	if (glewStatus != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW: "
			<< glewGetErrorString(glewStatus) << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return NULL;
	}
	glGetError();

	glfwSwapInterval(1);//交换间隔

	// 设置相机
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// 初始化交互变量
	LeftDown = RightDown = false;
	double cursorX = 0.0;
	double cursorY = 0.0;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	MouseX = static_cast<int>(cursorX);
	MouseY = static_cast<int>(cursorY);

	Window::resizeCallback(window,width,height);
	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
	if (width <= 0 || height <= 0) return;

	Window::width = width;
	Window::height = height;

	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
	Cam->Update();
}

void Window::idleCallback()
{
	// 执行必要的更新
	Cam->Update();

	static auto previousTime = std::chrono::steady_clock::now();
	static double accumulatedTime = 0.0;
	auto currentTime = std::chrono::steady_clock::now();
	double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();
	previousTime = currentTime;

	// 如果没有暂停，更新布料模拟
	if (!pause) {
		const float fixedStep = 0.001f;
		accumulatedTime += (elapsedTime > 0.05 ? 0.05 : elapsedTime);
		int stepCount = 0;
		while (accumulatedTime >= fixedStep && stepCount < 50) {
			cloth->update(fixedStep);
			accumulatedTime -= fixedStep;
			++stepCount;
		}
		if (stepCount == 50) accumulatedTime = 0.0;
	}
	else {
		accumulatedTime = 0.0;
	}
}

void Window::displayCallback(GLFWwindow* window) //渲染回调函数
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色和深度缓冲

	// 渲染地面和布料
	land->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	cloth->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	// 获取事件，包括输入（键盘、鼠标）和窗口大小调整等
	glfwPollEvents();
	// 交换缓冲区
	glfwSwapBuffers(window);
}

void Window::resetCamera()
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
	Cam->Update();
}

// 回调函数 - 处理交互
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// 检查键盘按键事件
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// 按下 ESC 键关闭窗口，退出程序
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_R:
			resetCamera(); // 重置相机
			break;

			// 暂停/恢复模拟
		case GLFW_KEY_SPACE:
			pause = !pause;
			break;

		case GLFW_KEY_BACKSPACE:
			cloth->toggleFree(); // 切换布料是否自由
			break;

			// 切换显示模式：面模式或线框模式
		case GLFW_KEY_P:
			wireMode = !wireMode;
			if (wireMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置为线框模式
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 设置为面模式
			}
			break;

			// 切换剔除模式
		case GLFW_KEY_C:
			cullingMode = !cullingMode;
			if (cullingMode) {
				glEnable(GL_CULL_FACE); // 启用剔除
				glCullFace(GL_BACK); // 剔除背面
			}
			else {
				glDisable(GL_CULL_FACE); // 禁用剔除
			}
			break;

			// 控制风力
		case GLFW_KEY_UP:
			wind -= glm::vec3(0, 0, 0.5); // 增加风速（z轴方向）
			cloth->blow(wind);
			std::cerr << "Wind Velocity: " << "(" <<
				wind.x << ", " <<
				wind.y << ", " <<
				wind.z << ")" << std::endl;
			break;

		case GLFW_KEY_DOWN:
			wind += glm::vec3(0, 0, 0.5); // 减小风速
			cloth->blow(wind);
			std::cerr << "Wind Velocity: " << "(" <<
				wind.x << ", " <<
				wind.y << ", " <<
				wind.z << ")" << std::endl;
			break;

		case GLFW_KEY_LEFT:
			wind -= glm::vec3(0.5, 0, 0); // 改变风速方向（x轴方向）
			cloth->blow(wind);
			std::cerr << "Wind Velocity: " << "(" <<
				wind.x << ", " <<
				wind.y << ", " <<
				wind.z << ")" << std::endl;
			break;

		case GLFW_KEY_RIGHT:
			wind += glm::vec3(0.5, 0, 0); // 改变风速方向（x轴方向）
			cloth->blow(wind);
			std::cerr << "Wind Velocity: " << "(" <<
				wind.x << ", " <<
				wind.y << ", " <<
				wind.z << ")" << std::endl;
			break;

		case GLFW_KEY_RIGHT_SHIFT:
			wind += glm::vec3(0, 0.5, 0); // 改变风速方向（y轴方向）
			cloth->blow(wind);
			std::cerr << "Wind Velocity: " << "(" <<
				wind.x << ", " <<
				wind.y << ", " <<
				wind.z << ")" << std::endl;
			break;

		case GLFW_KEY_RIGHT_CONTROL:
			wind -= glm::vec3(0, 0.5, 0); // 改变风速方向（y轴方向）
			cloth->blow(wind);
			std::cerr << "Wind Velocity: " << "(" <<
				wind.x << ", " <<
				wind.y << ", " <<
				wind.z << ")" << std::endl;
			break;

			// 控制布料位置和旋转
		case GLFW_KEY_W:
			cloth->translate(glm::vec3(0, 0, -0.05)); // 沿 z 轴向前移动
			break;

		case GLFW_KEY_S:
			cloth->translate(glm::vec3(0, 0, 0.05)); // 沿 z 轴向后移动
			break;

		case GLFW_KEY_A:
			cloth->translate(glm::vec3(-0.05, 0, 0)); // 沿 x 轴向左移动
			break;

		case GLFW_KEY_D:
			cloth->translate(glm::vec3(0.05, 0, 0)); // 沿 x 轴向右移动
			break;

		case GLFW_KEY_LEFT_SHIFT:
			cloth->translate(glm::vec3(0, 0.05, 0)); // 沿 y 轴向上移动
			break;

		case GLFW_KEY_LEFT_CONTROL:
			cloth->translate(glm::vec3(0, -0.05, 0)); // 沿 y 轴向下移动
			break;

		case GLFW_KEY_Q:
			cloth->rotate(glm::radians(1.0f)); // 顺时针旋转
			break;

		case GLFW_KEY_E:
			cloth->rotate(glm::radians(-1.0f)); // 逆时针旋转
			break;

		default:
			break;
		}
	}
}

// 鼠标事件回调
void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS); // 左键按下或抬起
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS); // 右键按下或抬起
	}
}

// 鼠标移动事件回调
void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// 移动相机
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate); // 水平旋转
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f)); // 垂直旋转
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist); // 缩放视距
	}
}


