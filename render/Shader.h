#pragma once

#include "WndDxIncludes.h"

class Shader {
public:
	virtual void Create(Microsoft::WRL::ComPtr<ID3DBlob> blob) = 0;
};