#include <PipelineFactory.h>

PipelineFactory::PipelineFactory(const std::wstring& path, IStateProvider* prov, std::unordered_map<int, ShaderOptions> defs)
	: shader_path(path), provider(prov), def_to_sm(defs) {
}

PipelineState* PipelineFactory::GetState(int defs, bool* error)
{
	PipelineState* ps;
	if (def_to_ps.count(defs)) 
	{
		return def_to_ps.at(defs);
	}
	else 
	{
		bool error_ = false;

		ps = new PipelineState();

		ps->gs = new GeometryShader();
		ps->ps = new PixelShader();
		ps->vs = new VertexShader();

		if (def_to_sm.at(defs).entrys & CS_SHADER_ENTRY)
		{

		}
		if (def_to_sm.at(defs).entrys & DS_SHADER_ENTRY)
		{

		}
		if (def_to_sm.at(defs).entrys & GS_SHADER_ENTRY)
		{
			ID3DBlob* blob = CompileShader(shader_path.c_str(), def_to_sm.at(defs).mac, "GSMain", "gs_5_0");
			if (blob != nullptr)
				ps->gs->Create(blob);
			else
				error_ = true;
		}
		if (def_to_sm.at(defs).entrys & HS_SHADER_ENTRY)
		{

		}
		if (def_to_sm.at(defs).entrys & PS_SHADER_ENTRY)
		{
			ID3DBlob* blob = CompileShader(shader_path.c_str(), def_to_sm.at(defs).mac, "PSMain", "ps_5_0");
			if (blob != nullptr)
				ps->ps->Create(blob);
			else
				error_ = true;
		}
		if (def_to_sm.at(defs).entrys & VS_SHADER_ENTRY)
		{
			ID3DBlob* blob = CompileShader(shader_path.c_str(), def_to_sm.at(defs).mac, "VSMain", "vs_5_0");
			if (blob != nullptr)
				ps->vs->Create(blob);
			else
				error_ = true;
		}

		if (error_)
		{
			delete ps->gs;
			delete ps->ps;
			delete ps->vs;
			delete ps;

			if(error != nullptr)
				*error = error_;

			return nullptr;
		}

		//ps->vs->Create(CompileShader(shader_path.c_str(), def_to_sm.at(defs).mac, "VSMain", "vs_5_0"));
		//ps->gs->Create(CompileShader(shader_path.c_str(), def_to_sm.at(defs).mac, "GSMain", "gs_5_0"));
		//ps->ps->Create(CompileShader(shader_path.c_str(), def_to_sm.at(defs).mac, "PSMain", "ps_5_0"));
		
		provider->PatchPipelineState(ps, defs);

		def_to_ps.insert({defs, ps});
		return ps;
	}
}