#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	ALPHA,
	NOALPHA
};

Microsoft::WRL::ComPtr<ID3D11BlendState> MakeAlphaBS();
Microsoft::WRL::ComPtr<ID3D11BlendState> MakeNoAlphaBS();

