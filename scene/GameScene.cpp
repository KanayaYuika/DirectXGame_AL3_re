#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include"PrimitiveDrawer.h"
#include<random>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_; delete debugCamera_; delete player_; delete enemy_;
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
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	//自キャラの生成
	player_ = new Player();
	//自キャラの更新
	player_->Initialize(model_, textureHandle_);
	//敵キャラの生成
	enemy_ = new Enemy();
	Vector3 EnemyPosition = { 10,0,50 };
	//敵キャラの更新
	enemy_->Initialize(model_, EnemyPosition);
	//敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);


}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = true;
	}
#endif
	//カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}
	else {
		viewProjection_.TransferMatrix();
	}

	player_->Update();
	//ポインタがnullでない時だけ行う
	enemy_->Update();

	CheckAllCollisions();

}

void GameScene::CheckAllCollisions() {
	//判定対象AとBの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();

	//敵弾リストの取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();

#pragma regin //----------自キャラと敵弾の当たり判定

	//自キャラの座標
	posA= player_->GetWorldPosition();
	//自キャラと敵弾全ての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//敵弾の座標
		posB = bullet->GetWorldPosition();
		float hanA = 2.0f;
		float hanB = 2.0f;
		//座標AとBの距離を求める
		float X = posA.x - posB.x;
		float Y = posA.y - posB.y;
		float Z = posA.z - posB.z;
		float H = hanA + hanB;
		//球と球の交差判定
		if (X * X + Y * Y + Z * Z < H * H) {
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			//敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}

	}
#pragma endregion 

#pragma regin //----------敵キャラと自弾の当たり判定

	//敵キャラの座標
	posA = enemy_->GetWorldPosition();
	//敵キャラと自弾全ての当たり判定
	for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		//自弾の座標
		posB = bullet->GetWorldPosition();
		float hanA = 2.0f;
		float hanB = 2.0f;
		//座標AとBの距離を求める
		float X = posA.x - posB.x;
		float Y = posA.y - posB.y;
		float Z = posA.z - posB.z;
		float H = hanA + hanB;
		//球と球の交差判定
		if (X * X + Y * Y + Z * Z < H * H) {
			//敵キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision();
			//自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}

	}
#pragma endregion


#pragma regin //-----------自弾と敵弾の当たり判定

	//敵弾と自弾全ての当たり判定
	for (const std::unique_ptr<PlayerBullet>& playerBullet_ : playerBullets) {
		for (const std::unique_ptr<EnemyBullet>& enemyBullet_ : enemyBullets) {
			//自弾の座標
			posA = playerBullet_->GetWorldPosition();
			//敵弾の座標
			posA = enemyBullet_->GetWorldPosition();
			float hanA = 2.0f;
			float hanB = 2.0f;
			//座標AとBの距離を求める
			float X = posA.x - posB.x;
			float Y = posA.y - posB.y;
			float Z = posA.z - posB.z;
			float H = hanA + hanB;
			//球と球の交差判定
			if (X * X + Y * Y + Z * Z < H * H) {
				//自弾の衝突時コールバックを呼び出す
				playerBullet_->OnCollision();
				//敵弾の衝突時コールバックを呼び出す
				enemyBullet_ ->OnCollision();
			}

		}
	}
#pragma endregion


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
	/// 自キャラの描画
	player_->Draw(viewProjection_);
	/// 敵キャラの描画
	enemy_->Draw(viewProjection_);

	//範囲forで全てのワールドトランスフォームを順に処理する


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
