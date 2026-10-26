#include "Triangle.h"

// 构造函数，初始化三角形的三个粒子，拖曳系数和流体密度，并更新法线
Triangle::Triangle(Particle* p1, Particle* p2, Particle* p3) :
    p1(p1), p2(p2), p3(p3), dragCo(1.28f), fluidDensity(1.225f) {
    updateNormal();
}

// 析构函数
Triangle::~Triangle() {}

// 更新三角形的法线
void Triangle::updateNormal() {
    // 计算两个边：从 p1 到 p2 和从 p1 到 p3
    glm::vec3 side1 = p2->position - p1->position;
    glm::vec3 side2 = p3->position - p1->position;
    glm::vec3 crossProduct = glm::cross(side1, side2);
    if (glm::length(crossProduct) <= 1e-8f) return;
    // 通过叉乘计算法线向量，并归一化
    glm::vec3 normal = glm::normalize(crossProduct);

    // 将计算得到的法线添加到每个粒子的法线中
    p1->normal += normal;
    p2->normal += normal;
    p3->normal += normal;
}

// 计算风力对三角形的作用
void Triangle::wind(glm::vec3 vWind) {
    // 计算三角形的两个边：从 p1 到 p2 和从 p1 到 p3
    glm::vec3 side1 = p2->position - p1->position;
    glm::vec3 side2 = p3->position - p1->position;
    // 计算三角形的面积，使用叉乘来得到平行四边形的面积，再除以 2 得到三角形面积
    float area = glm::length(glm::cross(side1, side2)) / 2.0f;

    // 计算三角形的法线
    glm::vec3 normal;
    if (area <= 1e-8f) {
        normal = glm::vec3(0, 1, 0);  
        return;
    }
    else {
        normal = glm::normalize(glm::cross(side1, side2));  // 归一化叉乘
    }

    // 计算三角形的平均速度
    glm::vec3 vTriangle = (p1->velocity + p2->velocity + p3->velocity) / 3.0f;
    // 计算相对风速
    glm::vec3 vClose = vTriangle - vWind;

    // 计算与风速方向垂直的面积分量
    float crossArea;
    if (glm::length(vClose) <= 1e-8f) {
        crossArea = 0;  // 如果风速为 0，则没有风力作用
    }
    else {
        // 使用风速与法线的点积来计算垂直于风速的面积分量
        crossArea = area * glm::dot(vClose, normal) / glm::length(vClose);
    }

    // 计算风力作用于三角形的力，公式为：拖曳力 = -0.5 * 流体密度 * 拖曳系数 * 风速平方 * 面积 * 法线
    glm::vec3 force = -fluidDensity * dragCo * glm::dot(vClose, vClose) * crossArea * normal / 2.0f;

    // 将风力分配到每个粒子上，假设风力均匀作用于三个粒子
    glm::vec3 forceOnParticle = force / 3.0f;

    // 对每个粒子施加计算得到的风力
    p1->applyForce(forceOnParticle);
    p2->applyForce(forceOnParticle);
    p3->applyForce(forceOnParticle);
}
