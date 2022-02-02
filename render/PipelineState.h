#pragma once

#include "WndDxIncludes.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "RasterizationState.h"
#include "BlendState.h"
#include <InputLayout.h>
#include <Topology.h>
#include <States.h>

struct PipelineState {
	VertexShader* vs = new VertexShader();
	PixelShader* ps = new PixelShader();
	BlendState bs;
	RasterizationState rs;
	Layout layout;
	Topology topology;
};

void SetPipelineState(PipelineState* state);
