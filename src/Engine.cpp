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
	CreateShadowResources();
	CreateMapObjects();

	_man = std::make_shared<GameObject>();
	_man->Init(
		"Data\\FBX\\Man\\Man.fbx",
		L"Data\\FBX\\Man\\Man_texture_0.png",
		L"Data\\FBX\\Man\\Man_normal.png",
		L"Data\\FBX\\Man\\Man_roughness.png",
		L"Data\\FBX\\Man\\Man_metallic.png");
	_man->SetPosition(_manFixedPosition);
	_man->AddAnimation("Idle", "Data\\FBX\\Man\\Animation\\Idle.fbx");
	_man->PlayAnimation("Idle", 0.0f);
	DirectionalLight characterLight;
	characterLight.SetDirection(0.3f, -1.0f, 0.3f);
	characterLight.SetColor(1.0f, 1.0f, 1.0f);
	characterLight.SetIntensity(1.0f);
	_mapLightData.directionalLightDirection = DirectX::XMFLOAT4(0.3f, -1.0f, 0.3f, 0.0f);
	_mapLightData.directionalLightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_mapLightData.ambientColor = DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	_mapLightData.useLight = 1;
	_mapLightConstantBuffer = std::make_shared<ConstantBuffer<LightData>>();
	_mapLightConstantBuffer->Create();
	_mapLightConstantBuffer->Update(&_mapLightData);
	_man->SetDirectionalLight(characterLight);
	std::shared_ptr<CapsuleCollider> collider = std::make_shared<CapsuleCollider>(0.35f, 1.8f);
	collider->SetCenter(DirectX::XMFLOAT3(0.0f, 0.9f, 0.0f));
	collider->Init();
	_man->SetCollider(collider);

	_character = std::make_shared<GameObject>();
	_character->Init(
		"Data\\FBX\\Woman\\Woman.fbx",
		L"Data\\FBX\\Woman\\Woman_texture.png",
		L"Data\\FBX\\Woman\\Woman_normal.png",
		L"Data\\FBX\\Woman\\Woman_roughness.png",
		L"Data\\FBX\\Woman\\Woman_metallic.png");
	_character->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	_character->AddAnimation("Idle", "Data\\FBX\\Woman\\Animation\\Idle.fbx");
	_character->AddAnimation("Walk", "Data\\FBX\\Woman\\Animation\\Walking.fbx");
	_character->AddAnimation("Punch1", "Data\\FBX\\Woman\\Animation\\Punch1.fbx");
	_character->AddAnimation("Punch2", "Data\\FBX\\Woman\\Animation\\Punch2.fbx");
	_character->PlayAnimation("Idle", 0.0f);
	_character->SetDirectionalLight(characterLight);
	std::shared_ptr<CapsuleCollider> womanCollider = std::make_shared<CapsuleCollider>(0.35f, 1.8f);
	womanCollider->SetCenter(DirectX::XMFLOAT3(0.0f, 0.9f, 0.0f));
	womanCollider->Init();
	_character->SetCollider(womanCollider);

	_camera = std::make_shared<Camera>();
	UpdateCamera();
	GameTimer::GetInstance()->ToggleState();
	return true;
}

