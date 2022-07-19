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
/// 自キャラ
/// </summary>
class Player {
public:
	void Initialize(Model* model, uint32_t textureHandle);
	void Update();
	void Draw(ViewProjection& viewProjection_);
	void Attack();
	void OnCollision();//衝突を検出したら呼び出されるコールバック関数
	//ワールド座標を取得
	Vector3 GetWorldPosition() { return worldTransform_.translation_; }
	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }
private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//入力処理するため
	Input* input_ = nullptr;
	//デバッグテキスト
	DebugText* debugText_ = nullptr;
	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	PlayerBullet* bullet;

};