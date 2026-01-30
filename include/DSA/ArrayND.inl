// ================= DSA DLL Files =====================
// File: ArrayND.hpp
// XG  02/22/2009	Create
// =======================================================
// Note:
//
#ifndef DSA_ARRAYND_INL
#define DSA_ARRAYND_INL
//	Prerequisites:
//#include <float.h>
//#ifndef _WINNT_
//#include <windows.h>
//#endif

/*==========================================================================*\
**				Non-inline template function definitions					**
\*==========================================================================*/

namespace DSA
{

	// Copy Construct
	template<typename T, int ND, template<int> class INDEXER >
	ArrayND<T,ND,INDEXER>::ArrayND(const ArrayND<T,ND,INDEXER>& src) : data(0)
	{
		copy(src.data, src.size, src.size);
	};

	// Copy Assignment
	template<typename T, int ND, template<int> class INDEXER >
	ArrayND<T,ND,INDEXER>& ArrayND<T,ND,INDEXER>::operator=(const ArrayND<T,ND,INDEXER>& src)
	{
		copy(src.data, src.size, src.size);
//		Array<T>::operator =(src);
		return *this;
	};

	// Copy Assignment
	template<typename T, int ND, template<int> class INDEXER >
	bool ArrayND<T,ND,INDEXER>::copy(const T* src, int srcLen, int space)
	{
		if(space < srcLen)
			space = srcLen;
		if( allocSpace(space) )
		{
			for (int i = 0;  i < srclen;  ++i) 
				data[i] = val;
			return true;
		}
		return false;
	};

	template<typename T, int ND, template<int> class INDEXER >
	ArrayND<T,ND,INDEXER>& ArrayND<T,ND,INDEXER>::operator=(const T& val)
	{
		int len = idx.s[ND-1]; //TBD...
		for (int i = 0;  i < len;  ++i) 
			data[i] = val;
		return *this;
	};


}// End of namespace DSA
#endif