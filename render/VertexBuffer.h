#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Bindable.h>
#include <Renderer.h>
#include <Shader.h>
#include <UIVertex.h>
#include <BSPVertex.h>
#include <ModVertex.h>
#include <SkyVertex.h>
#include <BeamVertex.h>

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
	
	template<typename T>
	void CreateDynamic(const UINT size) {
		if (!buffer) {
			stride = sizeof(T);
			count = size;
			Renderer* renderer = Renderer::GetInstance();
			D3D11_BUFFER_DESC buffer_desc{};
			ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));

			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			buffer_desc.ByteWidth = stride * count;

			//D3D11_SUBRESOURCE_DATA subresource_data{};
			//ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

			//subresource_data.pSysMem = vertices.data();

			DXCHECK(renderer->GetDevice()->CreateBuffer(&buffer_desc, NULL, &buffer));
		}
	}

	template<typename T>
	void Update(std::vector<T> vertices) {
		Renderer* renderer = Renderer::GetInstance();
		ID3D11DeviceContext* context = renderer->GetContext();

		count = vertices.size();

		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		ZeroMemory(&mapped_subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DXCHECK(context->Map(buffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped_subresource));
		CopyMemory(mapped_subresource.pData, vertices.data(), stride * count);
		context->Unmap(buffer, 0u);
	}

	virtual void Bind() override;

	UINT GetCount();
private:
	UINT stride;
	UINT count;
	ID3D11Buffer* buffer;
	ID3D11VertexShader* shader;
};