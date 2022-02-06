#include <VertexShader.h>

using std::string;

VertexShader::VertexShader() : shader(nullptr) {
}

VertexShader::~VertexShader() {
	shader->Release();
	blob->Release();
}

void VertexShader::Create(Microsoft::WRL::ComPtr<ID3DBlob> b) {
	b.CopyTo(&blob);
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
}

void VertexShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->VSSetShader(shader, nullptr, 0u);
}

Microsoft::WRL::ComPtr<ID3DBlob> VertexShader::GetBlob() {
	return blob;
}