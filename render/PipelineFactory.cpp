#include "PipelineFactory.h"

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, std::vector<D3D_SHADER_MACRO> defs)
	: shader_path(path), provider(prov), defines(defs) {
}

PipelineState* PipelineFactory::GetState(int defs) {
	PipelineState* ps;
	if (dict.count(defs)) {
		return dict.at(defs);
	}
	else {
		ps = new PipelineState();

		ps->ps->Create(CompileShader(shader_path.c_str(), defines.data(), "PSMain", "ps_5_0"));
		ps->vs->Create(CompileShader(shader_path.c_str(), defines.data(), "VSMain", "vs_5_0"));

		provider->PatchPipelineState(ps, defs);

		dict.insert({defs, ps});
		return ps;
	}
}