#pragma once

#include <WndDxIncludes.h>

// Interface for all objects that we can to Bind to the Pipeline

class Bindable {
public:
	virtual void Bind() = 0;
};