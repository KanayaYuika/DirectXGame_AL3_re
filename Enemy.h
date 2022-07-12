#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "DebugText.h"
#include "function.h"
#include "EnemyBullet.h"
#include <cassert>
#include<memory>
#include<list>
#include<cmath>
#include"Vector3.h"

//Vector3& Vector3::normalize() {
//	float len = (x * x) + (y * y) + (z * z);
//	if (len != 0) {
//		return *this /= len;
//	}
//	return *this;
//}

class Player;
class Enemy
{
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(ViewProjection& viewProjection_);
	void Fire();
	//���ˊԊu�p�ϐ�
	static const int kFireInterval = 60;
	//�ڋ߃t�F�[�Y������
	void APInitialization();
	void SetPlayer(Player* player) { player_ = player; }
	//���[���h���W���擾
	Vector3 GetWorldPosition();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;
	//���͏������邽��
	Input* input_ = nullptr;
	//�f�o�b�O�e�L�X�g
	DebugText* debugText_ = nullptr;
	//�s���t�F�[�Y
	enum class Phase {
		Approach,//�ڋ߂���
		Leave,//���E����
	};
	//�t�F�[�Y
	Phase phase_ = Phase::Approach;
	//�e
	std::list<std::unique_ptr<EnemyBullet>> bullets_;
	//���˃^�C�}�[
	int32_t fireIntervalTimer = 0;
	//���L����
	Player* player_ = nullptr;
	Vector3* vector3;

};

