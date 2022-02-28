#pragma once

#include <WndDxIncludes.h>
#include <Bindable.h>
#include <Renderer.h>

//struct ConstantBufferQuad {
//	DirectX::XMMATRIX position_transform = DirectX::XMMatrixIdentity();
//	DirectX::XMMATRIX texture_transform = DirectX::XMMatrixIdentity();
//	float color[4];
//};

//struct ConstantBufferPolygon {
//	DirectX::XMMATRIX position_transform = DirectX::XMMatrixIdentity();
//	float color[4];
//};

//struct ConstantBufferSkyBox {
//	DirectX::XMMATRIX position_transform = DirectX::XMMatrixIdentity();
//};

//struct ConstantBufferParticles {
//	DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
//	DirectX::XMMATRIX projection = DirectX::XMMatrixIdentity();
//};

//struct ConstantBufferEffects {
//	DirectX::XMMATRIX position_transform = DirectX::XMMatrixIdentity();
//	float color[4];
//};

template<typename T>
class ConstantBuffer {
public:

	ConstantBuffer() :transforms(T()), buffer(nullptr){
	}

	template<typename T>
	ConstantBuffer(const T& trans) : transforms(trans) {
		if (!buffer) {
			Renderer* renderer = Renderer::GetInstance();

			D3D11_BUFFER_DESC constant_buffer_desc{ 0 };
			constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constant_buffer_desc.ByteWidth = sizeof(T);

			D3D11_SUBRESOURCE_DATA constant_subresource_data{};
			constant_subresource_data.pSysMem = &transforms;

			renderer->GetDevice()->CreateBuffer(&constant_buffer_desc, &constant_subresource_data, &buffer);
		}
	}	
	
	template<typename T>
	void Create(const T& trans) {
		if (!buffer) {
			transforms = trans;
			Renderer* renderer = Renderer::GetInstance();

			D3D11_BUFFER_DESC constant_buffer_desc{ 0 };
			constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constant_buffer_desc.ByteWidth = sizeof(T);

			D3D11_SUBRESOURCE_DATA constant_subresource_data{};
			constant_subresource_data.pSysMem = &transforms;

			renderer->GetDevice()->CreateBuffer(&constant_buffer_desc, &constant_subresource_data, &buffer);
		}
	}
	
	~ConstantBuffer() {
		if (buffer)
			buffer->Release();
	}

	template<typename T>
	void Update(const T& trans) {
		Renderer* renderer = Renderer::GetInstance();
		
		ID3D11DeviceContext* context = renderer->GetContext();

		transforms = trans;
		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		ZeroMemory(&mapped_subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DXCHECK(context->Map(buffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped_subresource));
		CopyMemory(mapped_subresource.pData, &transforms, sizeof(T));
		context->Unmap(buffer, 0u);
	}

	template<typename T>
	void Bind(UINT slot) {
		Renderer* renderer = Renderer::GetInstance();
		renderer->GetContext()->VSSetConstantBuffers(slot, 1u, &buffer);
		renderer->GetContext()->GSSetConstantBuffers(slot, 1u, &buffer);
		renderer->GetContext()->PSSetConstantBuffers(slot, 1u, &buffer);
	}

	ID3D11Buffer* GetBuffer() {
		return buffer;
	}

private:
	T transforms;
	ID3D11Buffer* buffer;
};