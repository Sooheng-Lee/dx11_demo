#pragma once
#include "Client.h"
class Engine : public Client
{
public:
	Engine();
	~Engine();
	bool Init();

protected:
	virtual void Update() override;
	virtual void Render() override;

private:
	void CreateRSState();
	void CreateBlendState();
	void CreateSamplerState();

private:
	ComPtr<ID3D11RasterizerState> _rsState;
	ComPtr<ID3D11BlendState> _blendState;
	ComPtr<ID3D11SamplerState> _samplerState;

	std::shared_ptr<Camera> _camera;
	std::shared_ptr<GameObject> _character;
	std::shared_ptr<GameObject> _woman;
};

