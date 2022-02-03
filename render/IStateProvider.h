#pragma once

#include <PipelineState.h>

class IStateProvider {
public:
	virtual void PatchPipelineState(PipelineState* state, int defines) = 0;
};