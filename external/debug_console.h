#pragma once

#include <windows.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <DirectXMath.h>

#define WRITE_CONSOLE(handle, str, length) WriteConsoleW(handle, str, length, NULL, NULL)

#define invalid_default default:{assert(false);}

/*
	formatted keys

	%s8  - string (char)
	%s16 - string (wchar_t)

	%i32 - int (i32)
	%i64 - long long (i64)

	%u32 - unsigned int (u32)
	%u64 - unsigned long long (u64)

	%f - float (f32)

	%b32 - binary output all bit in (32 bits) variable
	%b64 - binary output all bit in (64 bits) variable

	%m2 - matrix 2x2 (m2)
	%m3 - matrix 3x3 (m3)
	%m4 - matrix 4x4 (m4)

	%v2 - vector 2 (v2)
	%v3 - vector 3 (v3)
	%v4 - vector 4 (v4)

*/

namespace con
{
	extern HANDLE hConsole;

	void InitConsole();

	void SetConColor(int color);

	typedef int8_t		i8;
	typedef int16_t		i16;
	typedef int32_t		i32;
	typedef int64_t		i64;

	typedef uint8_t		u8;
	typedef uint16_t	u16;
	typedef uint32_t	u32;
	typedef uint64_t	u64;

	typedef float		f32;
	typedef double		f64;

	// convert to string

	u32 U32FloatToWCHAR(float value, wchar_t* dstStr);

	u32 U32DoubleToWCHAR(double value, wchar_t* dstStr);

	void VoidFloatToWCHAR(float value, wchar_t* dstStr);

	void VoidDoubleToWCHAR(double value, wchar_t* dstStr);

	// i32
	u32 U32I32ToWCHAR(i32 value, wchar_t* dstStr);

	void VoidI32ToWCHAR(i32 value, wchar_t* dstStr);

	// u32
	u32 U32U32ToWCHAR(u32 value, wchar_t* dstStr);

	void VoidU32ToWCHAR(u32 value, wchar_t* dstStr);

	// i64
	u32 U32I64ToWCHAR(i64 value, wchar_t* dstStr);

	void VoidI64ToWCHAR(i64 value, wchar_t* dstStr);

	// u64
	u32 U32U64ToWCHAR(u64 value, wchar_t* dstStr);

	void VoidU64ToWCHAR(u64 value, wchar_t* dstStr);

	// convert to value

	void WCHARToi32(i32* value, wchar_t* src);

	void WCHARToi64(i64* value, wchar_t* src);

	void WCHARToFloat(float* value, wchar_t* src);

	void WCHARToDouble(double* value, wchar_t* src);

	void Out(char* str);

	void Outf(const wchar_t* fmt, ...);

}