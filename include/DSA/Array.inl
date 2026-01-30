// ================= DSA DLL Files =====================
// File: Array.hpp
// XG  02/22/2009	Create
// =======================================================
// Note:
//
#ifndef DSA_ARRAY_INL
#define DSA_ARRAY_INL
//	Prerequisites:
#include <cfloat>
#include <cmath>
//#ifndef _WINNT_
//#include <windows.h>
//#endif

/*==========================================================================*\
**				Non-inline template function definitions					**
\*==========================================================================*/

namespace DSA
{
	//
	//	Array<T> non-inline template methods
	//
	template<class T>
	Array<T>::Array() : m_data(0), m_len(0), m_space(0)
	{}

	// Construct an array of given length(=space).
	template<class T>
	Array<T>::Array(int len) : m_data(0), m_len(0), m_space(0)
	{
		alloc(len);
	}

	// Construct an array of given space(>len).
	template<class T>
	Array<T>::Array(int len, int space) : m_data(0), m_len(0), m_space(0)
	{
		alloc(len,space);
	}

	// Construct and assignement
	template<class T>
	Array<T>::Array(const Array& a) : m_data(0), m_len(0), m_space(0)
	{
		copy(a.m_data, a.m_len, a.m_space);
	}

	template<class T>
	Array<T>& Array<T>::operator=(const Array& a)
	{
		copy(a.m_data, a.m_len, a.m_space);
		return *this;
	}

	// Move construct and assignment
	template<class T>
	Array<T>::Array(Array<T>&& rval) : m_data(0), m_len(0), m_space(0)
	{
		swap(rval);
	}

	template<class T>
	Array<T>& Array<T>::operator=(Array<T>&& rval)
	{
		swap(rval);
		return *this;
	}

	// Swap with another array
	template<class T>
	void Array<T>::swap(Array<T>& src)
	{
		m_data = src.m_data;
		m_len  = src.m_len;
		m_space= src.m_space;
	}

	// Copy-construct an array from a C-style array
	template<class T>
	Array<T>::Array(const T (*src), int len, int space) : m_data(0), m_len(0), m_space(0)
	{
		copy(src, len, space);
	}
/*	template<class T>
	template<class T, int N>
	Array<T>& Array<T>::operator=(T const(&)[N] src)
	{
		copy(src, N, N);
		return *this;
	}
*/

	// Copy-construct elements from the source C-style array, 
	// and class-specific-default-construct the unused space.
	template<class T>
	bool Array<T>::copy(const T (*src), int srcLen, int space)
	{
		// Allocate 
		if( m_data != src && alloc(srcLen, space) ) // !!! TBD. Deprecate alloc(len), use allocSpace() ONLY !
		{
			// Copy construct up to srcLen elements into the new array
			if (srcLen > abs(m_space)) srcLen = abs(m_space);

			// Class copy-construct elements
			int i;
			for (i = 0;  i < srcLen;  ++i) ::new((NewPlacement*)(m_data+i)) T(src[i]);

			m_len = srcLen;

			//	Class-specific-default-construct any remaining elements
			// "T" MUST have default constructor T() defined ! or error C2512 show here !
			while (i < abs(m_space)) ::new((NewPlacement*)(m_data+i++)) T();
//Delete this!			m_space = space; 

			return true;
		}
		return false;
	}


	template<class T>
	Array<T>& Array<T>::operator=(const T& value)
	{
		for (int i = 0;  i < len();  ++i) 
			m_data[i] = value;
		return *this;
	}

// Invoke each element's destructor for safe clean ups...
//	template<class T>
//	void Array<T>::dealloc()
//	{
//		for(int i = 0; i < m_len; i++)
//			~(T*)(m_data+i);
//		if(m_space>0) delete [] m_data;
//		m_len   = 0;
//		m_space = 0;
//	}


	// Allocate to a given space, if "space" <= existing space, use existing array.
	// otherwise, deallocate old array and allocate with default-construct new array.
	// alloc(0) can not be used to clean up array.
	// space MUST > 0 !!!
	template<class T>
	bool Array<T>::allocSpace(int space)
	{
		// Keep existing space if input "space" <= "m_space"
		if (space <= abs(m_space) ) //NOTE: Compatible with static-sized Array<T,N> !!!
		{
			if(m_len > space) 
				m_len = space > 0? space : 0;  // m_len MUST <= m_space
			return true;
		}

		// Dealloc memory 
		dealloc(); 

		// Since m_space always >= 0, "space" must > 0 already...
		// Default construct all elements
		m_data = (T*)new T[space];

		// Allocation failure
		if(m_data == 0)
		{
			if(m_space>0) 
				m_space = 0;
			m_len   = 0;
			return false;
		}
		else
		{
			m_space = space;
			if (m_len > m_space) m_len = m_space;
		}
		return true;
	}



