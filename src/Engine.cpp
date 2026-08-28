#include "pch.h"

Engine::Engine()
{
	_constData.worldMat = DirectX::XMMatrixIdentity();
	_constData.viewMat = DirectX::XMMatrixIdentity();
	_constData.projMat = DirectX::XMMatrixIdentity();

	for (DirectX::XMMATRIX& boneMat : _boneConstData.boneMats)
	{
		boneMat = DirectX::XMMatrixIdentity();
	}
}

Engine::~Engine()
{
}

bool Engine::Init()
{
	if(!Graphic::GetInstance()->Create(_handle, _width, _height)) return false;
	
	CreateVertexShader();
	CreateInputLayout();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreatePixelShader();
	CreateSRV();
	CreateAnimation();
	CreateConstantBuffer();
	CreateRSState();
	CreateBlendState();
	CreateSamplerState();
	_camera = std::make_shared<Camera>();
	_camera->SetPosition(0.0f, 0.0f, -10.0f);
	GameTimer::GetInstance()->ToggleState();
	return true;
}

void Engine::Update()
{
	__super::Update();
	DirectX::XMFLOAT3 camPosition = _camera->GetPosition();
	FLOAT moveSpeed = 10.0f;
	if (_keyboard->GetKey('W'))
	{
		camPosition.z += GameTimer::GetInstance()->GetDeltaTime() * moveSpeed;
		OutputDebugStringW(L"W\n");
	}
	if (_keyboard->GetKey('S'))
	{
		camPosition.z -= GameTimer::GetInstance()->GetDeltaTime() * moveSpeed;
		OutputDebugStringW(L"S\n");
	}
	if (_keyboard->GetKey('D'))
	{
		camPosition.x += GameTimer::GetInstance()->GetDeltaTime() * moveSpeed;
		OutputDebugStringW(L"D\n");
	}
	if (_keyboard->GetKey('A'))
	{
		camPosition.x -= GameTimer::GetInstance()->GetDeltaTime() * moveSpeed;
		OutputDebugStringW(L"A\n");
	}
	if (_keyboard->GetKey('E'))
	{
		camPosition.y += GameTimer::GetInstance()->GetDeltaTime() * moveSpeed;
		OutputDebugStringW(L"E\n");
	}
	if (_keyboard->GetKey('Q'))
	{
		camPosition.y -= GameTimer::GetInstance()->GetDeltaTime() * moveSpeed;
		OutputDebugStringW(L"Q\n");
	}
	if (_keyboard->GetKeyDown(VK_ESCAPE))
	{
		GameTimer::GetInstance()->ToggleState();
		OutputDebugStringW(L"ESC\n");
	}

	_camera->SetPosition(camPosition.x, camPosition.y, camPosition.z);
	static float angle = 0.001f;
	angle += GameTimer::GetInstance()->GetDeltaTime();
	const DirectX::XMMATRIX worldMat =
		DirectX::XMMatrixRotationY(angle);
	_constData.worldMat = DirectX::XMMatrixTranspose(worldMat);
	_constData.viewMat = DirectX::XMMatrixTranspose(_camera->GetViewMat());
	_constData.projMat = DirectX::XMMatrixTranspose(_camera->GetProjMat());
	_constantBuffer->Update(&_constData);

	if (_animation != nullptr)
	{
		_animation->Update(GameTimer::GetInstance()->GetDeltaTime());
		_boneConstData = _animation->GetBoneTransformData();
		_boneConstantBuffer->Update(&_boneConstData);
	}
}

void Engine::Render()
{
	Graphic::GetInstance()->RenderBegin();
	UINT offset = 0;
	Graphic::GetInstance()->GetDeviceContext()->IASetInputLayout(_geometry->GetComPtr().Get());
	Graphic::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), _vertexBuffer->GetStridePtr(), &offset);
	Graphic::GetInstance()->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, offset);
	Graphic::GetInstance()->GetDeviceContext()->RSSetState(_rsState.Get());
	Graphic::GetInstance()->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(1, 1, _boneConstantBuffer->GetComPtr().GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
	Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, _texture->GetComPtr().GetAddressOf());

	FLOAT blendFactor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	Graphic::GetInstance()->GetDeviceContext()->OMSetBlendState(_blendState.Get(), blendFactor, 0xFFFFFFFF);
	Graphic::GetInstance()->GetDeviceContext()->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
	// RenderEnd
	Graphic::GetInstance()->RenderEnd();
}

void Engine::CreateInputLayout()
{
	_geometry = std::make_shared<Geometry>();
	std::vector<D3D11_INPUT_ELEMENT_DESC>layout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_geometry->Create(layout, _vertexShader->GetBlob());
}

void Engine::CreateVertexBuffer()
{
	_model.Load("Data\\FBX\\Man\\Man.fbx");
	_vertexBuffer = std::make_shared<VertexBuffer<VertexAnimData>>();
	/*
	std::vector<VertexTexData> vertices;
	vertices.resize(4);
	{
		vertices[0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f);
		vertices[0].uv = DirectX::XMFLOAT2(0.0f, 1.0f);
		vertices[1].position = DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f);
		vertices[1].uv = DirectX::XMFLOAT2(0.0f, 0.0f);
		vertices[2].position = DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f);
		vertices[2].uv = DirectX::XMFLOAT2(1.0f, 0.0f);
		vertices[3].position = DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f);
		vertices[3].uv = DirectX::XMFLOAT2(1.0f, 1.0f);
	}
	*/
	_vertexBuffer->Create(_model.GetVertices());
}

void Engine::CreateIndexBuffer()
{
	_indexBuffer = std::make_shared<IndexBuffer>();
	//std::vector<UINT> indices = { 0, 1, 2, 2, 3, 0 };
	_indexBuffer->Create(_model.GetIndices());
}

void Engine::CreateVertexShader()
{
	_vertexShader = std::make_shared<VertexShader>();
	_vertexShader->Create(L"src\\HLSL\\DefaultAnim.hlsl", "VS", "vs_5_0");
}

void Engine::CreatePixelShader()
{
	_pixelShader = std::make_shared<PixelShader>();
	_pixelShader->Create(L"src\\HLSL\\DefaultAnim.hlsl", "PS", "ps_5_0");
}

void Engine::CreateSRV()
{
	
	_texture = std::make_shared<Texture>();
	_texture->Create(L"Data\\FBX\\Man\\Man_texture_0.png");
}

void Engine::CreateAnimation()
{
	_animation = std::make_shared<Animation>();
	_animation->Load(
		"Data\\FBX\\Man\\Idle.fbx",
		_model.GetBoneInfoMap(),
		_model.GetBoneCount()
	);
	_boneConstData = _animation->GetBoneTransformData();
}

void Engine::CreateConstantBuffer()
{
	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	_constantBuffer->Create();

	_boneConstantBuffer = std::make_shared<ConstantBuffer<BoneTransformData>>();
	_boneConstantBuffer->Create();
	_boneConstantBuffer->Update(&_boneConstData);
}

void Engine::CreateRSState()
{
	CD3D11_RASTERIZER_DESC desc{ D3D11_DEFAULT };
	desc.CullMode = D3D11_CULL_NONE;
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateRasterizerState(&desc, _rsState.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateBlendState()
{
	CD3D11_BLEND_DESC desc{ D3D11_DEFAULT };
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hr);
}

void Engine::CreateSamplerState()
{
	CD3D11_SAMPLER_DESC desc{ D3D11_DEFAULT };
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateSamplerState(&desc, _samplerState.GetAddressOf());
	CHECK(hr);
}
