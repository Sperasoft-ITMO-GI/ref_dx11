#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum DepthStencilState {
	NEVER,
	LESS,
	LESS_EQUAL
};

ID3D11DepthStencilState* MakeNeverDSS();
ID3D11DepthStencilState* MakeLessDSS();
ID3D11DepthStencilState* MakeLessEqualDSS();