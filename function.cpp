#include"function.h"


//スケーリング行列関数
Matrix4 MatScaleCreate(Vector3 scale) {
	Matrix4 matScale = MathUtility::Matrix4Identity();
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;

	return matScale;
}

//回転行列関数Z
Matrix4 MatRotZCreate(float rot) {
	Matrix4 matRotZ = MathUtility::Matrix4Identity();

	matRotZ.m[0][0] = cos(rot);
	matRotZ.m[0][1] = sin(rot);
	matRotZ.m[1][0] = -sin(rot);
	matRotZ.m[1][1] = cos(rot);

	return matRotZ;
}

//回転行列関数X
Matrix4 MatRotXCreate(float rot) {
	Matrix4 matRotX = MathUtility::Matrix4Identity();

	matRotX.m[1][1] = cos(rot);
	matRotX.m[1][2] = sin(rot);
	matRotX.m[2][1] = -sin(rot);
	matRotX.m[2][2] = cos(rot);

	return matRotX;
}

//回転行列関数Y
Matrix4 MatRotYCreate(float rot) {
	Matrix4 matRotY = MathUtility::Matrix4Identity();

	matRotY.m[0][0] = cos(rot);
	matRotY.m[0][2] = -sin(rot);
	matRotY.m[2][0] = sin(rot);
	matRotY.m[2][2] = cos(rot);

	return matRotY;
}

//回転行列合成行列関数
Matrix4 MatRotSCreate(Vector3 rot) {
	Matrix4 matRot = MathUtility::Matrix4Identity();

	matRot *= MatRotZCreate(rot.z);
	matRot *= MatRotYCreate(rot.x);
	matRot *= MatRotYCreate(rot.y);

	return matRot;
}

//平行移動行列関数
Matrix4 MatTransCreate(Vector3 trans) {
	Matrix4 matTrans = MathUtility::Matrix4Identity();

	matTrans.m[3][0] = trans.x;
	matTrans.m[3][1] = trans.y;
	matTrans.m[3][2] = trans.z;

	return matTrans;
}

///ワールド行列関数
Matrix4 MatWorldCreate(Vector3 scale, Vector3 rot, Vector3 trans) {
	Matrix4 matWorld = MathUtility::Matrix4Identity();

	matWorld *= MatScaleCreate(scale);
	matWorld *= MatRotSCreate(rot);
	matWorld *= MatTransCreate(trans);

	return matWorld;
}