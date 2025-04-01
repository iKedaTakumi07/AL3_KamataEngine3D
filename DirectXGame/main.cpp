#include <KamataEngine.h>
#include <vector>

using namespace KamataEngine;

// アフィン変換行列を高速に生成する
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 num;
	num.m[0][0] = scale.x * 1.0f * cosf(rotate.y) * cosf(rotate.x) * 1.0f;
	num.m[0][1] = 0.0f;
	num.m[0][2] = 0.0f;
	num.m[0][3] = 0.0f;
	num.m[1][0] = 0.0f;
	num.m[1][1] = scale.y * cosf(rotate.x) * 1.0f * cosf(rotate.z) * 1.0f;
	num.m[1][2] = 0.0f;
	num.m[1][3] = 0.0f;
	num.m[2][0] = 0.0f;
	num.m[2][1] = 0.0f;
	num.m[2][2] = scale.z * cosf(rotate.x) * cosf(rotate.y) * 1.0f * 1.0f;
	num.m[2][3] = 0.0f;
	num.m[3][0] = translate.x;
	num.m[3][1] = translate.y;
	num.m[3][2] = translate.z;
	num.m[3][3] = 1.0f;
	return num;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"LE2C_03_イケダ_タクミ_AL3");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 3Dモデルデータ
	Model* model_ = nullptr;

	// 3Dモデルの生成
	model_ = Model::Create();

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// カメラ
	Camera camera;
	camera.translation_ = Vector3(0.0f, 0.0f, -50.0f);
	camera.Initialize();

	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	debugCamera_ = new DebugCamera(1280, 720);

	// 要素数
	const uint32_t kNumBlockHorizontal = 20;
	const uint32_t kNumBlockVirtical = 10;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブ生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			if (j > i) {
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			}
		}
	}

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換
				Matrix4x4 worldmatrix;

				worldmatrix = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				worldTransformBlock->matWorld_ = worldmatrix;

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}

#ifdef _DEBUG
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			isDebugCameraActive_ = true;
		}
#endif

		if (isDebugCameraActive_) {
			debugCamera_->Update();

			camera.matView = debugCamera_->GetCamera().matView;
			camera.matProjection = debugCamera_->GetCamera().matProjection;

			camera.TransferMatrix();
		} else {
			camera.UpdateMatrix();
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		// 軸表示の描画
		axisIndicator->Draw();

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				model_->PreDraw(dxCommon->GetCommandList());

				model_->Draw(*worldTransformBlock, camera);

				model_->PostDraw();
			}
		}

		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	delete model_;
	delete debugCamera_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}