#include "PipelineFactory.h"

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, std::vector<D3D_SHADER_MACRO> defs)
	: shader_path(path), provider(prov), defines(defs) {
}

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, std::unordered_map<int, D3D_SHADER_MACRO*> defs)
	: shader_path(path), provider(prov), definesMap(defs) {
}

PipelineState* PipelineFactory::GetState(int defs) {
	PipelineState* ps;
	if (dict.count(defs)) {
		return dict.at(defs);
	}
	else {
		ps = new PipelineState();

		ps->vs->Create(CompileShader(shader_path.c_str(), definesMap.at(defs), "VSMain", "vs_5_0"));
		ps->ps->Create(CompileShader(shader_path.c_str(), definesMap.at(defs), "PSMain", "ps_5_0"));
		
		provider->PatchPipelineState(ps, defs);

		dict.insert({defs, ps});
		return ps;
	}
}