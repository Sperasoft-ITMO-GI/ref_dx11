#pragma once

#include <vector>

#include "WndDxIncludes.h"
#include "Utils.h"
#include "Bindable.h"
#include <Renderer.h>

class IndexBuffer : public Bindable {
public:
	IndexBuffer(std::vector<uint16_t> indexes); // maybe uint16_t too small for store indixes;

	virtual void Bind() override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};
