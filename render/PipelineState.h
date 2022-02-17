#pragma once

#include <WndDxIncludes.h>
#include <VertexShader.h>
#include <GeometryShader.h>
#include <PixelShader.h>
#include <RasterizationState.h>
#include <BlendState.h>
#include <InputLayout.h>
#include <Topology.h>
#include <States.h>

struct PipelineState {
	VertexShader* vs = nullptr;
	GeometryShader* gs = nullptr;
	PixelShader* ps = nullptr;
	ID3D11BlendState* bs = nullptr;
	ID3D11RasterizerState* rs = nullptr;
	ID3D11InputLayout* layout = nullptr;
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

void SetPipelineState(PipelineState* state);
