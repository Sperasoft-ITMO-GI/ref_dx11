#pragma once

#include <string>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Shader.h>
#include <Bindable.h>
#include <Renderer.h>

class ComputeShader : public Shader, public Bindable {
public:
	ComputeShader();

	~ComputeShader();

	virtual void Bind() override;

	virtual void Create(ID3DBlob* blob) override;

private:
	ID3D11ComputeShader* shader;
};