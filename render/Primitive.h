#pragma once

#include <ConstantBuffer.hpp>
#include <VertexBuffer.h>
#include <IndexBuffer.h>

class Primitive {
public:
	Primitive(bool is_indexed);
	Primitive(VertexBuffer& vb, IndexBuffer& ib, bool is_indexed);

	~Primitive();

	inline bool IsIndexed() { return is_indexed; }
	inline bool IsStatic()  { return is_static; }

protected:
	VertexBuffer dynamic_vb;
	IndexBuffer  dynamic_ib;

protected:
	bool is_indexed;
	bool is_static;
};