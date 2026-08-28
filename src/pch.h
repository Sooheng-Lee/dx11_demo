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

#include "Graphics/Graphic.h"
#include "Graphics/Geometry.h"

#include "Structs.h"
#include "Engine.h"
// Define
#define CHECK(hr) assert(SUCCEEDED(hr))