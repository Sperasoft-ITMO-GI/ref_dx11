#include <GeometryShader.h>

GeometryShader::GeometryShader() : shader(nullptr) {
}

GeometryShader::~GeometryShader() {
	if(shader != nullptr)
		shader->Release();
}

void GeometryShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->GSSetShader(shader, nullptr, 0u);
}

void GeometryShader::Create(ID3DBlob* blob) {
	if (blob != nullptr) {
		Renderer* renderer = Renderer::GetInstance();
		renderer->GetDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	}
}
