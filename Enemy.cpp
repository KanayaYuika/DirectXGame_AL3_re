#include "Enemy.h"

void Enemy::Initialize(Model* model,const Vector3 &position) {
	//NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	textureHandle_= TextureManager::Load("enemy.png");

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();

	//敵初期位置
	worldTransform_.translation_ = position;
}

void Enemy::Update() {

	const float kEnemySpeed = 0.2f;
	Vector3 velocity(0, 0, -kEnemySpeed); 

	switch(phase_) {
	case Phase::Approach:
	default:
		//移動（ベクトルを加算）
		worldTransform_.translation_ += velocity;
		//既定の位置に到達したら離脱
		if (worldTransform_.translation_.z < 0.0f) {
			phase_ = Phase::Leave;
		}
		break;
	}

	worldTransform_.translation_ += velocity;

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= MatTransCreate(worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
}

