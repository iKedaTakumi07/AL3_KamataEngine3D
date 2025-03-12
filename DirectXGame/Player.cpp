#include "Player.h"

void Player::Initialize(Model* model, uint32_t textureHandle, Camera camera) {
	// nullポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandel_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_.translation_ = camera.translation_;
	
}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, camera_,textureHandel_);
}
