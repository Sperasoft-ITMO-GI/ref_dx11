#include <PixelShader.h>

using Microsoft::WRL::ComPtr;

PixelShader::PixelShader() : shader(nullptr) {
}

void PixelShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->PSSetShader(shader.Get(), nullptr, 0u);
}

void PixelShader::Create(ComPtr<ID3DBlob> blob) {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
}


