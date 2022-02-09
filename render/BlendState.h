#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	UIALPHABS,
	ALPHABS,
	WATERBS,
	SURFLIGHTMAPBS,
	NOBS
};

ID3D11BlendState* MakeUIAlphaBS();
ID3D11BlendState* MakeAlphaBS();
ID3D11BlendState* MakeWaterBS();
ID3D11BlendState* MakeSurfLightMapBS();
ID3D11BlendState* MakeNoBS();

