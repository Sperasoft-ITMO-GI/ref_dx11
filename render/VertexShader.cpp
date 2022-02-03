#include <VertexShader.h>

using Microsoft::WRL::ComPtr;
using std::string;

VertexShader::VertexShader() : shader(nullptr) {
}

void VertexShader::Create(ComPtr<ID3DBlob> b) {
	b.CopyTo(&blob);
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
}

void VertexShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->VSSetShader(shader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBlob() {
	return blob.Get();
}