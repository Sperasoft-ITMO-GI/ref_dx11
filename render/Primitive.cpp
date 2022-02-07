#include <Primitive.h>

Primitive::Primitive(bool is_indexed)
:  
  is_indexed(is_indexed), is_static(true) {
}

Primitive::Primitive(VertexBuffer& vb, IndexBuffer& ib,  bool is_indexed)
: dynamic_vb(vb), dynamic_ib(ib), is_indexed(is_indexed), is_static(false) {
}

Primitive::~Primitive() {
	dynamic_vb.~VertexBuffer();
	dynamic_ib.~IndexBuffer();
}


