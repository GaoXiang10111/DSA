// ================= DSA DLL Files =====================
// File: DSA.cpp  
// Data Structure and Algorithm Library
// XG	07/14/2009	Created.
//	
// =======================================================

#include "DSA/DSA.h"
#include "DSA/Array.h"
#include <DSA/Hash.h> 

#include <DSA/Holder.h>
#include <cstring>

namespace DSA
{
//	static HINSTANCE hDSAInst;
//	HINSTANCE instance() { return hDSAInst;	}
}

/*
// DLL MAIN
int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	switch (fdwReason)
	{
    case DLL_PROCESS_ATTACH:
		DSA::hDSAInst = hInstance;
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
*/

// DLL export functions: TBD remove...
namespace DSA
{

	int hashTest(char* const& s)
	{
		int i = 0;
		short c = 0;
		while(s[i]) c += s[i++];
		return c%256;
	}
	bool hashMatch(char* const& s0, char* const& q)
	{
		return strcmp(s0, q)==0; 
	}

	void test()
	{
/* TBD FIx this	
		DSA::Array<int> t;
		DSA::Array<double> d(30);
		for(int i = 0; i < d.len(); i++)
			d[i] = -i;

//		DSA::Array<double>  d2( d.begin(), d.len());
		DSA::Array<double> d3( d );
		d3.resize(4);
		d3.append(2.0);
		d3.resize(33);
		double min, max;
		getMinMax(d3, min, max); 
//		Array< Array< double> > d;


		DSA::Lists<char> lists;
		lists.alloc(20, 5);
		lists.insertAtEnd('a',1);
		lists.insertAtEnd('b',1);
		lists.insertAtEnd('c',1);
		lists.insertAtEnd('d',1);
		lists.remove('d',1);
		lists.remove('b',1);
		lists.remove('c',1);
		Lists<char>::LinkNext* o = lists.getLast(1);
		DSA::HashEntry<char*, int>* a0;
		DSA::Array< HashEntry<double, double> > arr;
		DSA::Hash<char*, int> h(hashTest, hashMatch, 200,  256 );
		h.add(DSA::HashEntry<char*,int>("cc",   4) );
		h.add(DSA::HashEntry<char*,int>("aaad", 5) );
		h.add(DSA::HashEntry<char*,int>("cc",   7) );
		h.del("cc");

		a0 = h.find("cc");
*/

		return;
	}

}