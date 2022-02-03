#include <VertexBuffer.h>

using std::vector;
using Microsoft::WRL::ComPtr;

VertexBuffer::VertexBuffer() : stride(0), count(0), buffer(nullptr), shader(nullptr) {
}

VertexBuffer::VertexBuffer(UIVertex vertices[]) : stride(sizeof(UIVertex)) {
	Renderer* renderer = Renderer::GetInstance();
	D3D11_BUFFER_DESC buffer_desc{};
	ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));

	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.ByteWidth = sizeof(vertices);

	D3D11_SUBRESOURCE_DATA subresource_data{};
	ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

	subresource_data.pSysMem = vertices;

	DXCHECK(renderer->GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &buffer));
}

void VertexBuffer::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	const UINT offset = 0u;
	renderer->GetContext()->IASetVertexBuffers(0u, 1u, buffer.GetAddressOf(), &stride, &offset);
}

UINT VertexBuffer::GetCount() {
	return count;
}
