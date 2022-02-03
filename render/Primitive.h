#pragma once

#include <ConstantBuffer.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>

class Primitive {
public:
	Primitive(int flags, int tex_index, int lm_index, bool is_indexed);
	Primitive(VertexBuffer& vb, IndexBuffer& ib, int flags, int tex_index, int lm_index, bool is_indexed);

	inline bool IsIndexed() { return is_indexed; }
	inline bool IsStatic()  { return is_static; }

protected:
	VertexBuffer dynamic_vb;
	IndexBuffer  dynamic_ib;

protected:
	int flags;
	int texture_index;
	int lightmap_index;
	bool is_indexed;
	bool is_static;
};