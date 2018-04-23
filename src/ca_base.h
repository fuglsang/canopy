#pragma once

namespace ca
{
	//----------
	// keywords

	#define CA_INLINE inline

	//------------
	// primitives

	typedef char i8;
	typedef short i16;
	typedef int i32;
	typedef long long i64;
	#define CA_XDECL_TYPES_INTEGRAL_SIGNED\
		X(i8)\
		X(i16)\
		X(i32)\
		X(i64)\

	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
	typedef unsigned long long u64;
	#define CA_XDECL_TYPES_INTEGRAL_UNSIGNED\
		X(u8)\
		X(u16)\
		X(u32)\
		X(u64)\

	#define CA_XDECL_TYPES_INTEGRAL\
		CA_XDECL_TYPES_INTEGRAL_SIGNED\
		CA_XDECL_TYPES_INTEGRAL_UNSIGNED\

	typedef float f32;
	typedef double f64;
	#define CA_XDECL_TYPES_FLOATING\
		X(f32)\
		X(f64)\

	// empty line consumes last backslash
}