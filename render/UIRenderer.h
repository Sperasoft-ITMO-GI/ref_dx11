#pragma once 

#include <vector>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <Quad.h>

#define COLORED  0x001
#define TEXTURED 0x002

class UIRenderer {
public:

	void Init();

	void Render();

	//void Swap();

	void AddElement(const Quad& quad); // должен принимать Quad или что то типо такого

private:
	class UIPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	std::vector<Quad> quads;
};