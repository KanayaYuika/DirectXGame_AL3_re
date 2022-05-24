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
	textureHandle_ = TextureManager:: Load("mario.jpg");
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

		worldTransform.scale_ = {1.0f,1.0f,1.0f};//X,Y,Z方向のスケーリングを設定
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
	viewProjection_.eye = { 0,0,-10 };
	viewProjection_.up = { cosf(3.14 / 4.0f),sinf(3.14 / 4.0f),0.0f };

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	

	/*PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	for (int i = 0; i < 30; i++) {
		if (i < 15) {
			iti[i] = Vector3(-14+i*2, 0, -14);
			ni[i] = Vector3(-14+i*2, 0, 14);
		}
		else {
			iti[i] = Vector3(-14, 0, -14+(i-15)*2);
			ni[i] = Vector3(14, 0, -14+(i-15)*2);
		}
	}*/


	{//単体用
		////X,Y,Z方向のスケーリングを設定
		//worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
		////スケーリング行列を宣言
		//Matrix4 matScale;
		//matScale = MathUtility::Matrix4Identity();//スケーリング倍率を行列に設定する
		//matScale.m[0][0] = worldTransform_.scale_.x;
		//matScale.m[1][1] = worldTransform_.scale_.y;
		//matScale.m[2][2] = worldTransform_.scale_.z;

		////X,Y,Z軸周りの回転角を設定
		//worldTransform_.rotation_ = { PI / 4,PI / 4,0 };
		////合成用回転行列を宣言
		//Matrix4 matRot = MathUtility::Matrix4Identity();
		////Z軸回転行列を宣言
		//Matrix4 matRotZ = MathUtility::Matrix4Identity();
		//Matrix4 matRotX = MathUtility::Matrix4Identity();
		//Matrix4 matRotY = MathUtility::Matrix4Identity();

		//matRotZ.m[0][0] = cos(worldTransform_.rotation_.z);
		//matRotZ.m[0][1] = sin(worldTransform_.rotation_.z);
		//matRotZ.m[1][0] = -sin(worldTransform_.rotation_.z);
		//matRotZ.m[1][1] = cos(worldTransform_.rotation_.z);

		//matRotX.m[1][1] = cos(worldTransform_.rotation_.x);
		//matRotX.m[1][2] = sin(worldTransform_.rotation_.x);
		//matRotX.m[2][1] = -sin(worldTransform_.rotation_.x);
		//matRotX.m[2][2] = cos(worldTransform_.rotation_.x);

		//matRotY.m[0][0] = cos(worldTransform_.rotation_.y);
		//matRotY.m[0][2] = -sin(worldTransform_.rotation_.y);
		//matRotY.m[2][0] = sin(worldTransform_.rotation_.y);
		//matRotY.m[2][2] = cos(worldTransform_.rotation_.y);

		//matRot *= matRotZ;
		//matRot *= matRotX;
		//matRot *= matRotY;


		////X,Y,Z軸周りの平行移動を設定
		//worldTransform_.translation_ = { 10,10,10 };
		////平行移動行列を宣言
		//Matrix4 matTrans = MathUtility::Matrix4Identity();

		//matTrans.m[3][0] = worldTransform_.translation_.x;
		//matTrans.m[3][1] = worldTransform_.translation_.y;
		//matTrans.m[3][2] = worldTransform_.translation_.z;


		//worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
		//worldTransform_.matWorld_ *= matScale;
		//worldTransform_.matWorld_ *= matRot;
		//worldTransform_.matWorld_ *= matTrans;

		////行列の転送
		//worldTransform_.TransferMatrix();
	}

}

void GameScene::Update() {
	debugCamera_->Update();

	//-----カメラ視点移動処理-----//
	{
		//視点の移動ベクトル
		Vector3 move = { 0, 0, 0 };
		//視点の移動速さ
		const float kEyeSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_W)) {
			move = { 0, 0, kEyeSpeed };
		}
		else if (input_->PushKey(DIK_S)) {
			move = { 0, 0, -kEyeSpeed };
		}

		//視点移動（ベクトルの加算）
		viewProjection_.eye.x += move.x;
		viewProjection_.eye.y += move.y;
		viewProjection_.eye.z += move.z;

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバッグ再表示
		debugText_->SetPos(50, 50);
		debugText_->Printf(
			"eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	}

	//-----カメラ注視点移動処理-----//
	{
		//注視点の移動ベクトル
		Vector3 move = { 0, 0, 0 };
		//注視点の移動速さ
		const float kTargetSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = { -kTargetSpeed,0, 0 };
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move = { kTargetSpeed,0, 0 };
		}

		//注視点移動（ベクトルの加算）
		viewProjection_.target.x += move.x;
		viewProjection_.target.y += move.y;
		viewProjection_.target.z += move.z;

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバッグ再表示
		debugText_->SetPos(50, 70);
		debugText_->Printf(
			"target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
			viewProjection_.target.z);
	}

	//上方向回転処理
	{
		//上方向の回転速さ[ラジアン/frame]
		const float kUpRotSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_SPACE)) {
			viewAngle += kUpRotSpeed;
			//2πを超えたら0に戻す
			viewAngle = fmodf(viewAngle, 3.14 * 2.0f);
		}
		//上方向ベクトルを計算（半径1の円周上の座標）
		viewProjection_.up = { cosf(viewAngle), sinf(viewAngle), 0.0f };

		//行列の再計算
		viewProjection_.UpdateMatrix();
		//デバッグ再表示
		debugText_->SetPos(50, 90);
		debugText_->Printf(
			"up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
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
	/// 単体
	//model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

	//範囲forで全てのワールドトランスフォームを順に処理する//複数
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, viewProjection_, textureHandle_);
	}

	//ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	/*for (int i = 0; i < 30; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(iti[i], ni[i], {0,0,250,1});
	}*/

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
