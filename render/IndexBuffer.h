#pragma once

#include <vector>

#include "WndDxIncludes.h"
#include "Utils.h"
#include "Bindable.h"
#include <Renderer.h>

class IndexBuffer : public Bindable {
public:
	IndexBuffer() {
	}

	IndexBuffer(std::vector<uint16_t> indexes); // maybe uint16_t too small for store indixes;

	void Create(std::vector<uint16_t> indexes);

	virtual void Bind() override;
	
	int GetCount();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	uint16_t count;
};
