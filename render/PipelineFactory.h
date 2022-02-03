#pragma once

#include <unordered_map>
#include <string>

#include <WndDxIncludes.h>
#include <PipelineState.h>
#include <IStateProvider.h>

class PipelineFactory {
public:
	PipelineFactory(const std::wstring& path, IStateProvider* prov, std::unordered_map<int, D3D_SHADER_MACRO*> defs);

	PipelineState* GetState(int defs);
private:
	std::wstring shader_path;
	IStateProvider* provider;
	std::unordered_map<int, D3D_SHADER_MACRO*> def_to_sm;
	std::unordered_map<int, PipelineState*> def_to_ps;
};