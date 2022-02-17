#pragma once

#include <string>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Shader.h>
#include <Bindable.h>
#include <Renderer.h>

class GeometryShader : public Shader, public Bindable {
public:
	GeometryShader();

	~GeometryShader();

	virtual void Bind() override;

	virtual void Create(ID3DBlob* blob) override;

private:
	ID3D11GeometryShader* shader;
};