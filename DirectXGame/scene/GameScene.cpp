#include "GameScene.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include <ImGuiManager.h>
#include <cassert>
#include "AxisIndicator.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle_ = TextureManager::Load("sample.png");
	uvCheckerHandle_ = TextureManager::Load("uvChecker.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	model_ = Model::Create();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示を有効
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	soundDataHandle_ = audio_->LoadWave("mokugyo.wav");
	audio_->PlayWave(soundDataHandle_);
	voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);
}

void GameScene::Update() {
#ifdef _DEBUG
	ImGui::Begin("Debug1");
	ImGui::Text("Choung TinLong %d.%d.%d", 2004, 10, 16);

	ImGui::InputFloat("InputFloat3", inputFloat3);
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);

	ImGui::ShowDemoWindow();

	ImGui::End();
#endif // _DEBUG
	debugCamera_->Update();

	Vector2 position = sprite_->GetPosition();
	/*position.x += 2.0f;
	position.y += 1.0f;*/
	sprite_->SetPosition(position);

	if (input_->TriggerKey(DIK_SPACE)) {
		if (audio_->IsPlaying(voiceHandle_)) {
			audio_->StopWave(voiceHandle_);
		} else {
			audio_->PlayWave(soundDataHandle_, true);
		}
	}
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
	//model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), uvCheckerHandle_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	for (int i = 0; i < 20; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d({float(i * 2) - 20, -20, 50}, {float(i * 2) - 20, 18, 20}, {1.0f, 1.0f, 2.0f, 1.0f});
		PrimitiveDrawer::GetInstance()->DrawLine3d({-20, float(i * 2) - 20, 20}, {18, float(i * 2) - 20, 50}, {1.0f, 0, 0, 1.0f});
	}


	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
