#include <Novice.h>
#include <cmath>
#include <cassert>

const char kWindowTitle[] = "LC1A_16_ツカモトキズナ_MT3_00_05_確認課題";

// 行列
//=========================

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

// 関数
//=========================
#pragma region VectorScreenPrintf

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, Matrix4x4 &matrix, const char *label) {
	Novice::ScreenPrintf(x, y, "%s", label);

	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth,
				y + row * kRowHeight + 20,
				"%6.02f", matrix.m[row][column]
			);
		}
	}
}

#pragma endregion

#pragma region 計算関数
Matrix4x4 MakeAffineMatrix(Vector3 &scale, Vector3 &rotation, Vector3 &translation) {
	// アフィン変換行列の作成
	Matrix4x4 affineMatrix = {};

	for (int i = 0; i < 4; i++) {
		affineMatrix.m[i][i] = 1.0f;
	}

	// アフィン変換行列
	affineMatrix.m[0][0] = scale.x * (cos(rotation.y) * cos(rotation.z));
	affineMatrix.m[0][1] = scale.x * (cos(rotation.y) * sin(rotation.z));
	affineMatrix.m[0][2] = scale.x * (-sin(rotation.y));

	affineMatrix.m[1][0] = scale.y * (sin(rotation.x) * sin(rotation.y) * cos(rotation.z) - cos(rotation.x) * sin(rotation.z));
	affineMatrix.m[1][1] = scale.y * (sin(rotation.x) * sin(rotation.y) * sin(rotation.z) + cos(rotation.x) * cos(rotation.z));
	affineMatrix.m[1][2] = scale.y * (sin(rotation.x) * cos(rotation.y));

	affineMatrix.m[2][0] = scale.z * (cos(rotation.x) * sin(rotation.y) * cos(rotation.z) + sin(rotation.x) * sin(rotation.z));
	affineMatrix.m[2][1] = scale.z * (cos(rotation.x) * sin(rotation.y) * sin(rotation.z) - sin(rotation.x) * cos(rotation.z));
	affineMatrix.m[2][2] = scale.z * (cos(rotation.x) * cos(rotation.y));

	affineMatrix.m[3][0] = translation.x;
	affineMatrix.m[3][1] = translation.y;
	affineMatrix.m[3][2] = translation.z;

	return affineMatrix;
};
#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// 変数の初期化
	Vector3 scale{1.2f, 0.79f, -2.1f};
	Vector3 rotate{0.4f, 1.43f, -0.8f};
	Vector3 transform{2.7f, -4.15f, 1.57f};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 worldMatrix = MakeAffineMatrix(scale, rotate, transform);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, worldMatrix, "worldMatrix");
		
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
