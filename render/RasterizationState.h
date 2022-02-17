#pragma once

#include <unordered_map>

#include <WndDxIncludes.h>
#include <Renderer.h>

enum RasterizationState {
	CULL_NONE,
	CULL_BACK,
	CULL_FRONT,
	WIREFRAME
};

ID3D11RasterizerState* MakeCullNoneRS();
ID3D11RasterizerState* MakeCullBackRS();
ID3D11RasterizerState* MakeCullFrontRS();

ID3D11RasterizerState* MakeWireframeRS();

