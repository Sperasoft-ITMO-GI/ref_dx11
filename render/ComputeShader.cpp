#include <ComputeShader.h>

ComputeShader::ComputeShader() : shader(nullptr) {
}

ComputeShader::~ComputeShader() {
	if(shader != nullptr)
		shader->Release();
}

void ComputeShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->CSSetShader(shader, nullptr, 0u);
}

void ComputeShader::Create(ID3DBlob* blob) {
	if (blob != nullptr) {
		Renderer* renderer = Renderer::GetInstance();
		renderer->GetDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	}
}
