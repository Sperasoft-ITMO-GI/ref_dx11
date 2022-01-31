#include <PipelineState.h>

using Microsoft::WRL::ComPtr;

void SetPipelineState(PipelineState* state) {
	Renderer* renderer = Renderer::GetInstance();
	States* states = States::GetInstance();
	ComPtr<ID3D11Device> device = renderer->GetDevice();
	ComPtr<ID3D11DeviceContext> context = renderer->GetContext();

	state->vs->Bind();
	state->ps->Bind();

	// Кажется, что они не всегда такие
	float blend_factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sample_mask = 0xffffffff;
	context->OMSetBlendState(states->blend_states.at(state->bs).Get(), blend_factor, sample_mask);

	context->RSSetState(states->rasterization_states.at(state->rs).Get());

	context->IASetInputLayout(MakeLayout(state->vs->GetBlob(), states->input_layouts.at(state->layout)).Get());
	context->IASetPrimitiveTopology(states->topology.at(state->topology));
}