void Engine::Update()
{
	__super::Update();
	DirectX::XMVECTOR moveDirection = DirectX::XMVectorZero();

	if (_keyboard->GetKey('A'))
	{
		moveDirection = DirectX::XMVectorAdd(moveDirection, DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
		OutputDebugStringW(L"A\n");
	}
	if (_keyboard->GetKey('D'))
	{
		moveDirection = DirectX::XMVectorAdd(moveDirection, DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
		OutputDebugStringW(L"D\n");
	}
	if (_keyboard->GetKey('W'))
	{
		moveDirection = DirectX::XMVectorAdd(moveDirection, DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
		OutputDebugStringW(L"W\n");
	}
	if (_keyboard->GetKey('S'))
	{
		moveDirection = DirectX::XMVectorAdd(moveDirection, DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
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
		if (_man != nullptr)
		{
			_man->SetLightEnabled(_character->IsLightEnabled());
		}
		_mapLightData.useLight = _character->IsLightEnabled() ? 1 : 0;
		if (_mapLightConstantBuffer != nullptr)
		{
			_mapLightConstantBuffer->Update(&_mapLightData);
		}
		OutputDebugStringW(_character->IsLightEnabled() ? L"Light On\n" : L"Light Off\n");
	}
	if (_keyboard->GetKeyDown(VK_F2) && _character != nullptr)
	{
		_character->ToggleColliderVisible();
		if (_man != nullptr)
		{
			_man->SetColliderVisible(_character->IsColliderVisible());
		}
		OutputDebugStringW(_character->IsColliderVisible() ? L"Collider On\n" : L"Collider Off\n");
	}

	if (_character != nullptr)
	{
		const bool isMoving = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(moveDirection)) > 0.0001f;
		if (isMoving)
		{
			const DirectX::XMFLOAT3 prevPosition = _character->GetPosition();
			moveDirection = DirectX::XMVector3Normalize(moveDirection);
			DirectX::XMFLOAT3 moveDirectionFloat;
			DirectX::XMStoreFloat3(&moveDirectionFloat, moveDirection);
			_character->Move(moveDirectionFloat, GameTimer::GetInstance()->GetDeltaTime());

			if (IsCharacterColliding())
			{
				ResolveCharacterCollision(prevPosition, moveDirectionFloat);
			}
		}

		UpdateCharacterAnimation(isMoving, GameTimer::GetInstance()->GetDeltaTime());
		UpdateCamera();
		_character->Update(
			GameTimer::GetInstance()->GetDeltaTime(),
			_camera->GetViewMat(),
			_camera->GetProjMat());
	}

	if (_man != nullptr)
	{
		_man->SetPosition(_manFixedPosition);
		_man->PlayAnimation("Idle", 0.2f);
		_man->Update(
			GameTimer::GetInstance()->GetDeltaTime(),
			_camera->GetViewMat(),
			_camera->GetProjMat());
	}
}

void Engine::Render()
{
	RenderShadowMap();
	Graphic::GetInstance()->RenderBegin();
	RenderMapObjects();

	if (_character != nullptr)
	{
		_character->Render(_rsState.Get(), _blendState.Get(), _samplerState.Get());
	}
	if (_man != nullptr)
	{
		_man->Render(_rsState.Get(), _blendState.Get(), _samplerState.Get());
	}

	Graphic::GetInstance()->RenderEnd();
}

void Engine::CreateShadowResources()
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = _shadowMapSize;
	textureDesc.Height = _shadowMapSize;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ComPtr<ID3D11Texture2D> shadowTexture;
	HRESULT hr = Graphic::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, shadowTexture.GetAddressOf());
	CHECK(hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = Graphic::GetInstance()->GetDevice()->CreateDepthStencilView(shadowTexture.Get(), &dsvDesc, _shadowDepthStencilView.GetAddressOf());
	CHECK(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	hr = Graphic::GetInstance()->GetDevice()->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, _shadowShaderResourceView.GetAddressOf());
	CHECK(hr);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Graphic::GetInstance()->GetDevice()->CreateSamplerState(&samplerDesc, _shadowSamplerState.GetAddressOf());
	CHECK(hr);

	_shadowVertexShader = std::make_shared<VertexShader>();
	_shadowVertexShader->Create(L"src\\HLSL\\ShadowAnim.hlsl", "VS", "vs_5_0");

	_shadowConstantBuffer = std::make_shared<ConstantBuffer<ShadowData>>();
	_shadowConstantBuffer->Create();
}

void Engine::UpdateShadowMatrix()
{
	const DirectX::XMFLOAT3 target = _character != nullptr
		? _character->GetPosition()
		: DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR targetVector = DirectX::XMVectorSet(target.x, target.y + 0.8f, target.z, 1.0f);
	DirectX::XMVECTOR lightDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.3f, -1.0f, 0.3f, 0.0f));
	DirectX::XMVECTOR lightPosition = DirectX::XMVectorSubtract(targetVector, DirectX::XMVectorScale(lightDirection, 8.0f));
	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	_lightViewMat = DirectX::XMMatrixLookAtLH(lightPosition, targetVector, upVector);
	_lightProjMat = DirectX::XMMatrixOrthographicLH(14.0f, 14.0f, 0.1f, 30.0f);
	_shadowData.lightViewProjMat = DirectX::XMMatrixTranspose(_lightViewMat * _lightProjMat);
	_shadowData.useShadow = 1;

	if (_shadowConstantBuffer != nullptr)
	{
		_shadowConstantBuffer->Update(&_shadowData);
	}
}

void Engine::RenderShadowMap()
{
	if (_shadowDepthStencilView == nullptr || _shadowVertexShader == nullptr)
	{
		return;
	}

	UpdateShadowMatrix();

	ID3D11ShaderResourceView* nullSRV = nullptr;
	Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, &nullSRV);
	Graphic::GetInstance()->GetDeviceContext()->OMSetRenderTargets(0, nullptr, _shadowDepthStencilView.Get());
	Graphic::GetInstance()->GetDeviceContext()->ClearDepthStencilView(_shadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<FLOAT>(_shadowMapSize);
	viewport.Height = static_cast<FLOAT>(_shadowMapSize);
	Graphic::GetInstance()->GetDeviceContext()->RSSetViewports(1, &viewport);
	Graphic::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (_character != nullptr)
	{
		_character->RenderShadow(_rsState.Get(), _shadowVertexShader.get(), _lightViewMat, _lightProjMat);
	}

	if (_man != nullptr)
	{
		_man->RenderShadow(_rsState.Get(), _shadowVertexShader.get(), _lightViewMat, _lightProjMat);
	}
}

