#include <IndexBuffer.h>

IndexBuffer::IndexBuffer() : count(0), buffer(nullptr) {
}

IndexBuffer::IndexBuffer(std::vector<uint16_t> indexes) : count(indexes.size()) {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BUFFER_DESC index_buffer_desc{};
	ZeroMemory(&index_buffer_desc, sizeof(D3D11_BUFFER_DESC));

	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = sizeof(count) * count;

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
		index_buffer_desc.ByteWidth = sizeof(count) * count;

		D3D11_SUBRESOURCE_DATA index_subresource_data{};
		ZeroMemory(&index_subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

		index_subresource_data.pSysMem = indexes.data();

		DXCHECK(renderer->GetDevice()->CreateBuffer(&index_buffer_desc, &index_subresource_data, &buffer));
	}
}

void IndexBuffer::CreateDynamic(const UINT size) {
	if (!buffer) {
		count = size;
		Renderer* renderer = Renderer::GetInstance();
		D3D11_BUFFER_DESC buffer_desc{};
		ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));

		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.ByteWidth = sizeof(count) * count;

		//D3D11_SUBRESOURCE_DATA subresource_data{};
		//ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));

		//subresource_data.pSysMem = vertices.data();

		DXCHECK(renderer->GetDevice()->CreateBuffer(&buffer_desc, NULL, &buffer));
	}
}

void IndexBuffer::Update(std::vector<uint16_t> indexes) {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11DeviceContext* context = renderer->GetContext();

	count = indexes.size();

	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	ZeroMemory(&mapped_subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	DXCHECK(context->Map(buffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped_subresource));
	CopyMemory(mapped_subresource.pData, indexes.data(), sizeof(count) * count);
	context->Unmap(buffer, 0u);
}

void IndexBuffer::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	const UINT offset = 0u;
	renderer->GetContext()->IASetIndexBuffer(buffer, DXGI_FORMAT_R16_UINT, offset);
}

uint16_t IndexBuffer::GetCount() {
	return count;
}
