#include "Enemy.h"

void Enemy::Initialize(Model* model, const Vector3& position) {
	//NULL�|�C���^�`�F�b�N
	assert(model);

	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	model_ = model;

	textureHandle_ = TextureManager::Load("enemy.png");

	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();


	//�G�����ʒu
	worldTransform_.translation_ = position;
}

void Enemy::Update() {

	const float kEnemySpeed = 0.1f;
	Vector3 velocity(0, 0, -kEnemySpeed);

	//Fire();
	//���˃^�C�}�[�J�E���g�_�E��
	fireIntervalTimer--;
	//�w�莞�ԂɒB����
	if (fireIntervalTimer <= 0) {
		//�e�𔭎�
		Fire();
		// �ڋ߃t�F�[�Y������
		APInitialization();
	}
	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});

	//�e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Update();
	}

	switch (phase_) {
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

	//�e�`��
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}

void Enemy::Fire() {
	//�e�̑��x
	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
	velocity = VectorMatrix(velocity, worldTransform_);

	//�e�𐶐����A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	//�e��o�^����
	bullets_.push_back(std::move(newBullet));
}

void Enemy::APInitialization() {
	//���˃^�C�}�[��������
	fireIntervalTimer = kFireInterval;

}

