#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <cassert>
#include "Input.h"
#include "DebugText.h"
#include "function.h"
#include "PlayerBullet.h"
#include<memory>
#include<list>
/// <summary>
/// ���L����
/// </summary>
class Player {
public:
	void Initialize(Model* model, uint32_t textureHandle);
	void Update();
	void Draw(ViewProjection& viewProjection_);
	void Attack();
	//���[���h���W���擾
	Vector3 GetWorldPosition() { return worldTransform_.translation_; }
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
	//�e
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

};