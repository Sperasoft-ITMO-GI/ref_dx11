#include <InputLayout.h>

using std::vector;

ID3D11InputLayout* MakeLayout(Microsoft::WRL::ComPtr<ID3DBlob> blob, vector<D3D11_INPUT_ELEMENT_DESC> desc) {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11InputLayout* input_layout;

	DXCHECK(renderer->GetDevice()->CreateInputLayout(
		desc.data(), desc.size(),
		blob->GetBufferPointer(), blob->GetBufferSize(),
		&input_layout
	));

	return input_layout;
}

