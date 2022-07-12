#include "Enemy.h"
#include "Player.h"

void Enemy::Initialize(Model* model, const Vector3& position) {
	//NULLポインタチェック
	assert(model);
	
	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	textureHandle_ = TextureManager::Load("enemy.png");

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();


	//敵初期位置
	worldTransform_.translation_ = position;
}

void Enemy::Update() {

	const float kEnemySpeed = 0.03f;
	Vector3 velocity(0, 0, -kEnemySpeed);

	//Fire();
	//発射タイマーカウントダウン
	fireIntervalTimer--;
	//指定時間に達した
	if (fireIntervalTimer <= 0) {
		//弾を発射
		Fire();
		// 接近フェーズ初期化
		APInitialization();
	}
	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});

	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Update();
	}

	switch (phase_) {
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

	//弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}

void Enemy::Fire() {
	//assert(player_);
	//弾の速度
	const float kBulletSpeed = 5.0f;

	////自キャラのワールド座標を取得する
	Vector3 playerTransform = player_->GetWorldPosition();
	////敵キャラのワールド座標を取得
	Vector3 enemyTransform = this->GetWorldPosition();
	//敵キャラ→自キャラの差分ベクトルを求める
	Vector3 velocity = playerTransform;
	velocity -= enemyTransform;
	//ベクトルの正規化
	velocity.normalize();
	//Normalize(velocity);
	//ベクトルの長さを、早さに合わせる
	velocity *= kBulletSpeed;

	//弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	//弾を登録する
	bullets_.push_back(std::move(newBullet));
}

void Enemy::APInitialization() {
	//発射タイマーを初期化
	fireIntervalTimer = kFireInterval;

}


Vector3 Enemy::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

