#include "Cloth.h"

Cloth::Cloth(int width, int height, glm::vec3 offset, Land* land) :
	width(width), height(height), land(land), localWind(glm::vec3(0)),
	worldWind(glm::vec3(0)), translation(offset) {
	// 初始化模型矩阵和颜色
	model = glm::translate(offset) * glm::mat4(1.0f);
	color = glm::vec3(0.5f, 0.5f, 1.0f);  // 设置布料的颜色为红色

	// 创建布料粒子并初始化
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			// 计算每个粒子的位置
			glm::vec3 pos = glm::vec3(-0.1f * float(width) / 2.0f, 0.1f * float(height) / 2.0f, 0) +
				glm::vec3(float(i)) * glm::vec3(0, -0.1f, 0) +
				glm::vec3(float(j)) * glm::vec3(0.1f, 0, 0);
			Particle* particle = new Particle(pos, glm::vec3(0.1f), 0.1f); // 创建粒子
			particles.push_back(particle);  // 将粒子加入到布料中
			positions.push_back(pos);  // 保存粒子的位置
			if (i == 0) {  // 如果是布料的第一行，固定该行的粒子
				particle->setFix();  // 设置粒子为固定状态
				fixedIdx.push_back(i * width + j);  // 记录固定粒子的索引
			}
		}
	}

	// 创建布料的三角形面片（上三角形）
	for (int i = 0; i < height - 1; ++i) {
		for (int j = 0; j < width - 1; ++j) {
			int index1 = i * width + j;
			int index2 = (i + 1) * width + j;
			int index3 = i * width + j + 1;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			Particle* p3 = particles[index3];
			Triangle* triangle = new Triangle(p1, p2, p3);

			indices.push_back(index1);
			indices.push_back(index2);
			indices.push_back(index3);
			triangles.push_back(triangle);
		}
	}

	// 创建布料的三角形面片（下三角形）
	for (int i = 1; i < height; ++i) {
		for (int j = 0; j < width - 1; ++j) {
			int index1 = i * width + j;
			int index2 = i * width + j + 1;
			int index3 = (i - 1) * width + j + 1;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			Particle* p3 = particles[index3];
			Triangle* triangle = new Triangle(p1, p2, p3);

			indices.push_back(index1);
			indices.push_back(index2);
			indices.push_back(index3);
			triangles.push_back(triangle);
		}
	}

	// 创建水平弹簧阻尼器
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width - 1; ++j) {
			int index1 = i * width + j;
			int index2 = i * width + j + 1;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.1f);

			springDampers.push_back(springDamper);
		}
	}

	// 创建垂直弹簧阻尼器
	for (int i = 0; i < height - 1; ++i) {
		for (int j = 0; j < width; ++j) {
			int index1 = i * width + j;
			int index2 = (i + 1) * width + j;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.1f);

			springDampers.push_back(springDamper);
		}
	}

	// 创建对角线（左上到右下）弹簧阻尼器
	for (int i = 0; i < height - 1; ++i) {
		for (int j = 0; j < width - 1; ++j) {
			int index1 = i * width + j;
			int index2 = (i + 1) * width + j + 1;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.1f * glm::sqrt(2.0f));

			springDampers.push_back(springDamper);
		}
	}

	// 创建对角线（左下到右上）弹簧阻尼器
	for (int i = 1; i < height; ++i) {
		for (int j = 0; j < width - 1; ++j) {
			int index1 = i * width + j;
			int index2 = (i - 1) * width + j + 1;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.1f * glm::sqrt(2.0f));

			springDampers.push_back(springDamper);
		}
	}

	// 创建大范围水平弹簧阻尼器
	for (int i = 0; i < height - 2; i += 2) {
		for (int j = 0; j < width - 2; j += 2) {
			int index1 = i * width + j;
			int index2 = i * width + j + 2;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.2f);

			springDampers.push_back(springDamper);
		}
	}

	// 创建大范围垂直弹簧阻尼器
	for (int i = 0; i < height - 2; i += 2) {
		for (int j = 0; j < width - 2; j += 2) {
			int index1 = i * width + j;
			int index2 = (i + 2) * width + j;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.2f);

			springDampers.push_back(springDamper);
		}
	}

	// 创建大范围对角线（左上到右下）弹簧阻尼器
	for (int i = 0; i < height - 2; i += 2) {
		for (int j = 0; j < width - 2; j += 2) {
			int index1 = i * width + j;
			int index2 = (i + 2) * width + j + 2;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.2f * glm::sqrt(2.0f));

			springDampers.push_back(springDamper);
		}
	}

	// 创建大范围对角线（左下到右上）弹簧阻尼器
	for (int i = 2; i < height; i += 2) {
		for (int j = 0; j < width - 2; j += 2) {
			int index1 = i * width + j;
			int index2 = (i - 2) * width + j + 2;
			Particle* p1 = particles[index1];
			Particle* p2 = particles[index2];
			SpringDamper* springDamper = new SpringDamper(p1, p2, 0.2f * glm::sqrt(2.0f));

			springDampers.push_back(springDamper);
		}
	}

	// 更新法向量和加速度
	updateNormal();
	updateAcceleration();

	// 计算每个粒子的法向量
	for (auto particle : particles) {
		normals.push_back(particle->normal);
	}

	// 生成VAO（顶点数组对象）和VBO（顶点缓冲对象）
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// 绑定VAO
	glBindVertexArray(VAO);

	// 绑定VBO - 用于存储顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// 绑定VBO - 用于存储法向量
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// 生成EBO（元素索引缓冲区），并绑定
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// 解绑VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Cloth::~Cloth() {
	// 删除VBO和VAO
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	// 删除粒子、三角形和弹簧阻尼器
	for (auto particle : particles) {
		delete particle;
	}

	for (auto triangle : triangles) {
		delete triangle;
	}

	for (auto springDamper : springDampers) {
		delete springDamper;
	}
}

