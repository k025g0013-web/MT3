#include <Novice.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <algorithm>

#ifdef _DEBUG
#include "imgui.h"
#endif 

const char kWindowTitle[] = "LE2A_16_ツカモトキズナ_MT3_確認課題";

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

struct Sphere {
	Vector3 center;
	float radius;
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Plane {
	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 vertices[3];
};

struct AABB {
	Vector3 min;
	Vector3 max;
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

void MatrixScreenPrintf(int x, int y, const Matrix4x4 &matrix, const char *label) {
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
	float length = Length(v);
	Vector3 result = {
		v.x / length,
		v.y / length,
		v.z / length,
	};

	return result;
}

// 行列の加法
Matrix4x4 Add(const Matrix4x4 &m1, const Matrix4x4 &m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}
	return result;
}

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4 &m1, const Matrix4x4 &m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}
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

// 行列式
float Det3(
	float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3) {

	return
		a1 * (b2 * c3 - b3 * c2) -
		a2 * (b1 * c3 - b3 * c1) +
		a3 * (b1 * c2 - b2 * c1);
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4 &m) {
	Matrix4x4 result = {};

	// 余因子行列
	Matrix4x4 cofactor = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			float sub[3][3]{};
			int r = 0;

			// 元の行列を走査
			for (int row = 0; row < 4; ++row) {
				if (row == i) continue;
				int c = 0;

				for (int col = 0; col < 4; ++col) {
					if (col == j) continue;
					sub[r][c] = m.m[row][col];
					c++;
				}
				r++;
			}

			// 3x3行列式
			float minor = Det3(
				sub[0][0], sub[0][1], sub[0][2],
				sub[1][0], sub[1][1], sub[1][2],
				sub[2][0], sub[2][1], sub[2][2]
			);

			// 符号付き余因子
			float sign;
			if ((i + j) % 2 == 0) {
				sign = 1.0f;
			} else {
				sign = -1.0f;
			}

			cofactor.m[i][j] = sign * minor;
		}
	}

	// 転置行列
	Matrix4x4 adjugate = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			adjugate.m[i][j] = cofactor.m[j][i];
		}
	}

	// 行列式
	float det = 0.0f;
	for (int j = 0; j < 4; ++j) {
		det += m.m[0][j] * cofactor.m[0][j];
	}

	// ゼロチェック
	if (fabs(det) < 1e-4f) {
		return result;
	}

	// 逆行列
	float invDet = 1.0f / det;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = adjugate.m[i][j] * invDet;
		}
	}

	return result;
}

// 転置行列
Matrix4x4 Transpose(const Matrix4x4 &m) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m.m[column][row];
		}
	}
	return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1;
	}
	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3 &translate) {
	Matrix4x4 result = {};

	// 単位行列にする
	for (int i = 0; i < 4; ++i) {
		result.m[i][i] = 1.0f;
	}

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3 &scale) {
	Matrix4x4 result = {};

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;

	return result;
}

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

// x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {};

	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);

	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
};

// y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {};

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[1][1] = 1.0f;

	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
};

// z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {};

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);

	result.m[1][1] = std::cos(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
};

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Vector3 &rotation, const Vector3 &translation) {

	Matrix4x4 s = MakeScaleMatrix(scale);

	Matrix4x4 r = Multiply(Multiply(
		MakeRotateXMatrix(rotation.x),
		MakeRotateYMatrix(rotation.y)),
		MakeRotateZMatrix(rotation.z)
	);

	Matrix4x4 t = MakeTranslateMatrix(translation);

	return Multiply(Multiply(s, r), t);
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};

	float fov = 1.0f / std::tan(fovY / 2.0f);

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

// クロス積
Vector3 Cross(const Vector3 &v1, const Vector3 &v2) {
	Vector3 result = {};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
};

#pragma endregion

