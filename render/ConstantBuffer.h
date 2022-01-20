#pragma once

#include <WndDxIncludes.h>
#include <Bindable.h>
#include <Renderer.h>

struct ConstantBufferQuad {
	DirectX::XMMATRIX position_transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX color_transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX texture_transform = DirectX::XMMatrixIdentity();
};

struct ConstantBufferPolygon {
	DirectX::XMMATRIX position_transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX texture_transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX lightmap_transform = DirectX::XMMatrixIdentity(); // ????
};

template<typename T>
class ConstantBuffer {
public:
	template<typename T>
	ConstantBuffer(const T& trans) : transfroms(trans) {
		Renderer* renderer = Renderer::GetInstance();

		D3D11_BUFFER_DESC constant_buffer_desc{ 0 };
		constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constant_buffer_desc.ByteWidth = sizeof(T);

		D3D11_SUBRESOURCE_DATA constant_subresource_data{};
		constant_subresource_data.pSysMem = &trans;

		renderer->GetDevice()->CreateBuffer(&constant_buffer_desc, &constant_subresource_data, &buffer);
	}

	template<typename T>
	void Update(const T& transforms) {
		Renderer* renderer = Renderer::GetInstance();
		
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = renderer->GetContext();

		T data = transforms;
		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		ZeroMemory(&mapped_subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DXCHECK(context->Map(buffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped_subresource));
		CopyMemory(mapped_subresource.pData, data, sizeof(T));
		context->Unmap(buffer.Get(), 0u);
	}

	template<typename T>
	void Bind() {
		Renderer* renderer = Renderer::GetInstance();
		renderer->GetContext()->VSSetConstantBuffers(0u, 1u, buffer.GetAddressOf());
	}
private:
	T transfroms;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};