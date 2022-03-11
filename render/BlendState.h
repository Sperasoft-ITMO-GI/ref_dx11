#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	UIALPHABS,
	ALPHABS,
	SURFLIGHTMAPBS,
	NOBS,
	EFFECTBS,
	SCREEN
};

ID3D11BlendState* MakeUIAlphaBS();
ID3D11BlendState* MakeAlphaBS();
ID3D11BlendState* MakeSurfLightMapBS();
ID3D11BlendState* MakeNoBS();
ID3D11BlendState* MakeEffectBS();
ID3D11BlendState* MakeScreenBS();

