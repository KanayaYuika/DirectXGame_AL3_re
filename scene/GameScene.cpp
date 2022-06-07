#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include"PrimitiveDrawer.h"
#include<random>

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; delete debugCamera_; }

float PI = 3.141592;

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

//ワールド行列関数
Matrix4 MatWorldCreate(Vector3 scale,Vector3 rot,Vector3 trans) {
	Matrix4 matWorld = MathUtility::Matrix4Identity();

	matWorld *= MatScaleCreate(scale);
	matWorld *= MatRotSCreate(rot);
	matWorld *= MatTransCreate(trans);

	return matWorld;
}



void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	//3Dモデルの生成
	model_ = Model::Create(); //ワールドトランスフォームの初期化

	//乱数シードの生成器
	std::random_device seed_gen;
	//メルセンヌ・ついうたーの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲の指定
	std::uniform_real_distribution<float> posDist(-10.0, 10.0);//平行移動//最小値、最大値

	std::uniform_real_distribution<float> rotDist(0.0, 3.14);//回転//最小値、最大値

	//キャラクターの大元
	worldTransforms_[PartId::kRoot].Initialize();
	//脊椎
	worldTransforms_[PartId::kSpine].Initialize();
	worldTransforms_[PartId::kSpine].translation_ = { 0,0,0 };
	worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];

	//上半身
	worldTransforms_[PartId::kChest].Initialize();
	worldTransforms_[PartId::kChest].translation_ = { 0, 4.0f, 0 };
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];

	worldTransforms_[PartId::kHead].translation_ = { 0, 4.0f, 0 };
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kHead].Initialize();

	worldTransforms_[PartId::kArmL].translation_ = { -4.0f, 0, 0 };
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmL].Initialize();

	worldTransforms_[PartId::kArmR].translation_ = { 4.0f, 0, 0 };
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmR].Initialize();

	//下半身
	worldTransforms_[PartId::kHip].Initialize();
	worldTransforms_[PartId::kHip].translation_ = { 0, 0, 0 };
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];

	worldTransforms_[PartId::kLegL].translation_ = { 3.5f, -3.0f, 0 };
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegL].Initialize();

	worldTransforms_[PartId::kLegR].translation_ = { -3.5f, -3.0f, 0 };
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegR].Initialize();


	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

}

void GameScene::Update() {
	debugCamera_->Update();

	//----- キャラクター移動処理 -----//
	{
		//キャラクターの移動ベクトル
		Vector3 move = { 0, 0, 0 };

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move.x -= 0.1f;
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move.x += 0.1f;
		}

		worldTransforms_[kRoot].translation_ += move;
		worldTransforms_[kRoot].matWorld_ = MatWorldCreate(worldTransforms_[kRoot].scale_, worldTransforms_[kRoot].rotation_, worldTransforms_[kRoot].translation_);
		worldTransforms_[kRoot].TransferMatrix();
		debugText_->SetPos(50, 90);
		debugText_->Printf("translation:%f", worldTransforms_[kRoot].translation_.x);

	}
	//上半身回転処理
	{
		//上半身の回転速さ[ラジアン/frame]
		const float kChestRotSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_U)) {
			worldTransforms_[PartId::kChest].rotation_.y -= kChestRotSpeed;
		}
		else if (input_->PushKey(DIK_I)) {
			worldTransforms_[PartId::kChest].rotation_.y += kChestRotSpeed;
		}
	}

	//下半身回転処理
	{
		//下半身の回転速さ[ラジアン/frame]
		const float kChestRotSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_J)) {
			worldTransforms_[PartId::kHip].rotation_.y -= kChestRotSpeed;
		}
		else if (input_->PushKey(DIK_K)) {
			worldTransforms_[PartId::kHip].rotation_.y += kChestRotSpeed;
		}
	}
	//子の更新
	//{
	//	worldTransforms_[kSpine].matWorld_ = MatWorldCreate(worldTransforms_[kSpine].scale_, worldTransforms_[kSpine].rotation_, worldTransforms_[kSpine].translation_);
	//	worldTransforms_[kSpine].matWorld_ *= worldTransforms_[kRoot].matWorld_;
	//	worldTransforms_[kSpine].TransferMatrix();
	//}

	//大元から順に更新していく
	for (int i = 0; i < kNumPartId; i++) {
		worldTransforms_[i].matWorld_= MatWorldCreate(worldTransforms_[i].scale_, worldTransforms_[i].rotation_, worldTransforms_[i].translation_);
		if (worldTransforms_[i].parent_ != nullptr) {
		worldTransforms_[i].matWorld_ *= worldTransforms_[i].parent_->matWorld_;
		}
		worldTransforms_[i].TransferMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/// 3Dモデル描画

	//範囲forで全てのワールドトランスフォームを順に処理する
	//大元から順に描画していく
	for (int i = 0; i < kNumPartId; i++) {
		if (i >= 2) {
			model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
		}
	}


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
