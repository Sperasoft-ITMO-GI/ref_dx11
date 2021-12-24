#pragma once

#include <string>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Bindable.h>
#include <Shader.h>
#include <Renderer.h>

class VertexShader : public Shader, public Bindable {
public:
	VertexShader();

	virtual void Create(Microsoft::WRL::ComPtr<ID3DBlob> blob) override;

	virtual void Bind() override;

	ID3DBlob* GetBlob();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
};