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

	~PixelShader();

	virtual void Bind() override;

	virtual void Create(ID3DBlob* blob) override;

private:
	ID3D11PixelShader* shader;
};