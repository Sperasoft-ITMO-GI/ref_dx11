#include <PixelShader.h>

PixelShader::PixelShader() : shader(nullptr) {
}

PixelShader::~PixelShader() {
	shader->Release();
}

void PixelShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->PSSetShader(shader, nullptr, 0u);
}

void PixelShader::Create(Microsoft::WRL::ComPtr<ID3DBlob> blob) {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
}
