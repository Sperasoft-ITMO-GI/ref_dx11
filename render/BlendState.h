#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	UIALPHABS,
	ALPHABS,
	SURFLIGHTMAPBS,
	NOBS
};

ID3D11BlendState* MakeUIAlphaBS();
ID3D11BlendState* MakeAlphaBS();
ID3D11BlendState* MakeSurfLightMapBS();
ID3D11BlendState* MakeNoBS();

