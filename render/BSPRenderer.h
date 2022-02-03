#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <BSPPoly.h>
#include <Utils.hpp>

// There should be defines
#define BSP_SOLID  0x001

class BSPRenderer {
public:

	void Init();

	void Render();

	void AddElement(const BSPPoly& polygon);

private:
	class ModelPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	std::vector<BSPPoly> polygons;
};