	template<class T>
	bool Array<T>::resize(int newLen)
	{
		// If newLen is less than current length
		if(newLen <= m_len)
		{
			m_len = newLen < 0? 0 : newLen;
			return true;
		}
		else if(newLen <= abs(m_space) ) //NOTE: compatible with static-sized array !!!
		{
			// Default construct new elements
			int i = m_len;

			//	Class-specific-default-construct remaining elements
			// Note: class T should have a default constructor defined T(), to avoid
			// C4345 compiler warning.
			while (i < newLen) ::new((NewPlacement*)(m_data+i++)) T();
			m_len = newLen;
			return true;
		}
		else // If more space is needed.
		{
			// Preserve old data to be copied later...

			// Allocate new space:
			int newSpace = abs(m_space)*2; // double original size
			if(newSpace < newLen) newSpace = newLen; // Make sure enough space
			T* new_data = (T*)new T[newSpace];

			// Return on allocation error
			if (new_data == 0)
			{
				if(m_space>0) 
					m_space = 0; 
				m_len = 0;
				return false;
			}

			// Copy construct up to srcLen elements into the new array
			int i;
			for (i = 0;  i < m_len;  ++i) ::new((NewPlacement*)(new_data+i)) T(m_data[i]);
			
			//	Class-specific-default-construct any remaining elements
			while (i < newLen) ::new((NewPlacement*)(new_data+i++)) T();

//			m_len   = newLen;
//			m_space = newSpace;

			// MUST deallocate and redefine 
//			redefine(new_data, m_len, m_space); 
			redefine(new_data, newLen, newSpace); 
			return true;
		}
	}

	// Copy-construct one or more elements to the end of the array
	template<class T>
	bool Array<T>::append(const T& t)
	{
		if(grow())
		{
			::new((NewPlacement*)(m_data+m_len-1)) T(t);
			return true;
		}
		return false;
	}
	template<class T>
	bool Array<T>::append(const T* src, int n)
	{
		int len = m_len; // old length.

		if(resize( m_len + n) )
		{
			for (int i = len; i < m_len; ++i) ::new((NewPlacement*)(m_data+i)) T(src[i-len]);
			return true;
		}
		return false;
	}

	template<class T>
	int Array<T>::addUnique(const T& t)
	{
		int i = findFirst(t);
		if(i>=0)
			return i;
		append(t);
		return len()-1;
	}


	template<class T>
	int Array<T>::findFirst(const T& t, int iStart)
	{
		bool found = false;
		int i = iStart;
		for( ; !found && i < m_len; i++)
			if( m_data[i] == t )
				found = true;
		if(found)
			return i-1;
		else
			return -1;
	}


	template<class T>
	int Array<T>::remove(const T& t)
	{
		int cnt = 0;
		int i=0;
		while(i>=0)
		{
			i = findFirst(t,i);
			if( i>=0 )
			{
				for( int j=i; j < m_len-1; j++ )
					m_data[j] = m_data[j+1];
				m_len -= 1;
				cnt++;
			}
		}
		return cnt;
	}


	template<class T>
	void Array<T>::redefine(T* data, int len, int space)
	{
		if (m_data != data)
		{
			// Dealloc()
			dealloc();

			m_data  = data;
			m_len   = len;
			m_space = space;
		}
	}

	template<class T>
	void Array<T>::getMinMax(T &min, T &max)
	{
		int i = 0; //index for the 1st non-NaN.
		// Initialized min and max:
		for(i = 0; i < m_len; i++)
		{
			if (!_isnan(m_data[i])) //TBD use !(v[i] == v[i]) ???
			{
				max = m_data[i];
				min = m_data[i];
				break;
			}
		}
		// ALL-NaN case:
//		if( i == m_len && _isnan(
		// start from the 1st non-NaN
		for (int j = i; j < m_len; j++)
		{
			if (max < m_data[j]) max = m_data[j];
			if (min > m_data[j]) min = m_data[j];
		}
	}

	// Find the min and max values of an array.
	template <typename T>
	void getMinMax(Array<T>& v, T& min, T& max)
	{
		int i = 0; //index for the 1st non-NaN.
		int len = v.len();
		// Initialized min and max:
		for(i = 0; i < len; i++)
		{
			if (!std::isnan((double)v[i]))
			{
				max = v[i];
				min = v[i];
				break;
			}
		}
		// start from the 1st non-NaN
		for (int j = i; j < len; j++)
		{
//			if(!_isnan((double)v[j])) //XG: add omiting NaN.
			{
				if (max < v[j]) max = v[j];
				if (min > v[j]) min = v[j];
			}
		}
	}

}// End of namespace DSA
#endif