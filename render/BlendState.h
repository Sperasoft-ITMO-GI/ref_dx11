#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	SIMPLEALPHA,
	ALPHA,
	NOBS
};

ID3D11BlendState* MakeSimpleAlphaBS();
ID3D11BlendState* MakeAlphaBS();
ID3D11BlendState* MakeNoBS();

