#include "SpringDamper.h"


SpringDamper::SpringDamper(Particle* p1, Particle* p2, float restLength) :
    ks(2000), kd(12), p1(p1), p2(p2), restLength(restLength) {
}


SpringDamper::~SpringDamper() {}


void SpringDamper::updateAcceleration() {

    float currLength = glm::length(p2->position - p1->position);

    float deltaX = currLength - restLength;
    glm::vec3 direction;

    if (currLength == 0) {
        direction = glm::vec3(0, 1, 0);
    }
    else {
        // 计算单位向量，表示弹簧的方向
        direction = glm::normalize(p2->position - p1->position);
    }
    if (currLength > 1.2 * restLength) {
        // 计算弹簧中点
        glm::vec3 center = (p2->position + p1->position) / 2.0f;
        if (!p1->fixed) {
            p1->position = center - 0.6f * restLength * direction;
        }
        if (!p2->fixed) {
            p2->position = center + 0.6f * restLength * direction;
        }
    }
    // 如果当前长度小于静止长度的 0.5 倍，则进行过度压缩的处理
    else if (currLength < 0.5 * restLength) {
        // 计算弹簧中点
        glm::vec3 center = (p2->position + p1->position) / 2.0f;
        if (!p1->fixed) {
            p1->position = center - restLength * direction / 4.0f;
        }
        if (!p2->fixed) {
            p2->position = center + restLength * direction / 4.0f;
        }
    }

    glm::vec3 vClose = glm::dot(p2->velocity - p1->velocity, direction) * direction;
    // 计算弹簧力和阻尼力
    glm::vec3 force = ks * deltaX * direction + kd * vClose;

    p1->applyForce(force);
    p2->applyForce(-force);
}