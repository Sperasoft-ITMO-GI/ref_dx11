#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <BSPPoly.h>

// There should be defines
#define COLORED  0x001
#define TEXTURED 0x002
#define FADE     0x004

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