#pragma once
// Windows
#include <Windows.h>
#include <assert.h>
#include <memory>
#include <string>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <DirectXMath.h>

// STL
#include <vector>
#include <queue>
#include <map>
using namespace Microsoft::WRL;

// Define
#define CHECK(hr) assert(SUCCEEDED(hr))

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
#include "Engine.h"