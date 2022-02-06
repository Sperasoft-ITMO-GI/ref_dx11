#pragma once

#include <WndDxIncludes.h>

class Shader {
public:
	virtual void Create(ID3DBlob* blob) = 0;
};