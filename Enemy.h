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
class Enemy
{
public:
	void Initialize(Model* model,const Vector3 &position );
	void Update();
	void Draw(ViewProjection& viewProjection_);
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
};

