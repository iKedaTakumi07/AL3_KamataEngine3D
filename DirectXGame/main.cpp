#include <KamataEngine.h>

using namespace KamataEngine;

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

	// サウンドデータハンドル
	uint32_t soundDataHandle_ = 0;
	soundDataHandle_ = audio->LoadWave("fanfare.wav");

	// 音声再生ハンドル
	uint32_t voiceHandel_ = 0;

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト
	/*uint32_t textureHandel_ = 0;
	textureHandel_ = TextureManager::Load("sample.png");*/

	// モデル
	uint32_t textureHandel_2 = 0;
	textureHandel_2 = TextureManager::Load("sample.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// スプライト
	/*Sprite* sprite_ = nullptr;
	sprite_ = Sprite::Create(textureHandel_, {100, 50});*/

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 3Dモデル
	Model* model_ = nullptr;
	model_ = Model::Create();

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	worldTransform_.Initialize();

	// カメラ
	Camera camera;
	camera.translation_ = Vector3(0.0f, 0.0f, -5.0f);
	camera.Initialize();

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

		// スプライト
		// スプライトの今の座標を所得
		// Vector2 position = sprite_->GetPosition();
		//// 座標を{2,1}移動
		// position.x += 2.0f;
		// position.y += 1.0f;
		//// 移動した座標をスプライトに反映
		// sprite_->SetPosition(position);

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 音声再生
		// audio->PlayWave(soundDataHandle_);

		voiceHandel_ = audio->PlayWave(soundDataHandle_, true, 0.5f);

		if (input->TriggerKey(DIK_SPACE)) {
			// 音声停止
			audio->StopWave(voiceHandel_);
		}

		// 描画開始
		dxCommon->PreDraw();

		// スプライト
		//// スプライト描画前処理
		// Sprite::PreDraw(dxCommon->GetCommandList());

		//// スプライトを描画
		// sprite_->Draw();

		//// スプライト描画後処理
		// Sprite::PostDraw();

		// モデル
		Model::PreDraw(dxCommon->GetCommandList());
		model_->Draw(worldTransform_, camera, textureHandel_2);
		Model::PostDraw();

		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	/*delete sprite_;*/
	delete model_;

	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}