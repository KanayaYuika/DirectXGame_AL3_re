#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include"PrimitiveDrawer.h"
#include<random>

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; delete debugCamera_; }

float PI = 3.141592;

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

	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();

		worldTransform.scale_ = { 1.0f,1.0f,1.0f };//X,Y,Z方向のスケーリングを設定
		worldTransform.rotation_ = { rotDist(engine),rotDist(engine),rotDist(engine) };//X,Y,Z軸周りの回転角を設定
		worldTransform.translation_ = { posDist(engine),posDist(engine),posDist(engine) };//X,Y,Z軸周りの平行移動を設定
		//スケーリング行列を宣言
		Matrix4 matScale;
		matScale = MathUtility::Matrix4Identity();//スケーリング倍率を行列に設定する
		matScale.m[0][0] = worldTransform.scale_.x;
		matScale.m[1][1] = worldTransform.scale_.y;
		matScale.m[2][2] = worldTransform.scale_.z;

		//合成用回転行列を宣言
		Matrix4 matRot = MathUtility::Matrix4Identity();
		//Z軸回転行列を宣言
		Matrix4 matRotZ = MathUtility::Matrix4Identity();
		Matrix4 matRotX = MathUtility::Matrix4Identity();
		Matrix4 matRotY = MathUtility::Matrix4Identity();

		matRotZ.m[0][0] = cos(worldTransform.rotation_.z);
		matRotZ.m[0][1] = sin(worldTransform.rotation_.z);
		matRotZ.m[1][0] = -sin(worldTransform.rotation_.z);
		matRotZ.m[1][1] = cos(worldTransform.rotation_.z);

		matRotX.m[1][1] = cos(worldTransform.rotation_.x);
		matRotX.m[1][2] = sin(worldTransform.rotation_.x);
		matRotX.m[2][1] = -sin(worldTransform.rotation_.x);
		matRotX.m[2][2] = cos(worldTransform.rotation_.x);

		matRotY.m[0][0] = cos(worldTransform.rotation_.y);
		matRotY.m[0][2] = -sin(worldTransform.rotation_.y);
		matRotY.m[2][0] = sin(worldTransform.rotation_.y);
		matRotY.m[2][2] = cos(worldTransform.rotation_.y);

		matRot *= matRotZ;
		matRot *= matRotX;
		matRot *= matRotY;

		//平行移動行列を宣言
		Matrix4 matTrans = MathUtility::Matrix4Identity();

		matTrans.m[3][0] = worldTransform.translation_.x;
		matTrans.m[3][1] = worldTransform.translation_.y;
		matTrans.m[3][2] = worldTransform.translation_.z;


		//スケーリング・回転・平行移動を合成した行列を計算
		worldTransform.matWorld_ = MathUtility::Matrix4Identity();
		worldTransform.matWorld_ *= matScale;
		worldTransform.matWorld_ *= matRot;
		worldTransform.matWorld_ *= matTrans;

		//行列の転送
		worldTransform.TransferMatrix();
	}
	//カメラ視点座標を設定
	viewProjection_.eye = { 0,0,200 };
	viewProjection_.up = { cosf(3.14 / 4.0f),sinf(3.14 / 4.0f),0.0f };
	//カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = 10.0f * 3.14 / 180;
	//アスペクト比を設定
	viewProjection_.aspectRatio = 1.0f;
	//ニアクリップ距離を設定
	viewProjection_.nearZ = 200.0f;
	//ファークリップ距離を設定
	viewProjection_.farZ = 250.0f;


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

	//FoV変更処理
	{
		//Wキーで視野角が広がる
		if (input_->PushKey(DIK_W)) {
			viewProjection_.fovAngleY += 0.01f;
			viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, 3.14);
		}//Sキーで視野角が広がる
		else if (input_->PushKey(DIK_S)) {
			viewProjection_.fovAngleY -= 0.01f;
			viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
		}
		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバッグ再表示
		debugText_->SetPos(50, 110);
		debugText_->Printf("fovAngleY(Degree):%f", viewProjection_.fovAngleY / 3.14);
	}
	//クリップ変更処理
	{
		//上下キーでニアクリップ距離を増減
		if (input_->PushKey(DIK_UP)) {
			viewProjection_.nearZ += 0.1f;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			viewProjection_.nearZ -= 0.1f;
		}
		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバッグ再表示
		debugText_->SetPos(50, 130);
		debugText_->Printf("nearZ:(%f)", viewProjection_.nearZ);
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

	//範囲forで全てのワールドトランスフォームを順に処理する//複数
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, viewProjection_, textureHandle_);
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
