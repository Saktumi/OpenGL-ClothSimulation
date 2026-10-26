#include "Particle.h"

// 构造函数，初始化粒子的位置、速度、质量等属性
Particle::Particle(glm::vec3 pos, glm::vec3 v, float m) : mass(m), fixed(false),
normal(glm::vec3(0)), position(pos), velocity(v), acceleration(glm::vec3(0)) {
}

// 析构函数
Particle::~Particle() {}

// 移动粒子，更新粒子的速度和位置
void Particle::move(float step) {
    // 如果粒子不是固定的，才进行运动更新
    if (!fixed) {
        // 根据加速度更新速度
        velocity = velocity + step * acceleration;
        // 根据速度更新位置
        position = position + step * velocity;
    }
}

// 清除粒子的加速度（将加速度设为零）
void Particle::clearAcceleration() {
    acceleration = glm::vec3(0);  // 重置加速度
}

// 施加外力给粒子，计算该外力对粒子的加速度影响
void Particle::applyForce(glm::vec3 force) {
    // 外力除以质量，得到加速度，并累加到当前加速度
    acceleration += force / mass;
}

// 直接施加加速度给粒子，忽略外力的计算
void Particle::applyAcceleration(glm::vec3 a) {
    acceleration += a;  // 累加额外的加速度
}

// 将粒子设置为固定状态，使其不能移动
void Particle::setFix() {
    fixed = true;  // 设置为固定
}

// 将粒子设置为自由状态，使其可以移动
void Particle::setFree() {
    fixed = false;  // 设置为自由
}