#pragma region 描画と点と線とAABB
// Drid表示
void DrawGrid(const Matrix4x4 &viewProjectionMatrix, const Matrix4x4 &viewportMatrix) {
	const float kGridHalfWidth = 2.0f;										// Gridの半分の幅
	const uint32_t kSubdivision = 10;										// 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);	// 一つ分の長さ

	// VPVMatrixを作る
	Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

	// 奥から手前の線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + xIndex * kGridEvery;

		// ワールド座標
		Vector3 start = { x, 0.0f, -kGridHalfWidth };
		Vector3 end = { x, 0.0f,  kGridHalfWidth };

		// スクリーン座標へ変換
		Vector3 startScreen = Transform(start, vpvMatrix);
		Vector3 endScreen = Transform(end, vpvMatrix);

		uint32_t color = 0xAAAAAAFF;

		// 原点軸は色を変える
		if (fabs(x) < 0.0001f) {
			color = 0x000000FF; // 黒
		}

		Novice::DrawLine(
			static_cast<int>(startScreen.x), static_cast<int>(startScreen.y),
			static_cast<int>(endScreen.x), static_cast<int>(endScreen.y),
			color
		);
	}
	// 右から左も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + zIndex * kGridEvery;

		// ワールド座標
		Vector3 start = { -kGridHalfWidth, 0.0f, z };
		Vector3 end = { kGridHalfWidth, 0.0f, z };

		// スクリーン座標へ変換
		Vector3 startScreen = Transform(start, vpvMatrix);
		Vector3 endScreen = Transform(end, vpvMatrix);

		uint32_t color = 0xAAAAAAFF;

		// 原点軸は色を変える
		if (fabs(z) < 0.0001f) {
			color = 0x000000FF; // 黒
		}

		Novice::DrawLine(
			static_cast<int>(startScreen.x), static_cast<int>(startScreen.y),
			static_cast<int>(endScreen.x), static_cast<int>(endScreen.y),
			color
		);
	}
};

// 球表示
void DrawSphere(const Sphere &sphere, const Matrix4x4 &viewProjectionMatrix, const Matrix4x4 &viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;									// 分割数
	const float kLonEvery = 2.0f * static_cast<float>(M_PI) / static_cast<float>(kSubdivision);	// 経度分割1つ分の角度
	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubdivision);			// 緯度分割1つ分の角度

	// VPVMatrixを作る
	Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

	// 緯度の方向に分割 -pi/2 ~ pi/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;
		float nextLat = lat + kLatEvery;

		// 経度の方向に分割 0 ~ 2pi
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			float nextLon = lon + kLonEvery;

			// world座標系でのa,b,cを求める
			Vector3 a = {
			   sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon),
			   sphere.center.y + sphere.radius * std::sin(lat),
			   sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon)
			};

			Vector3 b = {
				sphere.center.x + sphere.radius * std::cos(nextLat) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(nextLat),
				sphere.center.z + sphere.radius * std::cos(nextLat) * std::sin(lon)
			};

			Vector3 c = {
				sphere.center.x + sphere.radius * std::cos(lat) * std::cos(nextLon),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(nextLon)
			};

			// a,b,cをscreen座標系まで変換
			Vector3 aScreen = Transform(a, vpvMatrix);
			Vector3 bScreen = Transform(b, vpvMatrix);
			Vector3 cScreen = Transform(c, vpvMatrix);

			// ab,bcで線を引く
			Novice::DrawLine(
				static_cast<int>(aScreen.x), static_cast<int>(aScreen.y),
				static_cast<int>(bScreen.x), static_cast<int>(bScreen.y),
				color
			);
			Novice::DrawLine(
				static_cast<int>(aScreen.x), static_cast<int>(aScreen.y),
				static_cast<int>(cScreen.x), static_cast<int>(cScreen.y),
				color
			);
		}
	}
};

// 垂直線
Vector3 Perpendicular(const Vector3 &vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y, vector.x, 0.0f };
	}
	return { 0.0f, -vector.z, vector.y };
}

// 平面表示
void DrawPlane(const Plane &plane, const Matrix4x4 &viewProjectionMatrix, const Matrix4x4 &viewportMatrix, uint32_t color) {
	// 中心点を決める
	Vector3 center = Multiply(plane.distance, plane.normal);

	// 垂直線ベクトルを求める
	Vector3 perpendiculars[4]{};
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));								// 法線と垂直なベクトルを1つ求める
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z };	// 逆ベクトルを求める
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);								// 法線とのクロス積を求める
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };	// 逆ベクトルを求める

	// 四頂点を求める
	Vector3 points[4]{};
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	// 四頂点を結ぶ
	for (int i = 0; i < 2; i++) {
		Novice::DrawLine(
			static_cast<int>(points[i].x), static_cast<int>(points[i].y),
			static_cast<int>(points[2].x), static_cast<int>(points[2].y),
			color
		);

		Novice::DrawLine(
			static_cast<int>(points[i].x), static_cast<int>(points[i].y),
			static_cast<int>(points[3].x), static_cast<int>(points[3].y),
			color
		);
	}
}

