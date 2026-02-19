#include <Novice.h>
#include <cmath>
#include <cassert>

const char kWindowTitle[] = "LC1A_16_ツカモトキズナ_MT3_01_01_確認課題";

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

void VectorScreenPrintf(int x, int y, const Vector3 &vector, const char *label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

#pragma endregion

#pragma region 計算関数
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result = {};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
};

// アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Vector3 &rotation, const Vector3 &translation) {
	Matrix4x4 affineMatrix = {};

	for (int i = 0; i < 4; i++) {
		affineMatrix.m[i][i] = 1.0f;
	}

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

// 逆行列
Matrix4x4 Inverse(const Matrix4x4 &m) {
	Matrix4x4 result = {};

	float det =
		m.m[0][0] * (
			m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) -
			m.m[1][2] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) +
			m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) -
		m.m[0][1] * (
			m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) -
			m.m[1][2] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) +
			m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0])) +
		m.m[0][2] * (
			m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) -
			m.m[1][1] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) +
			m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])) -
		m.m[0][3] * (
			m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]) -
			m.m[1][1] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]) +
			m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]));

	float inverseDet = 1.0f / det;

	result.m[0][0] =
		(m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])
			- m.m[1][2] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])
			+ m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) * inverseDet;
	result.m[0][1] =
		-(m.m[0][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])
			- m.m[0][2] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])
			+ m.m[0][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) * inverseDet;
	result.m[0][2] =
		(m.m[0][1] * (m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2])
			- m.m[0][2] * (m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1])
			+ m.m[0][3] * (m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1])) * inverseDet;
	result.m[0][3] =
		-(m.m[0][1] * (m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2])
			- m.m[0][2] * (m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1])
			+ m.m[0][3] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1])) * inverseDet;

	result.m[1][0] =
		-(m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])
			- m.m[1][2] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0])
			+ m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0])) * inverseDet;
	result.m[1][1] =
		(m.m[0][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])
			- m.m[0][2] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0])
			+ m.m[0][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0])) * inverseDet;
	result.m[1][2] =
		-(m.m[0][0] * (m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2])
			- m.m[0][2] * (m.m[1][0] * m.m[3][3] - m.m[1][3] * m.m[3][0])
			+ m.m[0][3] * (m.m[1][0] * m.m[3][2] - m.m[1][2] * m.m[3][0])) * inverseDet;
	result.m[1][3] =
		(m.m[0][0] * (m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2])
			- m.m[0][2] * (m.m[1][0] * m.m[2][3] - m.m[1][3] * m.m[2][0])
			+ m.m[0][3] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0])) * inverseDet;


	result.m[2][0] =
		(m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])
			- m.m[1][1] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0])
			+ m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])) * inverseDet;
	result.m[2][1] =
		-(m.m[0][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])
			- m.m[0][1] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0])
			+ m.m[0][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])) * inverseDet;
	result.m[2][2] =
		(m.m[0][0] * (m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1])
			- m.m[0][1] * (m.m[1][0] * m.m[3][3] - m.m[1][3] * m.m[3][0])
			+ m.m[0][3] * (m.m[1][0] * m.m[3][1] - m.m[1][1] * m.m[3][0])) * inverseDet;
	result.m[2][3] =
		-(m.m[0][0] * (m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1])
			- m.m[0][1] * (m.m[1][0] * m.m[2][3] - m.m[1][3] * m.m[2][0])
			+ m.m[0][3] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0])) * inverseDet;


	result.m[3][0] =
		-(m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])
			- m.m[1][1] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0])
			+ m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])) * inverseDet;
	result.m[3][1] =
		(m.m[0][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])
			- m.m[0][1] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0])
			+ m.m[0][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])) * inverseDet;
	result.m[3][2] =
		-(m.m[0][0] * (m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1])
			- m.m[0][1] * (m.m[1][0] * m.m[3][2] - m.m[1][2] * m.m[3][0])
			+ m.m[0][2] * (m.m[1][0] * m.m[3][1] - m.m[1][1] * m.m[3][0])) * inverseDet;
	result.m[3][3] =
		(m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1])
			- m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0])
			+ m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0])) * inverseDet;

	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int k = 0; k < 4; k++) {
				result.m[row][column] += m1.m[row][k] * m2.m[k][column];
			}
		}
	}
	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};

	float fov = 1.0f / tanf(fovY / 2.0f);

	result.m[0][0] = fov / aspectRatio;
	result.m[1][1] = fov;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
};

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][3] = 1.0f;

	result.m[3][0] = -(right + left) / (right - left);
	result.m[3][1] = -(top + bottom) / (top - bottom);
	result.m[3][2] = -nearClip / (farClip - nearClip);

	return result;
};

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {};

	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][3] = 1.0f;

	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;

	return result;
};

// 座標変換
Vector3 Transform(const Vector3 &vector, const Matrix4x4 &matrix) {
	Vector3 result = {};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
#pragma endregion

static const float kWindowWidth = 1280.0f;
static const float kWindowHeight = 720.0f;

Vector3 cameraPosition{ 0.0f, 0.0f, -5.0f };
Vector3 kLocalVertices[3] = {
	{ -0.5f, -0.5f, 0.0f },
	{  0.0f,  0.5f, 0.0f },
	{  0.5f, -0.5f, 0.0f }
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/* 変数の初期化
	---------------*/
	// クロス積
	Vector3 v1{ 1.2f, -3.9f,  2.5f };
	Vector3 v2{ 2.8f,  0.4f, -1.3f };
	
	// 三角形の3D描画
	Vector3 rotate{};
	Vector3 translate{};

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

		/* クロス積
		---------------*/
		Vector3 cross = Cross(v1, v2);

		/*  三角形の3D描画
		---------------*/
		// 三角形の移動 (上下)
		if (keys[DIK_W]) {
			translate.z += 0.05f;   // 前
		}
		if (keys[DIK_S]) {
			translate.z -= 0.05f;   // 後
		}

		// 三角形の移動(左右)
		if (keys[DIK_A]) {
			translate.x -= 0.05f;   // 左
		}
		if (keys[DIK_D]) {
			translate.x += 0.05f;   // 右
		}

		// 三角形の回転
		rotate.y += 0.05f;

		// 各種行列の計算
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		// WVPMatrixを作る
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		// ViewportMatrixを作る
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		// Screen空間へと頂点を変換する
		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		/* クロス積
		---------------*/
		VectorScreenPrintf(0, 0, cross, "Cross");

		/*  三角形の3D描画
		---------------*/
		Novice::DrawTriangle(
			static_cast<int>(screenVertices[0].x), static_cast<int>(screenVertices[0].y),
			static_cast<int>(screenVertices[1].x), static_cast<int>(screenVertices[1].y),
			static_cast<int>(screenVertices[2].x), static_cast<int>(screenVertices[2].y),
			0xFF0000FF, kFillModeSolid
		);

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
