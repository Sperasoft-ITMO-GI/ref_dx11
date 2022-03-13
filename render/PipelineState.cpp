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
	float blend_factor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	UINT sample_mask = 0xffffffff;
	context->OMSetBlendState(state->bs, blend_factor, sample_mask);
	context->RSSetState(state->rs);
	context->OMSetDepthStencilState(state->dss, 1u);
	context->IASetInputLayout(state->layout);
	context->IASetPrimitiveTopology(state->topology);
}