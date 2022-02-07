#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Bindable.h>
#include <Renderer.h>

class IndexBuffer : public Bindable {
public:
	IndexBuffer();

	IndexBuffer(std::vector<uint16_t> indexes); // maybe uint16_t too small for store indixes;

	~IndexBuffer();

	void Create(std::vector<uint16_t> indexes);

	void CreateDynamic(const UINT size);

	void Update(std::vector<uint16_t> indexes);

	virtual void Bind() override;
	
	uint16_t GetCount();
private:
	uint16_t count;
	ID3D11Buffer* buffer;
};
