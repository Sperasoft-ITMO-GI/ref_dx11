#include <IndexBuffer.h>

IndexBuffer::IndexBuffer() : count(0), buffer(nullptr) {
}

IndexBuffer::IndexBuffer(std::vector<uint16_t> indexes) : count(indexes.size()) {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BUFFER_DESC index_buffer_desc{};
	ZeroMemory(&index_buffer_desc, sizeof(D3D11_BUFFER_DESC));

	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = sizeof(indexes) * count;

	D3D11_SUBRESOURCE_DATA index_subresource_data{};
	ZeroMemory(&index_subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

	index_subresource_data.pSysMem = indexes.data();

	DXCHECK(renderer->GetDevice()->CreateBuffer(&index_buffer_desc, &index_subresource_data, &buffer));
}

IndexBuffer::~IndexBuffer() {
	if (buffer)
		buffer->Release();
}

void IndexBuffer::Create(std::vector<uint16_t> indexes) {
	count = indexes.size();
	if (!buffer) {
		Renderer* renderer = Renderer::GetInstance();

		D3D11_BUFFER_DESC index_buffer_desc{};
		ZeroMemory(&index_buffer_desc, sizeof(D3D11_BUFFER_DESC));

		index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		index_buffer_desc.ByteWidth = sizeof(indexes) * count;

		D3D11_SUBRESOURCE_DATA index_subresource_data{};
		ZeroMemory(&index_subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

		index_subresource_data.pSysMem = indexes.data();

		DXCHECK(renderer->GetDevice()->CreateBuffer(&index_buffer_desc, &index_subresource_data, &buffer));
	}
}

void IndexBuffer::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	const UINT offset = 0u;
	renderer->GetContext()->IASetIndexBuffer(buffer, DXGI_FORMAT_R16_UINT, offset);
}

uint16_t IndexBuffer::GetCount() {
	return count;
}
