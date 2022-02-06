#include <PipelineFactory.h>

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, std::unordered_map<int, D3D_SHADER_MACRO*> defs)
	: shader_path(path), provider(prov), def_to_sm(defs) {
}

PipelineState* PipelineFactory::GetState(int defs) 
{
	PipelineState* ps;
	if (def_to_ps.count(defs)) 
	{
		return def_to_ps.at(defs);
	}
	else 
	{
		ps = new PipelineState();
		ps->vs = new VertexShader();
		ps->ps = new PixelShader();

		ps->vs->Create(CompileShader(shader_path.c_str(), def_to_sm.at(defs), "VSMain", "vs_5_0"));
		ps->ps->Create(CompileShader(shader_path.c_str(), def_to_sm.at(defs), "PSMain", "ps_5_0"));
		
		provider->PatchPipelineState(ps, defs);

		def_to_ps.insert({defs, ps});
		return ps;
	}
}