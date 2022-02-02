#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <Quad.h>

#define DEFAULT  0x001

class SkyRenderer {
public:

	void Init();

	void Render();

	//void Swap();

	void AddElement(const Quad& quad); // ������ ��������� Quad ��� ��� �� ���� ������

private:
	class SkyPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	std::vector<Quad> quads;
};