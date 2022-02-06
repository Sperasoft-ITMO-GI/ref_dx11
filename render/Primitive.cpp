#include <Primitive.h>

Primitive::Primitive(bool is_indexed)
:  
  is_indexed(is_indexed), is_static(true) {
}

Primitive::Primitive(VertexBuffer& vb, IndexBuffer& ib, int flags, int tex_index, int lm_index, bool is_indexed)
: dynamic_vb(vb), dynamic_ib(ib), flags(flags), texture_index(tex_index), 
  lightmap_index(lm_index), is_indexed(is_indexed), is_static(false) {
}

Primitive::~Primitive() {
	dynamic_vb.~VertexBuffer();
	dynamic_ib.~IndexBuffer();
}
