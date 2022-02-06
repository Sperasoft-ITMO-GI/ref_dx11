#include <VertexShader.h>

using std::string;

VertexShader::VertexShader() : shader(nullptr) {
}

VertexShader::~VertexShader() {
	shader->Release();
	blob->Release();
}

void VertexShader::Create(ID3DBlob* b) {
	blob = b;
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
}

void VertexShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->VSSetShader(shader, nullptr, 0u);
}

ID3DBlob* VertexShader::GetBlob() {
	return blob;
}