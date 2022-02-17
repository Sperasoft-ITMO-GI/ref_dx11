#include <PipelineState.h>

using Microsoft::WRL::ComPtr;

void SetPipelineState(PipelineState* state) {
	Renderer* renderer = Renderer::GetInstance();
	States* states = States::GetInstance();
	ID3D11DeviceContext* context = renderer->GetContext();

	state->vs->Bind();
	state->gs->Bind();
	state->ps->Bind();
	
	// Кажется, что они не всегда такие
	float blend_factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sample_mask = 0xffffffff;
	context->OMSetBlendState(state->bs, blend_factor, sample_mask);
	context->RSSetState(state->rs);
	context->IASetInputLayout(state->layout);
	context->IASetPrimitiveTopology(state->topology);
}