#include "Player.h"

void Player::Initialize(Model* model, uint32_t textureHandle) {
	// nullポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandel_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.translation_ = Vector3(0.0f, 0.0f, -5.0f);
	worldTransform_.Initialize();
}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {}
