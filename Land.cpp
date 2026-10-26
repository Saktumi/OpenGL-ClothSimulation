#include "Land.h"
#include <cmath>


////////////////////////////////////////////////////////////////////////////////

Land::Land(float width, glm::vec3 offset)
{
	halfWidth = width / 2.0f;
	model = glm::translate(offset) * glm::mat4(1.0f);

	color = glm::vec3(0.35f, 0.35f, 0.35f);  // 颜色

	positions = {
		// 前面
		glm::vec3(-width / 2,-1,width / 2),
		glm::vec3(width / 2,-1,width / 2),
		glm::vec3(width / 2,1,width / 2),
		glm::vec3(-width / 2,1,width / 2),

		// 后面
		glm::vec3(width / 2,-1,-width / 2),
		glm::vec3(-width / 2,-1,-width / 2),
		glm::vec3(-width / 2,1,-width / 2),
		glm::vec3(width / 2,1,-width / 2),

		// 上面
		glm::vec3(-width / 2,1,width / 2),
		glm::vec3(width / 2,1,width / 2),
		glm::vec3(width / 2,1,-width / 2),
		glm::vec3(-width / 2,1,-width / 2),

		// 下面
		glm::vec3(-width / 2,-1,-width / 2),
		glm::vec3(width / 2,-1,-width / 2),
		glm::vec3(width / 2,-1,width / 2),
		glm::vec3(-width / 2,-1,width / 2),

		// 左面
		glm::vec3(-width / 2,-1,-width / 2),
		glm::vec3(-width / 2,-1,width / 2),
		glm::vec3(-width / 2,1,width / 2),
		glm::vec3(-width / 2,1,-width / 2),

		// 右面
		glm::vec3(width / 2,-1,width / 2),
		glm::vec3(width / 2,-1,-width / 2),
		glm::vec3(width / 2,1,-width / 2),
		glm::vec3(width / 2,1,width / 2)
	};

	// 指定法线
	normals = {
		// 前面
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),

		// 后面
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),

		// 上面
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),

		// 下面
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),

		// 左面
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),

		// 右面
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0)
	};

	// 指定索引，用于绘制每个面
	indices = {
		0,1,2,	0,2,3,		// 前面
		4,5,6,	4,6,7,		// 后面
		8,9,10,	8,10,11,		// 上面
		12,13,14,	12,14,15,		// 下面
		16,17,18,	16,18,19,		// 左面
		20,21,22,	20,22,23,		// 右面
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);  // 启用顶点属性位置 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);  // 设置顶点属性指针

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);  // 启用顶点属性位置 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);  // 设置法线属性指针

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Land::~Land()
{
	// 删除 VBO 和 VAO
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void Land::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	// 激活着色器程序
	glUseProgram(shader);

	// 获取 uniform 变量的位置，并将数据传给着色器
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);  // 视图投影矩阵
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);  // 模型矩阵
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);  // 漫反射颜色

	// 绑定 VAO
	glBindVertexArray(VAO);

	// 使用 EBO 中的索引绘制三角形
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// 解绑 VAO 和着色器程序
	glBindVertexArray(0);
	glUseProgram(0);
}

void Land::update() {}

bool Land::collide(glm::vec3 pos) {
	// 计算地面上的一个点（位于局部 y=0 平面）
	glm::vec3 point = glm::vec3(model * glm::vec4(0, 1, 0, 1));
	// 地面的法线方向为 (0, 1, 0)
	glm::vec3 normal = glm::vec3(0, 1, 0);
	glm::vec3 localPos = glm::vec3(glm::inverse(model) * glm::vec4(pos, 1));

	if (std::abs(localPos.x) > halfWidth || std::abs(localPos.z) > halfWidth ||
		glm::dot(pos - point, normal) > 0) {
		return false;
	}
	else {
		return true;
	}
}

float Land::top() {
	return glm::vec3(model * glm::vec4(0, 1, 0, 1))[1];
}

