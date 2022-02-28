#include <external\debug_console.h>

namespace con
{
	HANDLE hConsole = NULL;

	void InitConsole()
	{
		if (!hConsole)
		{
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTitleW(L"Quake II console");
		}
	}

	void SetConColor(int color)
	{
		SetConsoleTextAttribute(hConsole, (WORD)color);
	}

	// push to warnings stack
#pragma warning(push)
// disable warning _s functions
#pragma warning(disable:4996)
// if wchar_t buffer bigger then unsigned long (u32) ~(4,294,967,295) the output will be erroneous
#pragma warning(disable:4267)

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

	u32 U32FloatToWCHAR(float value, wchar_t* dstStr)
	{
		char _string[_CVTBUFSIZE];
		_gcvt(value, 7, _string);
		mbstowcs(dstStr, _string, _CVTBUFSIZE);

		return wcslen(dstStr);
	}

	u32 U32DoubleToWCHAR(double value, wchar_t* dstStr)
	{
		char _string[_CVTBUFSIZE];
		_gcvt(value, 15, _string);
		mbstowcs(dstStr, _string, _CVTBUFSIZE);

		return wcslen(dstStr);
	}

	void VoidFloatToWCHAR(float value, wchar_t* dstStr)
	{
		char _string[_CVTBUFSIZE];
		_gcvt(value, 7, _string);
		mbstowcs(dstStr, _string, _CVTBUFSIZE);
	}

	void VoidDoubleToWCHAR(double value, wchar_t* dstStr)
	{
		char _string[_CVTBUFSIZE];
		_gcvt(value, 15, _string);
		mbstowcs(dstStr, _string, _CVTBUFSIZE);
	}

	// i32
	u32 U32I32ToWCHAR(i32 value, wchar_t* dstStr)
	{
		_itow(value, dstStr, 10);
		return wcslen(dstStr);
	}

	void VoidI32ToWCHAR(i32 value, wchar_t* dstStr)
	{
		_itow(value, dstStr, 10);
	}

	// u32
	u32 U32U32ToWCHAR(u32 value, wchar_t* dstStr)
	{
		_ultow(value, dstStr, 10);
		return wcslen(dstStr);
	}

	void VoidU32ToWCHAR(u32 value, wchar_t* dstStr)
	{
		_ultow(value, dstStr, 10);
	}

	// i64
	u32 U32I64ToWCHAR(i64 value, wchar_t* dstStr)
	{
		_i64tow(value, dstStr, 10);
		return wcslen(dstStr);
	}

	void VoidI64ToWCHAR(i64 value, wchar_t* dstStr)
	{
		_i64tow(value, dstStr, 10);
	}

	// u64
	u32 U32U64ToWCHAR(u64 value, wchar_t* dstStr)
	{
		_ui64tow(value, dstStr, 10);
		return wcslen(dstStr);
	}

	void VoidU64ToWCHAR(u64 value, wchar_t* dstStr)
	{
		_ui64tow(value, dstStr, 10);
	}

	// convert to value

	void WCHARToi32(i32* value, wchar_t* src)
	{
		*value = _wtoi(src);
	}

	void WCHARToi64(i64* value, wchar_t* src)
	{
		*value = _wtoi64(src);
	}

	void WCHARToFloat(float* value, wchar_t* src)
	{
		*value = (float)_wtof(src);
	}

	void WCHARToDouble(double* value, wchar_t* src)
	{
		*value = _wtof(src);
	}