// 三角形表示
void DrawTriangle(const Triangle &triangle, const Matrix4x4 &viewProjectionMatrix, const Matrix4x4 &viewportMatrix, uint32_t color) {
	// VPVMatrixを作る
	Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

	// スクリーン座標系まで変換
	Vector3 v0 = Transform(triangle.vertices[0], vpvMatrix);
	Vector3 v1 = Transform(triangle.vertices[1], vpvMatrix);
	Vector3 v2 = Transform(triangle.vertices[2], vpvMatrix);

	// 描画
	Novice::DrawLine(static_cast<int>(v0.x), static_cast<int>(v0.y), static_cast<int>(v1.x), static_cast<int>(v1.y), color);
	Novice::DrawLine(static_cast<int>(v1.x), static_cast<int>(v1.y), static_cast<int>(v2.x), static_cast<int>(v2.y), color);
	Novice::DrawLine(static_cast<int>(v2.x), static_cast<int>(v2.y), static_cast<int>(v0.x), static_cast<int>(v0.y), color);
}

// AABB正規化
AABB Normalize(const AABB &aabb) {
	AABB result;

	result.min.x = (std::min)(aabb.min.x, aabb.max.x);
	result.max.x = (std::max)(aabb.min.x, aabb.max.x);

	result.min.y = (std::min)(aabb.min.y, aabb.max.y);
	result.max.y = (std::max)(aabb.min.y, aabb.max.y);

	result.min.z = (std::min)(aabb.min.z, aabb.max.z);
	result.max.z = (std::max)(aabb.min.z, aabb.max.z);

	return result;
}

// AABB表示
void DrawAABB(const AABB &aabb, const Matrix4x4 &viewProjectionMatrix, const Matrix4x4 &viewportMatrix, uint32_t color) {
	// VPVMatrixを作る
	Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

	// ワールド座標での八頂点を求める
	Vector3 worldVertices[8] = {
		{aabb.min.x, aabb.min.y, aabb.min.z}, // 0
		{aabb.max.x, aabb.min.y, aabb.min.z}, // 1
		{aabb.max.x, aabb.max.y, aabb.min.z}, // 2
		{aabb.min.x, aabb.max.y, aabb.min.z}, // 3

		{aabb.min.x, aabb.min.y, aabb.max.z}, // 4
		{aabb.max.x, aabb.min.y, aabb.max.z}, // 5
		{aabb.max.x, aabb.max.y, aabb.max.z}, // 6
		{aabb.min.x, aabb.max.y, aabb.max.z}  // 7
	};

	// スクリーン座標まで変換
	Vector3 screenVertex[8];
	for (int i = 0; i < 8; i++) {
		screenVertex[i] = Transform(worldVertices[i], vpvMatrix);
	}


#pragma region 描画処理
	// 手前面
	Novice::DrawLine(
		static_cast<int>(screenVertex[0].x), static_cast<int>(screenVertex[0].y), 
		static_cast<int>(screenVertex[1].x), static_cast<int>(screenVertex[1].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[1].x), static_cast<int>(screenVertex[1].y), 
		static_cast<int>(screenVertex[2].x), static_cast<int>(screenVertex[2].y), color);
	Novice::DrawLine(
		static_cast<int>(screenVertex[2].x), static_cast<int>(screenVertex[2].y), 
		static_cast<int>(screenVertex[3].x), static_cast<int>(screenVertex[3].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[3].x), static_cast<int>(screenVertex[3].y), 
		static_cast<int>(screenVertex[0].x), static_cast<int>(screenVertex[0].y), color
	);

	// 奥面
	Novice::DrawLine(
		static_cast<int>(screenVertex[4].x), static_cast<int>(screenVertex[4].y), 
		static_cast<int>(screenVertex[5].x), static_cast<int>(screenVertex[5].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[5].x), static_cast<int>(screenVertex[5].y), 
		static_cast<int>(screenVertex[6].x), static_cast<int>(screenVertex[6].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[6].x), static_cast<int>(screenVertex[6].y), 
		static_cast<int>(screenVertex[7].x), static_cast<int>(screenVertex[7].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[7].x), static_cast<int>(screenVertex[7].y), 
		static_cast<int>(screenVertex[4].x), static_cast<int>(screenVertex[4].y), color
	);

	// 側面
	Novice::DrawLine(
		static_cast<int>(screenVertex[0].x), static_cast<int>(screenVertex[0].y), 
		static_cast<int>(screenVertex[4].x), static_cast<int>(screenVertex[4].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[1].x), static_cast<int>(screenVertex[1].y), 
		static_cast<int>(screenVertex[5].x), static_cast<int>(screenVertex[5].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[2].x), static_cast<int>(screenVertex[2].y), 
		static_cast<int>(screenVertex[6].x), static_cast<int>(screenVertex[6].y), color
	);
	Novice::DrawLine(
		static_cast<int>(screenVertex[3].x), static_cast<int>(screenVertex[3].y), 
		static_cast<int>(screenVertex[7].x), static_cast<int>(screenVertex[7].y), color
	);
#pragma endregion
}

