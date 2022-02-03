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
#define UI_FADE     0x004

class UIRenderer {
public:

	void Init();

	void Render();

	void AddElement(const Quad& quad); // ������ ��������� Quad ��� ��� �� ���� ������

private:
	class UIPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	std::vector<Quad> quads;
};