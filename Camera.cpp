
#include "Camera.h"

#include "glm/gtx/euler_angles.hpp"

Camera::Camera() {
    Reset(); //初始化摄像机参数
}

void Camera::Update() {
    glm::mat4 world(1); //创建单位矩阵
    world[3][2] = Distance; //把相机置于z正方向
    world =
        glm::eulerAngleY(glm::radians(-Azimuth)) *
        glm::eulerAngleX(glm::radians(-Incline)) *
        world;  //根据方位角倾斜相机
    glm::mat4 view = glm::inverse(world); //求逆得到视图矩阵
    glm::mat4 project = glm::perspective(
            glm::radians(FOV),
            Aspect,
            NearClip,
            FarClip
        ); //构建透视投影矩阵
    ViewProjectMtx = project * view; //合并视图与投影矩阵
}

void Camera::Reset() {
    // 初始化摄像机的各项参数
    FOV = 45.0f;          // 视野角度 (Field of View)
    Aspect = 1.33f;       // 视口的宽高比 (Aspect Ratio)
    NearClip = 0.1f;      // 近平面距离
    FarClip = 100.0f;     // 远平面距离

    Distance = 10.0f;     // 摄像机距离
    Azimuth = 0.0f;       // 方位角 (水平旋转角度)
    Incline = 20.0f;      // 倾斜角 (垂直旋转角度)
}