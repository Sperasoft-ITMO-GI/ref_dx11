#pragma once

#include <unordered_map>
#include <string>

#include <WndDxIncludes.h>
#include <PipelineState.h>
#include <IStateProvider.h>

class PipelineFactory {
public:
	PipelineFactory(const std::wstring& path, IStateProvider* prov, std::unordered_map<int, ShaderOptions> defs);

	PipelineState* GetState(int defs, bool* error = nullptr);

private:
	std::wstring shader_path;
	IStateProvider* provider;
	std::unordered_map<int, ShaderOptions> def_to_sm;
	std::unordered_map<int, PipelineState*> def_to_ps;
};