#include "PipelineFactory.h"

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, std::vector<D3D_SHADER_MACRO> defs)
	: shader_path(path), provider(prov), defines(defs) {
}

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, D3D_SHADER_MACRO defs[])
	: shader_path(path), provider(prov), definesArr(defs) {
}

PipelineState* PipelineFactory::GetState(int defs) {
	PipelineState* ps;
	if (dict.count(defs)) {
		return dict.at(defs);
	}
	else {
		ps = new PipelineState();

		definesArr[defs - 1].Definition = "1";

		ps->vs->Create(CompileShader(shader_path.c_str(), definesArr, "VSMain", "vs_5_0"));
		ps->ps->Create(CompileShader(shader_path.c_str(), definesArr, "PSMain", "ps_5_0"));

		definesArr[defs - 1].Definition = "0";
		
		provider->PatchPipelineState(ps, defs);

		dict.insert({defs, ps});
		return ps;
	}
}