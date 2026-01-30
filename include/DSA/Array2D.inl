// ================= DSA DLL Files =====================
// File: Array2D.hpp
// XG  02/22/2009	Create
// =======================================================
// Note:
//
#ifndef DSA_ARRAY2D_INL
#define DSA_ARRAY2D_INL
//	Prerequisites:
#include <float.h>
//#ifndef _WINNT_
//#include <windows.h>
//#endif

/*==========================================================================*\
**				Non-inline template function definitions					**
\*==========================================================================*/

namespace DSA
{

	// Copy Construct
	template<class T>
	Array2D<T>::Array2D(const Array2D<T>& src) : Array<T>(src)
	{
		m_rows = src.m_rows;
		m_cols = src.m_cols;
	};

	// Copy Assignment
	template<class T>
	Array2D<T>& Array2D<T>::operator=(const Array2D<T>& src)
	{
		Array<T>::operator =(src);
		m_rows = src.m_rows;
		m_cols = src.m_cols;
		return *this;
	};

	template<class T>
	Array2D<T>& Array2D<T>::operator=(const T& val)
	{
		for (int i = 0;  i < m_len;  ++i) 
			m_data[i] = val;
		return *this;
	};
	// Default construct an array of given length(=space).
	template<class T>
	Array2D<T>::Array2D(int nRow, int nCol) : Array<T>(nRow*nCol)
	{
		m_rows  = nRow;
		m_cols  = nCol;
	}

	// Default construct an array of given space(>len).
	template<class T>
	Array2D<T>::Array2D(int nRow, int nCol, int space) : Array<T>(nRow*nCol, space)
	{
		m_rows  = nRow;
		m_cols  = nCol;
	}

	template<class T>
	bool Array2D<T>::resize(int nRow, int nCol)
	{
		if( Array<T>::resize(nCol * nRow) )
		{
			m_rows  = nRow;
			m_cols  = nCol;
			return true;
		}
		else
		{
			Array<T>::resize(0);
			m_rows = 0;
			m_cols = 0;
		}
		return false;
	}

	template<class T>
	bool Array2D<T>::alloc(int nRow, int nCol)
	{
		if( Array<T>::alloc(nCol * nRow) )
		{
			m_rows  = nRow;
			m_cols  = nCol;
			return true;
		}
		else
		{
			Array<T>::alloc(0);
			m_rows = 0;
			m_cols = 0;
		}
		return false;
	}
}// End of namespace DSA
#endif