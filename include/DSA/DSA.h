// ================= DSA DLL Files =====================
// File: DSA.h  
// Data Structure and Algorithm Library for Native Software
// Coding.
// 
// XG	07/14/2009	Created.
//	
// =======================================================
#ifndef DSA_H
#define DSA_H

#include <limits>
#include <stdio.h>
#include <cstdint>

// EXPORT (IMPORT) MACROS
// ================================

#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif
/*
#if defined(DSALIB)
#pragma message("DSALIB is defined")
#define DSA_Export __declspec (dllexport)
#define DSA_Import __declspec (dllimport)
#else
#pragma message("DSALIB is NOT defined")
#define DSA_Export __declspec (dllexport)
#define DSA_Import __declspec (dllimport)
//#define DSA_Export __declspec (dllimport)
//#define DSA_Import __declspec (dllimport)
#endif
*/
#if defined(_WIN32) || defined(__CYGWIN__)
    // Windows/MSVC/MinGW logic
    #define DSA_Export __declspec(dllexport)
    #define DSA_Import __declspec(dllimport)
#else
    // Linux/Unix logic (GCC/Clang)
    #if __GNUC__ >= 4
        #define DSA_Export __attribute__((visibility("default")))
        #define DSA_Import __attribute__((visibility("default")))
    #else
        #define DSA_Export
        #define DSA_Import
    #endif
#endif

/*
#if defined(DSALIB)
	#if defined(DSA_Import)
		#undef DSA_Import
		#undef DSA_EXPORTS
	#endif
	#if !defined(DSA_Export)
		#define DSA_Export
		#define DSA_EXPORT(retType) __declspec(dllexport) retType
	#define DSA_EXPORTS __declspec(dllexport)
	#endif
#else
	#if defined(DSA_Export)
		#undef DSA_Export
		#undef DSA_EXPORTS
	#endif
	#if !defined(DSA_Import)
		#define DSA_Import
		#define DSA_EXPORTS __declspec(dllimport)
	#endif
#endif
*/
// ================================

// All class's ID should be declared by this macro:
// class MyClass
// {
//    ...
//    _MakeClassID_(12422, 1, MyDLL::ID_DLL);
// }

// TBD Use ClassID class instead...
// #include <windows.h>

namespace DSA
{
	// ==========  DLL Module Common Interface  ==========
	enum
	{
		ID_DLL = 23 // UChar[256] value for DLL ID
	};
//	DSA_Export HINSTANCE instance();
	// ===================================================

	// DLL EXPORT Functions:
//	int CALLBACK test();
	DSA_Export void test();


	// ==========================================
	// Fundamental Data Types
	// ==========================================

	// INT Data type by its length (C99/C++11 cross-platform equivalents)
	typedef uint8_t   UChar;     // 1-byte (8-bit) unsigned int [0; 255]
	typedef uint16_t  UShort;    // 2-byte (16-bit) unsigned int short [0; 65,535]
	typedef uint32_t  ULong;     // 4-byte (32-bit) unsigned int long  [0; 4,294,967,295]
	typedef uint64_t  ULongLong; // 8-byte (64-bit) unsigned int

	typedef int8_t   SChar;     // 1-byte (8-bit)  signed int [0; 255]
	typedef int16_t  SShort;    // 2-byte (16-bit) signed int short [0; 65,535]
	typedef int32_t  SLong;     // 4-byte (32-bit) signed int long  [0; 4,294,967,295]
	typedef int64_t  SLongLong; // 8-byte (64-bit) signed int

	// Floating data types
	typedef float    Float;   // 4-byte (32-bit) 3.4E +/- 38 (7 digits)
	typedef double   Double;  // 8-byte (64-bit) 1.7E +/- 308 (15 digits)

	static double __attribute__((unused)) NaN = std::numeric_limits<double>::quiet_NaN();
	static double __attribute__((unused)) INF = std::numeric_limits<double>::infinity();

	static float  __attribute__((unused)) NaN32 =std::numeric_limits<float>::quiet_NaN();
	static float  __attribute__((unused)) INF32 =std::numeric_limits<float>::infinity();

	// Complex number types
	struct Complex8
	{
		Complex8() : re(0), im(0) {}
		virtual ~Complex8() {}
		float re;
		float im; 
	};

	struct Complex16
	{
		Complex16() : re(0), im(0) {}
		virtual ~Complex16() {}
		double re;
		double im; 
	};

	// TBD: Add an Exception class...
	class Exception 
	{
	public:
		bool  bAbort;
		int   codeID;
		char  msg[128];
		Exception(const char* txt) :  bAbort(false), codeID(0)
		{
			//MS_specific sprintf_s(msg, "%s", txt);
			snprintf(msg, 128, "%s", txt);
		}
	};



}
#endif