// 正射影ベクトル
Vector3 Project(const Vector3 &v1, const Vector3 &v2) {
	float dot = Dot(v1, v2);
	float lengthSq = Dot(v2, v2);

	float t = dot / lengthSq;

	return Multiply(t, v2);
}

// 最近接点
Vector3 ClosestPoint(const Vector3 &point, const Segment &segment) {
	Vector3 vector = Subtract(point, segment.origin);

	float t = Dot(vector, segment.diff) / Dot(segment.diff, segment.diff);
	t = std::clamp(t, 0.0f, 1.0f);

	return Add(segment.origin, Multiply(t, segment.diff));
}

#pragma endregion

#pragma region 当たり判定
// 球同士の当たり判定
bool IsCollision(const Sphere &s1, const Sphere &s2) {
	float distance = Length(
		Subtract(s2.center, s1.center)
	);

	return distance <= s1.radius + s2.radius;
}

// 球と平面の当たり判定
bool IsCollision(const Sphere &sphere, const Plane &plane) {
	float distance = Dot(sphere.center, plane.normal) - plane.distance;

	return (-sphere.radius <= distance && distance <= sphere.radius);
}

// 直線と平面の当たり判定
bool IsCollision(const Line &line, const Plane &plane) {
	// 内積
	float dot = Dot(plane.normal, line.diff);

	// ゼロチェック
	if (fabs(dot) < 1e-4f) {
		return false;
	}

	return true;
}

// 半線と平面の当たり判定
bool IsCollision(const Ray &ray, const Plane &plane) {
	// 内積
	float dot = Dot(plane.normal, ray.diff);

	// ゼロチェック
	if (fabs(dot) < 1e-4f) {
		return false;
	}

	// t
	float t = (plane.distance - Dot(ray.origin, plane.normal)) / dot;

	return (t >= 0.0f);
}

// 線分と平面の当たり判定
bool IsCollision(const Segment &segment, const Plane &plane) {
	// 内積
	float dot = Dot(plane.normal, segment.diff);

	// ゼロチェック
	if (fabs(dot) < 1e-4f) {
		return false;
	}

	// t
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	return (0.0f <= t && t <= 1.0f);
}

// 三角形と直線の当たり判定
bool IsCollision(const Triangle &triangle, const Line &line) {
	// 各辺を結んだベクトル
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	// 法線
	Vector3 normal = Cross(
		v01, Subtract(triangle.vertices[2], triangle.vertices[0])
	);

	// 分母
	float denom = Dot(normal, line.diff);

	// ゼロチェック
	if (fabs(denom) < 1e-4f) {
		return false;
	}

	// t
	float t = Dot(normal, Subtract(triangle.vertices[0], line.origin)) / denom;

	// 衝突点p
	Vector3 p = Add(line.origin, Multiply(t, line.diff));

	//各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を求める
	Vector3 cross01 = Cross(v01, Subtract(p, triangle.vertices[0]));
	Vector3 cross12 = Cross(v12, Subtract(p, triangle.vertices[1]));
	Vector3 cross20 = Cross(v20, Subtract(p, triangle.vertices[2]));

	// 全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	return Dot(cross01, normal) >= 0 &&
		Dot(cross12, normal) >= 0 &&
		Dot(cross20, normal) >= 0;
}

