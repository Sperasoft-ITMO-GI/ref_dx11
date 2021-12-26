#pragma once

#include <unordered_map>
#include <string>

#include "WndDxIncludes.h"
#include "PipelineState.h"
#include "IStateProvider.h"

struct DefinePair {
	const char* name;
	int define;
};

class PipelineFactory {
public:
	PipelineFactory(const std::wstring& path, IStateProvider* prov, std::vector<D3D_SHADER_MACRO> defs);
	PipelineFactory(const std::wstring& path, IStateProvider* prov, std::unordered_map<int, D3D_SHADER_MACRO*> defs);

	PipelineState* GetState(int defs);
private:
	std::unordered_map<int, PipelineState*> dict;
	std::wstring shader_path;
	IStateProvider* provider;
	std::vector<D3D_SHADER_MACRO> defines; 
	std::unordered_map<int, D3D_SHADER_MACRO*> definesMap;
};