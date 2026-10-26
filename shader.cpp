#include "shader.h"

enum ShaderType{vertex,fragment};

//加载单个着色器
GLuint LoadSingleShader(const char* shaderFilePath, ShaderType type)
{
	GLuint shaderID = 0;
	if (type == vertex)
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	if (type == fragment)
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);//创建着色器

	std::string shaderCode;
	std::ifstream shaderStream(shaderFilePath, std::ios::in);
	if (shaderStream.is_open())
	{
		// 逐行读取着色器代码
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else
	{
		// 如果文件无法打开，输出错误信息
		std::cerr << "无法打开文件 " << shaderFilePath << ". "
			<< "请确保文件存在且路径正确!"
			<< std::endl;
		glDeleteShader(shaderID);
		return 0;
	} //从着色器那读取代码

	GLint Result = GL_FALSE;
	GLint InfoLogLength = 0;

	//编译着色器
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// 检查编译状态
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 1)
	{
		// 获取着色器编译信息并输出
		std::vector<char> shaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
		std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
		std::cerr << msg << std::endl;
	}
	if (Result == GL_FALSE)
	{
		// 如果编译失败，释放着色器对象
		glDeleteShader(shaderID);
		return 0;
	}
	else
	{
		// 编译成功，输出信息
		if (type == vertex)
			printf("成功编译顶点着色器!\n");
		else if (type == fragment)
			printf("成功编译片段着色器!\n");
	}

	// 返回编译后的着色器 ID
	return shaderID;



}

// 加载顶点着色器和片段着色器，并链接程序
GLuint LoadShaders(const char* vertexFilePath, const char* fragmentFilePath)
{
	// 创建顶点着色器和片段着色器
	GLuint vertexShaderID = LoadSingleShader(vertexFilePath, vertex);
	GLuint fragmentShaderID = LoadSingleShader(fragmentFilePath, fragment);

	// 如果任一着色器加载失败，返回 0
	if (vertexShaderID == 0 || fragmentShaderID == 0)
	{
		if (vertexShaderID != 0) glDeleteShader(vertexShaderID);
		if (fragmentShaderID != 0) glDeleteShader(fragmentShaderID);
		return 0;
	}

	GLint Result = GL_FALSE;
	GLint InfoLogLength = 0;

	// 链接着色器程序
	printf("链接着色器程序\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// 检查程序链接状态
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 1)
	{
		// 获取程序链接信息并输出
		std::vector<char> ProgramErrorMessage(InfoLogLength);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
	}
	if (Result == GL_FALSE)
	{
		// 删除链接失败的程序与着色器对象
		glDeleteProgram(programID);
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		return 0;
	}
	else
	{
		// 程序链接成功，输出信息
		printf("成功链接着色器程序!\n");
	}

	// 链接成功后，分离并删除顶点和片段着色器
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// 返回链接后的程序 ID
	return programID;
}
