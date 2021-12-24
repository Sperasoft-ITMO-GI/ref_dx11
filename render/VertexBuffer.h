#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Bindable.h>
#include <Renderer.h>
#include <Shader.h>
#include <UIVertex.h>

class VertexBuffer : public Bindable {
public:
	template<class T>
	VertexBuffer(std::vector<T> vertices) : stride(sizeof(T)) {
		Renderer* renderer = Renderer::GetInstance();
		D3D11_BUFFER_DESC buffer_desc{};
		ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));

		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = stride * vertices.size();

		D3D11_SUBRESOURCE_DATA subresource_data{};
		ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

		subresource_data.pSysMem = vertices.data();

		DXCHECK(renderer->GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &buffer));
	}

	VertexBuffer(UIVertex vert[]);

	virtual void Bind() override;
private:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};