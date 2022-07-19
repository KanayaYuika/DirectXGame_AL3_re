#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "DebugText.h"
#include "function.h"
#include "EnemyBullet.h"
#include <cassert>
#include<memory>
#include<list>
#include<cmath>
#include"Vector3.h"

//Vector3& Vector3::normalize() {
//	float len = (x * x) + (y * y) + (z * z);
//	if (len != 0) {
//		return *this /= len;
//	}
//	return *this;
//}

class Player;
class Enemy
{
public:
	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw(ViewProjection& viewProjection_);
	void Fire();
	void OnCollision();//衝突を検出したら呼び出されるコールバック関数
	//発射間隔用変数
	static const int kFireInterval = 60;
	//接近フェーズ初期化
	void APInitialization();
	void SetPlayer(Player* player) { player_ = player; }
	//ワールド座標を取得
	Vector3 GetWorldPosition();
	//弾リストを取得
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets_; }

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
	//行動フェーズ
	enum class Phase {
		Approach,//接近する
		Leave,//離脱する
	};
	//フェーズ
	Phase phase_ = Phase::Approach;
	//弾
	std::list<std::unique_ptr<EnemyBullet>> bullets_;
	//発射タイマー
	int32_t fireIntervalTimer = 0;
	//自キャラ
	Player* player_ = nullptr;
	Vector3* vector3 = nullptr;

};

