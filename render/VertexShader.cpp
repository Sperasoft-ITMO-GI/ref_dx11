#include <VertexShader.h>

using std::string;

VertexShader::VertexShader() : shader(nullptr) {
}

VertexShader::~VertexShader() {
	if (shader != nullptr)
		shader->Release();

	if (blob != nullptr)
		blob->Release();
}

void VertexShader::Create(ID3DBlob* in_blob) {
	if (in_blob != nullptr) {
		blob = in_blob;
		Renderer* renderer = Renderer::GetInstance();
		renderer->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	}
}

void VertexShader::Bind() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetContext()->VSSetShader(shader, nullptr, 0u);
}

ID3DBlob* VertexShader::GetBlob() {
	return blob;
}