	void Outf(const wchar_t* fmt, ...)
	{
		using namespace DirectX;

		va_list args;
		va_start(args, fmt);

		wchar_t result[2056];
		wchar_t temp[1024];
		memset(result, NULL, 2056 * sizeof(wchar_t));
		memset(temp, NULL, 1024 * sizeof(wchar_t));

		int i = 0;
		int length = wcslen(fmt);
		while (i < length)
		{
			if (fmt[i] == '%')
			{
				switch (fmt[i + 1])
				{
				case 's':
				{
					switch (fmt[i + 2])
					{
					case '8':
					{
						char* charstr = va_arg(args, char*);
						mbstowcs(temp, charstr, 1024);
						wcscat(result, temp);
						i = i + 1;
					} break;
					case '1':
					{
						wcscpy(temp, va_arg(args, wchar_t*));
						wcscat(result, temp);
						i = i + 2;
					} break;
					invalid_default
					}
				} break;
				case 'i':
				{
					switch (fmt[i + 2])
					{
					case '3':
					{
						_itow(va_arg(args, int), temp, 10);
						wcscat(result, temp);
					} break;
					case '6':
					{
						_i64tow(va_arg(args, long long), temp, 10);
						wcscat(result, temp);
					} break;
					invalid_default
					}
					i = i + 2;
				} break;
				case 'u':
				{
					switch (fmt[i + 2])
					{
					case '3':
					{
						_ultow(va_arg(args, unsigned long), temp, 10);
						wcscat(result, temp);
					} break;
					case '6':
					{
						_ui64tow(va_arg(args, unsigned long long), temp, 10);
						wcscat(result, temp);
					} break;
					invalid_default
					}
					i = i + 2;
				} break;
				case 'f':
				{
					double value = va_arg(args, double);
					VoidFloatToWCHAR((f32)value, temp);
					wcscat(result, temp);
				} break;
				case 'b':
				{
					switch (fmt[i + 2])
					{
					case '3':
					{
						unsigned long value = va_arg(args, unsigned long);
						u32 iter = 0;
						for (int j = 0; j < 32; j++)
						{
							if (value & 0x8000'0000)
							{
								temp[iter] = '1';
							}
							else
							{
								temp[iter] = '0';
							}
							value = value << 1;
							iter++;
							if (j == 7 || j == 15 || j == 23)
							{
								temp[iter] = ' ';
								iter++;
							}
						}
						wcscat(result, temp);

					} break;
					case '6':
					{
						unsigned long long value = va_arg(args, unsigned long long);
						u32 iter = 0;
						for (int j = 0; j < 64; j++)
						{
							if (value & 0x8000'0000'0000'0000)
							{
								temp[iter] = '1';
							}
							else
							{
								temp[iter] = '0';
							}
							value = value << 1;
							iter++;
							if (j == 7 || j == 15 || j == 23 || j == 31 || j == 39 || j == 47 || j == 55)
							{
								temp[iter] = ' ';
								iter++;
							}
						}
						wcscat(result, temp);
					} break;
					invalid_default
					}
					i = i + 2;
				} break;
				case 'm':
				{
					switch (fmt[i + 2])
					{
					case '4':
					{
						XMMATRIX value = va_arg(args, XMMATRIX);
						//value = Transpose(value);

						for (int i = 0; i < 4; i++)
						{
							for (int j = 0; j < 4; j++)
							{
								value.r[i].m128_f32[j];

								VoidFloatToWCHAR(value.r[i].m128_f32[j], temp);

								if (j == 3 || j == 7 || j == 11)
									wcscat(temp, L"\n");
								else
									wcscat(temp, L" ");

								wcscat(result, temp);
							}
						}

					} break;
					invalid_default
					}
					i = i + 1;
				} break;
				case 'v':
				{
					switch (fmt[i + 2])
					{
					case '4':
					{
						XMFLOAT4 value = va_arg(args, XMFLOAT4);

						VoidFloatToWCHAR(value.x, temp);
						wcscat(temp, L" ");

						VoidFloatToWCHAR(value.y, temp);
						wcscat(temp, L" ");

						VoidFloatToWCHAR(value.z, temp);
						wcscat(temp, L" ");

						VoidFloatToWCHAR(value.w, temp);
						wcscat(result, temp);

					} break;
					}
					i = i + 1;
				} break;
				invalid_default
				}
				i = i + 2;
			}
			else
			{
				result[wcslen(result)] = fmt[i];
				i++;
			}
		}
		WRITE_CONSOLE(hConsole, result, wcslen(result));
		va_end(args);
	}


	// pop of warnings stack
#pragma warning(pop)
}