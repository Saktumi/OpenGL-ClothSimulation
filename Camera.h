#pragma once
#include "main.h"

class Camera {
public:
	Camera();

	void Update();
	void Reset();

	void SetAspect(float a) { Aspect = a; } //宽高比
	void SetDistance(float d) { Distance = d; } //眼睛到原点的距离
	void SetAzimuth(float a) { Azimuth = a; }//绕y轴的水平旋转角 左右环绕
	void SetIncline(float i) { Incline = i; }//设置上下仰角 上下环绕

	float GetDistance() { return Distance; }
	float GetAzimuth() { return Azimuth; }
	float GetIncline() { return Incline; }

	const glm::mat4& GetViewProjectMtx() { return ViewProjectMtx; }

private:
	// 透视投影参数
	float FOV;			 // 视场角（单位：度）
	float Aspect;		 // 宽高比
	float NearClip;		 // 近裁剪面距离
	float FarClip;		// 远裁剪面距离

	// 极坐标（球坐标）参数
	float Distance;		 // 相机眼睛位置到原点的距离（单位：米）
	float Azimuth;		 // 相机绕 Y 轴的旋转角（单位：度）
	float Incline;		// 相机在 XZ 平面之上的仰角（单位：度）

	// 视图投影矩阵
	glm::mat4 ViewProjectMtx;
};