void Engine::CreateMapObjects()
{
	AddMapObject(
		PrimitiveGeometry::CreatePlane(12.0f, 12.0f),
		DirectX::XMMatrixTranslation(0.0f, -0.02f, 0.0f));

	AddMapObject(
		PrimitiveGeometry::CreateCube(12.0f, 0.8f, 0.4f),
		DirectX::XMMatrixTranslation(0.0f, 0.4f, 5.8f),
		DirectX::XMFLOAT3(12.0f, 0.8f, 0.4f));

	AddMapObject(
		PrimitiveGeometry::CreateCube(12.0f, 0.8f, 0.4f),
		DirectX::XMMatrixTranslation(0.0f, 0.4f, -5.8f),
		DirectX::XMFLOAT3(12.0f, 0.8f, 0.4f));

	AddMapObject(
		PrimitiveGeometry::CreateCube(0.4f, 0.8f, 12.0f),
		DirectX::XMMatrixTranslation(5.8f, 0.4f, 0.0f),
		DirectX::XMFLOAT3(0.4f, 0.8f, 12.0f));

	AddMapObject(
		PrimitiveGeometry::CreateCube(0.4f, 0.8f, 12.0f),
		DirectX::XMMatrixTranslation(-5.8f, 0.4f, 0.0f),
		DirectX::XMFLOAT3(0.4f, 0.8f, 12.0f));

	AddMapObject(
		PrimitiveGeometry::CreateCube(1.2f, 0.6f, 1.2f),
		DirectX::XMMatrixTranslation(2.5f, 0.3f, 2.0f),
		DirectX::XMFLOAT3(1.2f, 0.6f, 1.2f));

	AddMapObject(
		PrimitiveGeometry::CreateCube(1.6f, 0.5f, 0.8f),
		DirectX::XMMatrixTranslation(-2.2f, 0.25f, 1.2f),
		DirectX::XMFLOAT3(1.6f, 0.5f, 0.8f));
}

void Engine::AddMapObject(const MeshData<VertexTexData>& meshData, const DirectX::XMMATRIX& worldMat)
{
	MapObject mapObject;
	mapObject.mesh = std::make_shared<StaticMesh>();
	mapObject.mesh->Init(meshData, DirectX::XMFLOAT4(0.38f, 0.42f, 0.38f, 1.0f));
	mapObject.constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	mapObject.constantBuffer->Create();
	mapObject.worldMat = worldMat;
	_mapObjects.push_back(mapObject);
}

void Engine::AddMapObject(const MeshData<VertexTexData>& meshData, const DirectX::XMMATRIX& worldMat, const DirectX::XMFLOAT3& colliderSize)
{
	MapObject mapObject;
	mapObject.mesh = std::make_shared<StaticMesh>();
	mapObject.mesh->Init(meshData, DirectX::XMFLOAT4(0.46f, 0.47f, 0.43f, 1.0f));
	mapObject.constantBuffer = std::make_shared<ConstantBuffer<TransformData>>();
	mapObject.constantBuffer->Create();
	mapObject.collider = std::make_shared<BoxCollider>(colliderSize);
	mapObject.collider->Init();
	mapObject.worldMat = worldMat;
	_mapObjects.push_back(mapObject);
}

void Engine::RenderMapObjects()
{
	for (MapObject& mapObject : _mapObjects)
	{
		if (mapObject.mesh == nullptr || mapObject.constantBuffer == nullptr || _camera == nullptr)
		{
			continue;
		}

		TransformData transformData;
		transformData.worldMat = DirectX::XMMatrixTranspose(mapObject.worldMat);
		transformData.viewMat = DirectX::XMMatrixTranspose(_camera->GetViewMat());
		transformData.projMat = DirectX::XMMatrixTranspose(_camera->GetProjMat());
		mapObject.constantBuffer->Update(&transformData);

		mapObject.mesh->Bind(_rsState.Get(), _samplerState.Get());
		Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 1, mapObject.constantBuffer->GetComPtr().GetAddressOf());
		if (_mapLightConstantBuffer != nullptr)
		{
			Graphic::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(1, 1, _mapLightConstantBuffer->GetComPtr().GetAddressOf());
		}
		if (_shadowConstantBuffer != nullptr)
		{
			Graphic::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(2, 1, _shadowConstantBuffer->GetComPtr().GetAddressOf());
			Graphic::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(2, 1, _shadowConstantBuffer->GetComPtr().GetAddressOf());
		}
		if (_shadowShaderResourceView != nullptr)
		{
			Graphic::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, _shadowShaderResourceView.GetAddressOf());
		}
		if (_shadowSamplerState != nullptr)
		{
			Graphic::GetInstance()->GetDeviceContext()->PSSetSamplers(1, 1, _shadowSamplerState.GetAddressOf());
		}
		mapObject.mesh->Draw();
	}
}

