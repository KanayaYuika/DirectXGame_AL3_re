#include "Enemy.h"

void Enemy::Initialize(Model* model,const Vector3 &position) {
	//NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;

	textureHandle_= TextureManager::Load("enemy.png");

	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();

	//�G�����ʒu
	worldTransform_.translation_ = position;
}

void Enemy::Update() {

	const float kEnemySpeed = 0.2f;
	Vector3 velocity(0, 0, -kEnemySpeed); 

	switch(phase_) {
	case Phase::Approach:
	default:
		//�ړ��i�x�N�g�������Z�j
		worldTransform_.translation_ += velocity;
		//����̈ʒu�ɓ��B�����痣�E
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

