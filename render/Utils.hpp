#pragma once

#include <type_traits>

#include <Quad.h>
#include <BSPPoly.h>

template<typename T, typename... Types>
constexpr bool is_any_of = std::disjunction_v<std::is_same<T, Types>...>;

template<typename T>
std::enable_if_t<is_any_of<T, Quad, BSPPoly>>
Draw(T& primitive) {
	if (primitive.IsStatic()) {
		primitive.DrawStatic();
	} else if (primitive.IsIndexed()){
		primitive.DrawIndexed();
	} else {
		primitive.Draw();
	}
}