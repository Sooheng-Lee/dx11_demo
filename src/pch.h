#pragma once
// Windows
#include <Windows.h>
#include <assert.h>
#include <memory>
#include <string>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <cstdlib>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <DirectXMath.h>

// STL
#include <vector>
#include <queue>
#include <array>
#include <unordered_map>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
using namespace Microsoft::WRL;

// Define
#define CHECK(hr) assert(SUCCEEDED(hr))

inline DirectX::XMFLOAT4X4 ConvertMatrix(
    const aiMatrix4x4& matrix)
{
    return DirectX::XMFLOAT4X4(
        matrix.a1,
        matrix.b1,
        matrix.c1,
        matrix.d1,

        matrix.a2,
        matrix.b2,
        matrix.c2,
        matrix.d2,

        matrix.a3,
        matrix.b3,
        matrix.c3,
        matrix.d3,

        matrix.a4,
        matrix.b4,
        matrix.c4,
        matrix.d4
    );
};

// Client
#include "Inputs/Keyboard.h"
#include "Utils/GameTimer.h"
// Engine
#include "Graphics/Graphic.h"
#include "Graphics/Geometry.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Camera.h"


#include "Structs.h"
#include "Graphics/PrimitiveGeometry.h"
#include "Model.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Mesh/SkeletalMesh.h"
#include "Mesh/StaticMesh.h"
#include "Collider/Collider.h"
#include "Collider/BoxCollider.h"
#include "Collider/SphereCollider.h"
#include "Collider/CapsuleCollider.h"
#include "GameObject.h"
#include "EnemyObject.h"
#include "Engine.h"
