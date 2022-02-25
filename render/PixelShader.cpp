#include <PixelShader.h>

PixelShader::PixelShader() : shader(nullptr) {
}

PixelShader::~PixelShader() {
	if (shader != nullptr)
		shader->Release();
}

void PixelShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->PSSetShader(shader, nullptr, 0u);
}

void PixelShader::Create(ID3DBlob* blob) {
	if (blob != nullptr) {
		Renderer* renderer = Renderer::GetInstance();
		renderer->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	}
}