bool Engine::IsCharacterColliding() const
{
	if (_character == nullptr || _character->GetCollider() == nullptr)
	{
		return false;
	}

	if (_man != nullptr && _man->GetCollider() != nullptr &&
		IsCollidingWithObject(*_man->GetCollider(), _man->GetWorldMatrix()))
	{
		return true;
	}

	for (const MapObject& mapObject : _mapObjects)
	{
		if (mapObject.collider != nullptr &&
			IsCollidingWithObject(*mapObject.collider, mapObject.worldMat))
		{
			return true;
		}
	}

	return false;
}

bool Engine::IsCollidingWithObject(const Collider& collider, const DirectX::XMMATRIX& worldMat) const
{
	const std::shared_ptr<Collider> characterCollider = _character->GetCollider();
	if (characterCollider == nullptr)
	{
		return false;
	}

	const ColliderAABB characterAABB = characterCollider->GetWorldAABB(_character->GetWorldMatrix());
	const ColliderAABB objectAABB = collider.GetWorldAABB(worldMat);
	return Collider::Intersects(characterAABB, objectAABB);
}

void Engine::ResolveCharacterCollision(const DirectX::XMFLOAT3& prevPosition, const DirectX::XMFLOAT3& moveDirection)
{
	if (_character == nullptr)
	{
		return;
	}

	constexpr float pushBackDistance = 0.08f;
	DirectX::XMFLOAT3 pushedPosition = prevPosition;
	pushedPosition.x -= moveDirection.x * pushBackDistance;
	pushedPosition.y -= moveDirection.y * pushBackDistance;
	pushedPosition.z -= moveDirection.z * pushBackDistance;

	_character->SetPosition(pushedPosition);

	if (IsCharacterColliding())
	{
		_character->SetPosition(prevPosition);
	}
}

void Engine::UpdateCamera()
{
	if (_camera == nullptr || _character == nullptr)
	{
		return;
	}

	const DirectX::XMFLOAT3& characterPosition = _character->GetPosition();
	const DirectX::XMFLOAT3 target(
		characterPosition.x,
		characterPosition.y + 0.8f,
		characterPosition.z);
	const DirectX::XMFLOAT3 cameraPosition(
		characterPosition.x,
		characterPosition.y + 7.0f,
		characterPosition.z - 5.0f);

	_camera->LookAt(cameraPosition, target);
}

void Engine::UpdateCharacterAnimation(bool isMoving, float deltaTime)
{
	if (_character == nullptr)
	{
		return;
	}

	if (_attackState != eAttackState::None)
	{
		_attackElapsed += deltaTime;
	}

	if (_keyboard->GetKeyDown('I'))
	{
		if (_attackState == eAttackState::Punch1 && _attackElapsed >= _punchExtendTime)
		{
			_character->PlayAnimation("Punch2", 0.1f, true);
			_attackState = eAttackState::Punch2;
			_attackElapsed = 0.0f;
		}
		else if (_attackState == eAttackState::Punch2 && _attackElapsed >= _punchExtendTime)
		{
			_character->PlayAnimation("Punch1", 0.1f, true);
			_attackState = eAttackState::Punch1;
			_attackElapsed = 0.0f;
		}
		else if (_attackState == eAttackState::None)
		{
			_character->PlayAnimation("Punch1", 0.1f, true);
			_attackState = eAttackState::Punch1;
			_attackElapsed = 0.0f;
		}
	}

	if (_attackState == eAttackState::Punch1)
	{
		const double duration = _character->GetAnimationDurationSeconds("Punch1");
		if (duration > 0.0 && _attackElapsed >= static_cast<float>(duration))
		{
			_attackState = eAttackState::None;
			_attackElapsed = 0.0f;
		}
	}
	else if (_attackState == eAttackState::Punch2)
	{
		const double duration = _character->GetAnimationDurationSeconds("Punch2");
		if (duration > 0.0 && _attackElapsed >= static_cast<float>(duration))
		{
			_attackState = eAttackState::None;
			_attackElapsed = 0.0f;
		}
	}

	if (_attackState != eAttackState::None)
	{
		return;
	}

	_character->PlayAnimation(isMoving ? "Walk" : "Idle", 0.2f);
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
