#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <Quad.h>
#include <Utils.hpp>

#define UI_COLORED  0x001
#define UI_TEXTURED 0x002

struct QuadDefinitions {
	ConstantBufferQuad cbq;
	int flags;
	int teture_index;
};

class UIRenderer {
public:

	~UIRenderer();

	void Init();

	void Render();

	void Add(const QuadDefinitions& qd);

private:
	class UIPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	Quad* quad;
	std::vector<QuadDefinitions> qds;
};