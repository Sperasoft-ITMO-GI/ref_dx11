#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	ALPHA,
	NOBS
};

ID3D11BlendState* MakeAlphaBS();
ID3D11BlendState* MakeNoBS();

