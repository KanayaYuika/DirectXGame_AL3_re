#include "PlayerBullet.h"

void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	//NULLポインタチェック
	assert(model);
	model_ = model;
	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	//引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	worldTransform_.matWorld_ = MatWorldCreate(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
}
void PlayerBullet::Update() {

	worldTransform_.TransferMatrix();
}
void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}