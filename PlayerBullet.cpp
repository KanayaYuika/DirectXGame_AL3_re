#include "PlayerBullet.h"

void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	//NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	//�e�N�X�`���ǂݍ���
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	//�����Ŏ󂯎�����������W���Z�b�g
	worldTransform_.translation_ = position;
	worldTransform_.matWorld_ = MatWorldCreate(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
}
void PlayerBullet::Update() {

	worldTransform_.TransferMatrix();
}
void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}