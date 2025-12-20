#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "LC1A_16_ツカモトキズナ_MT3_00_01_確認課題";

// 行列
//=========================
struct Vector3 {
	float x;
	float y;
	float z;
};

// 関数
//=========================
#pragma region VectorScreenPrintf

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3 &vector, const char *label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

#pragma endregion

#pragma region 計算関数
// 加算
Vector3 Add(const Vector3 &v1, const Vector3 &v2) {
	Vector3 result = {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
	};
	return result;
}

// 減算
Vector3 Subtract(const Vector3 &v1, const Vector3 &v2) {
	Vector3 result = {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
	};
	return result;
}

// スカラー倍
Vector3 Multiply(float scalar, const Vector3 &v) {
	Vector3 result = {
		scalar * v.x,
		scalar * v.y,
		scalar * v.z,
	};
	return result;
}

// 内積
float Dot(const Vector3 &v1, const Vector3 &v2) {
	float result =
		v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return result;
}

// 長さ
float Length(const Vector3 &v) {
	float result =
		sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2));

	return result;
}

// 正規化
Vector3 Normalize(const Vector3 &v) {
	Vector3 result = {
		v.x / sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2)),
		v.y / sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2)),
		v.z / sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2)),
	};
	return result;
}
#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// 変数の初期化
	//=========================
	Vector3 v1{ 1.0f, 3.0f, -5.0f };
	Vector3 v2{ 4.0f, -1.0f, 2.0f };
	float k = { 4.0f };

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

		Vector3 resultAdd = Add(v1, v2);
		Vector3 resultSubtract = Subtract(v1, v2);
		Vector3 resultMultiply = Multiply(k, v1);
		float	resultDot = Dot(v1, v2);
		float	resultLength = Length(v1);
		Vector3 resultNormalize = Normalize(v2);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, resultAdd, ": Add");
		VectorScreenPrintf(0, kRowHeight, resultSubtract, ": Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, resultMultiply, ": Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f : Dot", resultDot);
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f : Length", resultLength);
		VectorScreenPrintf(0, kRowHeight * 5, resultNormalize, ": Normalize");

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
