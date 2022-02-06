#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Bindable.h>
#include <Renderer.h>
#include <Shader.h>
#include <UIVertex.h>
#include <BSPVertex.h>
#include <SkyVertex.h>

class VertexBuffer : public Bindable {
public:

	VertexBuffer();

	~VertexBuffer();

	template<typename T>
	VertexBuffer(std::vector<T> vertices) : stride(sizeof(T)), count(vertices.size()) {
		Renderer* renderer = Renderer::GetInstance();
		D3D11_BUFFER_DESC buffer_desc{};
		ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));

		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = stride * count;

		D3D11_SUBRESOURCE_DATA subresource_data{};
		ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

		subresource_data.pSysMem = vertices.data();

		DXCHECK(renderer->GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &buffer));
	}

	VertexBuffer(UIVertex vert[]);

	template<typename T>
	void Create(std::vector<T> vertices) {
		if (!buffer) {
			stride = sizeof(T);
			count = vertices.size();
			Renderer* renderer = Renderer::GetInstance();
			D3D11_BUFFER_DESC buffer_desc{};
			ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));

			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.ByteWidth = stride * count;

			D3D11_SUBRESOURCE_DATA subresource_data{};
			ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

			subresource_data.pSysMem = vertices.data();

			DXCHECK(renderer->GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &buffer));
		}
	}

	virtual void Bind() override;

	UINT GetCount();
private:
	UINT stride;
	UINT count;
	ID3D11Buffer* buffer;
	ID3D11VertexShader* shader;
};