void Cloth::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	// 激活着色器程序
	glUseProgram(shader);

	// 获取uniform变量的位置并传递给着色器
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// 绑定VAO
	glBindVertexArray(VAO);

	// 使用EBO绘制三角形
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// 解绑VAO和着色器程序
	glBindVertexArray(0);
	glUseProgram(0);
}

void Cloth::update(float step) {
	// 更新每个粒子的状态
	for (auto particle : particles) {
		particle->move(step);  // 移动粒子

		glm::vec3 pos = glm::vec3(model * glm::vec4(particle->position, 1));

		// 碰撞检测与反弹处理
		if (!particle->fixed && land->collide(pos)) {
			pos[1] = land->top() + 0.01f;
			//如果只改速度，不改位置,粒子已经在地面下面,下一帧还会继续穿透,最终还是抖动
			particle->position = glm::vec3(glm::inverse(model) * glm::vec4(pos, 1));

			//速度转换到世界空间，因为碰撞法线在世界空间，所以速度投影必须同空间，w=0是因为速度是方向向量，不受平移影响
			glm::vec3 v = glm::vec3(model * glm::vec4(particle->velocity, 0));
			//垂直于地面的速度（会反弹）
			glm::vec3 vClose = glm::dot(glm::vec3(0, 1, 0), v) * glm::vec3(0, 1, 0);
			//沿地面的速度（会摩擦）
			glm::vec3 vTangent = v - vClose;

			if (vTangent != glm::vec3(0)) {
				//接触越用力，摩擦越大
				float decreaseRatio = glm::min(0.5f * glm::length(vClose), 1.0f);
				vTangent = (1 - decreaseRatio) * vTangent;
			}
			//然后再把速度一转，回到局部空间
			particle->velocity = glm::vec3(glm::inverse(model) * glm::vec4(-0.1f * vClose + vTangent, 0));
		}
	}

	// 更新法向量和加速度
	updateNormal();
	updateAcceleration();

	// 更新粒子的位置和法向量
	for (size_t i = 0; i < particles.size(); ++i) {
		positions[i] = particles[i]->position;
	}

	for (size_t i = 0; i < particles.size(); ++i) {
		normals[i] = particles[i]->normal;
	}

	// 绑定VAO并更新数据
	glBindVertexArray(VAO);

	// 更新顶点位置VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// 更新法向量VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cloth::blow(glm::vec3 wind) {
	// 计算风力方向
	worldWind = wind;
	localWind = glm::vec3(glm::inverse(model) * glm::vec4(worldWind, 0));
}

void Cloth::translate(glm::vec3 v) {
	// 平移布料
	translation += v;
	model = glm::translate(v) * model;
	localWind = glm::vec3(glm::inverse(model) * glm::vec4(worldWind, 0));
}

void Cloth::rotate(float angle) {
	// 旋转布料
	model = glm::translate(translation) * glm::rotate(angle, glm::vec3(0, 1, 0)) *
		glm::translate(-translation) * model;
	localWind = glm::vec3(glm::inverse(model) * glm::vec4(worldWind, 0));
}

void Cloth::toggleFree() {
	// 切换粒子的固定状态
	for (auto i : fixedIdx) {
		particles[i]->fixed = !particles[i]->fixed;
	}
}

void Cloth::updateNormal() {
	// 更新每个粒子的法向量
	for (auto particle : particles) {
		particle->normal = glm::vec3(0);
	}

	// 更新三角形的法向量
	for (auto triangle : triangles) {
		triangle->updateNormal();
	}

	// 归一化粒子的法向量
	for (auto particle : particles) {
		if (glm::length(particle->normal) > 1e-8f) {
			particle->normal = glm::normalize(particle->normal);
		}
		else {
			particle->normal = glm::vec3(0, 0, 1);
		}
	}
}

void Cloth::updateAcceleration() {
	// 更新每个粒子的加速度
	for (auto particle : particles) {
		particle->clearAcceleration();  // 清除之前的加速度
		particle->applyAcceleration(glm::inverse(model) * glm::vec4(0, -9.8, 0, 0));  // 应用重力加速度
	}

	// 更新弹簧阻尼器的加速度
	for (auto springDamper : springDampers) {
		springDamper->updateAcceleration();
	}

	// 应用风力
	for (auto triangle : triangles) {
		triangle->wind(localWind);
	}
}
