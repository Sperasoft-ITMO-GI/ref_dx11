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

	~VertexShader();

	virtual void Create(ID3DBlob* blob) override;

	virtual void Bind() override;

	ID3DBlob* GetBlob();

private:
	ID3D11VertexShader* shader;
	ID3DBlob* blob;
};