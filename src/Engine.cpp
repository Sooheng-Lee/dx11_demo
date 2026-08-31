#include "pch.h"

Engine::Engine()
{
}

Engine::~Engine()
{
}

bool Engine::Init()
{
	if(!Graphic::GetInstance()->Create(_handle, _width, _height)) return false;

	CreateRSState();
	CreateBlendState();
	CreateSamplerState();

	_character = std::make_shared<GameObject>();
	_character->Init(
		"Data\\FBX\\Man\\Man.fbx",
		L"Data\\FBX\\Man\\Man_texture_0.png",
		L"Data\\FBX\\Man\\Man_normal.png",
		L"Data\\FBX\\Man\\Man_roughness.png",
		L"Data\\FBX\\Man\\Man_metallic.png");
	_character->AddAnimation("Idle", "Data\\FBX\\Man\\Animation\\Idle.fbx");
	_character->AddAnimation("Walk", "Data\\FBX\\Man\\Animation\\Walk.fbx");
	_character->PlayAnimation("Idle", 0.0f);
	DirectionalLight characterLight;
	characterLight.SetDirection(0.3f, -1.0f, 0.3f);
	characterLight.SetColor(1.0f, 1.0f, 1.0f);
	characterLight.SetIntensity(1.0f);
	_character->SetDirectionalLight(characterLight);
	std::shared_ptr<CapsuleCollider> collider = std::make_shared<CapsuleCollider>(0.35f, 1.8f);
	collider->SetCenter(DirectX::XMFLOAT3(0.0f, 0.9f, 0.0f));
	collider->Init();
	_character->SetCollider(collider);

	_woman = std::make_shared<GameObject>();
	_woman->Init(
		"Data\\FBX\\Woman\\Woman.fbx",
		L"Data\\FBX\\Woman\\Woman_texture.png",
		L"Data\\FBX\\Woman\\Woman_normal.png",
		L"Data\\FBX\\Woman\\Woman_roughness.png",
		L"Data\\FBX\\Woman\\Woman_metallic.png");
	_woman->SetPosition(DirectX::XMFLOAT3(1.5f, 0.0f, 0.0f));
	_woman->AddAnimation("Idle", "Data\\FBX\\Woman\\Animation\\Idle.fbx");
	_woman->PlayAnimation("Idle", 0.0f);
	_woman->SetDirectionalLight(characterLight);
	std::shared_ptr<CapsuleCollider> womanCollider = std::make_shared<CapsuleCollider>(0.35f, 1.8f);
	womanCollider->SetCenter(DirectX::XMFLOAT3(0.0f, 0.9f, 0.0f));
	womanCollider->Init();
	_woman->SetCollider(womanCollider);

	_camera = std::make_shared<Camera>();
	_camera->SetPosition(0.0f, 1.0f, -3.0f);
	GameTimer::GetInstance()->ToggleState();
	return true;
}

void Engine::Update()
{
	__super::Update();
	DirectX::XMVECTOR moveDirection = DirectX::XMVectorZero();
	DirectX::XMMATRIX invViewMat = DirectX::XMMatrixInverse(nullptr, _camera->GetViewMat());
	DirectX::XMVECTOR cameraRight = invViewMat.r[0];
	DirectX::XMVECTOR cameraForward = invViewMat.r[2];
	cameraRight = DirectX::XMVectorSetY(cameraRight, 0.0f);
	cameraForward = DirectX::XMVectorSetY(cameraForward, 0.0f);

	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(cameraRight)) > 0.0001f)
	{
		cameraRight = DirectX::XMVector3Normalize(cameraRight);
	}

	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(cameraForward)) > 0.0001f)
	{
		cameraForward = DirectX::XMVector3Normalize(cameraForward);
	}

	if (_keyboard->GetKey('A'))
	{
		moveDirection = DirectX::XMVectorSubtract(moveDirection, cameraRight);
		OutputDebugStringW(L"A\n");
	}
	if (_keyboard->GetKey('D'))
	{
		moveDirection = DirectX::XMVectorAdd(moveDirection, cameraRight);
		OutputDebugStringW(L"D\n");
	}
	if (_keyboard->GetKey('W'))
	{
		moveDirection = DirectX::XMVectorAdd(moveDirection, cameraForward);
		OutputDebugStringW(L"W\n");
	}
	if (_keyboard->GetKey('S'))
	{
		moveDirection = DirectX::XMVectorSubtract(moveDirection, cameraForward);
		OutputDebugStringW(L"S\n");
	}
	if (_keyboard->GetKeyDown(VK_ESCAPE))
	{
		GameTimer::GetInstance()->ToggleState();
		OutputDebugStringW(L"ESC\n");
	}
	if (_keyboard->GetKeyDown(VK_F1) && _character != nullptr)
	{
		_character->ToggleLightEnabled();
		if (_woman != nullptr)
		{
			_woman->SetLightEnabled(_character->IsLightEnabled());
		}
		OutputDebugStringW(_character->IsLightEnabled() ? L"Light On\n" : L"Light Off\n");
	}
	if (_keyboard->GetKeyDown(VK_F2) && _character != nullptr)
	{
		_character->ToggleColliderVisible();
		if (_woman != nullptr)
		{
			_woman->SetColliderVisible(_character->IsColliderVisible());
		}
		OutputDebugStringW(_character->IsColliderVisible() ? L"Collider On\n" : L"Collider Off\n");
	}

	if (_character != nullptr)
	{
		const bool isMoving = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(moveDirection)) > 0.0001f;
		if (isMoving)
		{
			moveDirection = DirectX::XMVector3Normalize(moveDirection);
			DirectX::XMFLOAT3 moveDirectionFloat;
			DirectX::XMStoreFloat3(&moveDirectionFloat, moveDirection);
			_character->Move(moveDirectionFloat, GameTimer::GetInstance()->GetDeltaTime());
			_character->PlayAnimation("Walk", 0.2f);
		}
		else
		{
			_character->PlayAnimation("Idle", 0.2f);
		}

		_character->Update(
			GameTimer::GetInstance()->GetDeltaTime(),
			_camera->GetViewMat(),
			_camera->GetProjMat());
	}

	if (_woman != nullptr)
	{
		_woman->PlayAnimation("Idle", 0.2f);
		_woman->Update(
			GameTimer::GetInstance()->GetDeltaTime(),
			_camera->GetViewMat(),
			_camera->GetProjMat());
	}
}

void Engine::Render()
{
	Graphic::GetInstance()->RenderBegin();
	if (_character != nullptr)
	{
		_character->Render(_rsState.Get(), _blendState.Get(), _samplerState.Get());
	}
	if (_woman != nullptr)
	{
		_woman->Render(_rsState.Get(), _blendState.Get(), _samplerState.Get());
	}

	Graphic::GetInstance()->RenderEnd();
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
