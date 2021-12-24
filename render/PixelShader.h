#pragma once

#include <string>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Shader.h>
#include <Bindable.h>
#include <Renderer.h>

class PixelShader : public Shader,  public Bindable {
public:
	PixelShader();

	virtual void Bind() override;

	virtual void Create(Microsoft::WRL::ComPtr<ID3DBlob> blob) override;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};