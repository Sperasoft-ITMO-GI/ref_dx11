#pragma once

#include <WndDxIncludes.h>
#include <Renderer.h>

enum BlendState {
	ALPHA,
	NOBS
};

Microsoft::WRL::ComPtr<ID3D11BlendState> MakeAlphaBS();
Microsoft::WRL::ComPtr<ID3D11BlendState> MakeNoBS();

