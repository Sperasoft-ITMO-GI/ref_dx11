#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Bindable.h>
#include <Renderer.h>

class IndexBuffer : public Bindable {
public:
	IndexBuffer();

	IndexBuffer(std::vector<UINT> indexes); // maybe UINT too small for store indixes;

	~IndexBuffer();

	void Create(std::vector<UINT> indexes);

	void CreateDynamic(const UINT size);

	void Update(std::vector<UINT> indexes);

	virtual void Bind() override;
	
	UINT GetCount();
private:
	UINT count;
	ID3D11Buffer* buffer;
};
