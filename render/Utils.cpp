#include <Utils.h>

#pragma comment(lib,"d3dcompiler.lib")

using Microsoft::WRL::ComPtr;
using std::wstring;
using std::string;

wstring ToWide(const string& narrow) {
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE); // string will be truncate if its bigger than 512 symbols
	return wide;
};

ID3DBlob* CompileShader(
	const wchar_t* filename,
	const D3D_SHADER_MACRO* defines,
	const char* entrypoint,
	const char* target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

#else
	compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* byteCode;
	ID3DBlob* errors;

	D3DCompileFromFile(filename, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint, target, compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		printf("[DX11][ERROR]:%s", (char*)errors->GetBufferPointer());

	return byteCode;
};

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync) {
	DXGI_RATIONAL refreshRate = { 0, 1 };
	if (vsync)
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		DXGI_MODE_DESC* displayModeList;

		// Create a DirectX graphics interface factory.
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Could not create DXGIFactory instance."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to create DXGIFactory.");
		}

		hr = factory->EnumAdapters(0, &adapter);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapters.");
		}

		hr = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapter outputs.");
		}

		UINT numDisplayModes;
		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		displayModeList = new DXGI_MODE_DESC[numDisplayModes];
		assert(displayModeList);

		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		// Now store the refresh rate of the monitor that matches the width and height of the requested screen.
		for (UINT i = 0; i < numDisplayModes; ++i)
		{
			printf("Widht: %d, Height: %d, hz: %d\n", displayModeList[i].Width, displayModeList[i].Height,
				(displayModeList[i].RefreshRate.Numerator / displayModeList[i].RefreshRate.Denominator));
			if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
			{
				refreshRate = displayModeList[i].RefreshRate;
			}
		}

		delete[] displayModeList;
		ReleaseCOM(adapterOutput);
		ReleaseCOM(adapter);
		ReleaseCOM(factory);
	}

	return refreshRate;
};

void SmartTriangulation(std::vector<uint16_t>* ind, int num, int offset)
{
	for (int i = offset; i < (num - 1) + offset; i++)
	{
		ind->push_back(offset);
		ind->push_back(i + 1);
		ind->push_back(i);
	}
}

void SmartTriangulationClockwise(std::vector<uint16_t>* ind, int num, int offset)
{
	for (int i = offset + 1; i < (num - 1) + offset; i++)
	{
		ind->push_back(offset);
		ind->push_back(i);
		ind->push_back(i + 1);
	}
}

void TriangulationTriangleStripToListClockwise(std::vector<uint16_t>* ind, int num, int offset)
{
	for (int i = 0; i < (num - 2); i++)
	{
		if (i % 2 == 0)
		{
			ind->push_back(i + offset);
			ind->push_back(i + 1 + offset);
			ind->push_back(i + 2 + offset);
		}
		else
		{
			ind->push_back(i + offset);
			ind->push_back(i + 2 + offset);
			ind->push_back(i + 1 + offset);
		}
	}
}

void TriangulationTriangleStripToList(std::vector<uint16_t>* ind, int num, int offset)
{
	for (int i = 0; i < (num - 2); i++)
	{
		if (i % 2 != 0)
		{
			ind->push_back(i + offset);
			ind->push_back(i + 1 + offset);
			ind->push_back(i + 2 + offset);
		}
		else
		{
			ind->push_back(i + offset);
			ind->push_back(i + 2 + offset);
			ind->push_back(i + 1 + offset);
		}
	}
}



