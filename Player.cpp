#include"Player.h"

void Player::Initialize(Model* model, uint32_t textureHandle) {
	//NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();
}

void Player::Update() {

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= 0.1f;
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		move.x += 0.1f;
	}
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= 0.1f;
	}
	else if (input_->PushKey(DIK_UP)) {
		move.y += 0.1f;
	}

	//回転速さ[ラジアン/frame]
	const float kChestRotSpeed = 0.05f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_U)) {
		worldTransform_.rotation_.y -= kChestRotSpeed;
	}
	else if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.y += kChestRotSpeed;
	}

	//キャラクター攻撃処理
	Attack();

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	//移動限界座標
	const float kMoveLimitX = 32;
	const float kMoveLimitY = 16;

	//範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	worldTransform_.translation_ += move;

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= MatRotSCreate(worldTransform_.rotation_);
	worldTransform_.matWorld_ *= MatTransCreate(worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"translation:(%f,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
}

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}

}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = VectorMatrix(velocity, worldTransform_);

		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_,velocity);

		//弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}
//
//Vector3 Player::GetWorldPosition() {
//	//ワールド座標を入れる変数
//	Vector3 worldPos;
//	//ワールド行列の平行移動成分を取得（ワールド座標）
//	worldPos.x = worldTransform_.translation_.x;
//	worldPos.y = worldTransform_.translation_.y;
//	worldPos.z = worldTransform_.translation_.z;
//
//	return worldPos;
//}