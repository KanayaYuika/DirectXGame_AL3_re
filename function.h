#pragma once

#include"Matrix4.h"
#include"AxisIndicator.h"
#include"WorldTransform.h"

#define PI = 3.141592;
Matrix4 MatScaleCreate(Vector3 scale);
Matrix4 MatRotZCreate(float rot);
Matrix4 MatRotXCreate(float rot);
Matrix4 MatRotYCreate(float rot);
Matrix4 MatRotSCreate(Vector3 rot);
Matrix4 MatTransCreate(Vector3 trans);
Matrix4 MatWorldCreate(Vector3 scale, Vector3 rot, Vector3 trans);