#pragma once
// Windows
#include <Windows.h>
#include <assert.h>
#include <memory>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <wrl/client.h>

// STL
#include <vector>
#include <map>
using namespace Microsoft::WRL;

// Define
#define CHECK(hr) assert(SUCCEEDED(hr))

// Engine
#include "Graphics/Graphic.h"
#include "Graphics/Geometry.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/ConstantBuffer.h"

#include "Structs.h"
#include "Engine.h"