// 三角形と半線の当たり判定
bool IsCollision(const Triangle &triangle, const Ray &ray) {
	// 各辺を結んだベクトル
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	// 法線
	Vector3 normal = Cross(
		v01, Subtract(triangle.vertices[2], triangle.vertices[0])
	);

	// 分母
	float denom = Dot(normal, ray.diff);

	// ゼロチェック
	if (fabs(denom) < 1e-4f) {
		return false;
	}

	// t
	float t = Dot(normal, Subtract(triangle.vertices[0], ray.origin)) / denom;

	// 判定制限
	if (t < 0.0f) {
		return false;
	}

	// 衝突点p
	Vector3 p = Add(ray.origin, Multiply(t, ray.diff));

	//各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を求める
	Vector3 cross01 = Cross(v01, Subtract(p, triangle.vertices[0]));
	Vector3 cross12 = Cross(v12, Subtract(p, triangle.vertices[1]));
	Vector3 cross20 = Cross(v20, Subtract(p, triangle.vertices[2]));

	// 全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	return Dot(cross01, normal) >= 0 &&
		Dot(cross12, normal) >= 0 &&
		Dot(cross20, normal) >= 0;
}

// 三角形と線分の当たり判定
bool IsCollision(const Triangle &triangle, const Segment &segment) {
	// 各辺を結んだベクトル
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	// 法線
	Vector3 normal = Cross(
		v01, Subtract(triangle.vertices[2], triangle.vertices[0])
	);

	// 分母
	float denom = Dot(normal, segment.diff);

	// ゼロチェック
	if (fabs(denom) < 1e-4f) {
		return false;
	}

	// t
	float t = Dot(normal, Subtract(triangle.vertices[0], segment.origin)) / denom;

	// 判定制限
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 衝突点p
	Vector3 p = Add(segment.origin, Multiply(t, segment.diff));
	
	//各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を求める
	Vector3 cross01 = Cross(v01, Subtract(p, triangle.vertices[0]));
	Vector3 cross12 = Cross(v12, Subtract(p, triangle.vertices[1]));
	Vector3 cross20 = Cross(v20, Subtract(p, triangle.vertices[2]));

	// 全ての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	return Dot(cross01, normal) >= 0 &&
		   Dot(cross12, normal) >= 0 && 
		   Dot(cross20, normal) >= 0;
}

// AABB同士の当たり判定
bool IsCollision(const AABB &aabb1, const AABB &aabb2) {
	// 衝突判定
	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		   (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		   (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);
}

// AABBと球の当たり判定
bool IsCollision(const AABB &aabb, const Sphere &sphere) {
	// 最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};

	// 最近接点と弾の中心の距離を求める
	float distance = Length(Subtract(closestPoint, sphere.center));

	// 距離が半径よりも小さければ衝突
	return distance <= sphere.radius;
}

#pragma endregion

static const float kWindowWidth = 1280.0f;
static const float kWindowHeight = 720.0f;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/* 変数の初期化
	---------------*/
	// AABB
	AABB aabb1{
		.min{-0.5f, -0.5f, -0.5f},
		.max{ 0.0f,  0.0f,  0.0f},
	};

	AABB aabb2{
		.min{ 0.2f,  0.2f,  0.2f},
		.max{ 1.0f,  1.0f,  1.0f},
	};

	// カメラ
	Vector3 cameraTranslation{ 3.3f, 2.2f, -6.5f };
	Vector3 cameraRotate{ 0.3f, -0.5f, 0.0f };

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

		/* ImGui
		---------------*/
#pragma region ImGui
		ImGui::Begin("Window");

		// AABB
		ImGui::DragFloat3("aabb1.min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("aabb1.max", &aabb1.max.x, 0.01f);
		aabb1 = Normalize(aabb1);

		ImGui::DragFloat3("aabb2.min", &aabb2.min.x, 0.01f);
		ImGui::DragFloat3("aabb2.max", &aabb2.max.x, 0.01f);
		aabb2 = Normalize(aabb2);

		ImGui::End();
#pragma endregion

		/* カメラ処理
		---------------*/
		Matrix4x4 cameraMatrix =
			MakeAffineMatrix({ 1,1,1 }, cameraRotate, cameraTranslation);

		Matrix4x4 viewMatrix = Inverse(cameraMatrix);

		// 各種行列の計算
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

		// VPMatrixを作る
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

		// ViewportMatrixを作る
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// グリッド
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// AABB
		if (IsCollision(aabb1, aabb2)) {
			DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, 0xFF0000FF);
		} else {
			DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);
		}

		DrawAABB(aabb2, viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);

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