// ================= DSA DLL Files =====================
// File: ArrayND.h
// XG	02/22/2009	Create
// =======================================================
// Note:
//

#ifndef DSA_ARRAYND_H
#define DSA_ARRAYND_H
#include <DSA/DSA.h>
#include <DSA/ClassID.h>
//#pragma
namespace DSA
{
	// N-Dimension indexer design: (N>=1)

	// Compact C-style row-major indexer:
	// (i1,i2,i3,...) gives i3+i2*d2
	template<int ND>
	class Indexer;

	// ==========   1D   =======================
	template<>
	class Indexer<1>
	{
	public:
		enum { ND = 1 };
		int  d[ND];  // the n-th dimension (size) in ND
		int  s[ND];  // dimension size (in 1D)

		bool set(int d1) { d[0]=d1;	s[0]=d1; return true; }
		inline int operator()(int const& i1) { return i1; }
	};

	// ==========   2D   =======================
	template<>
	class Indexer<2>
	{
	public:
		enum { ND = 2 };
		int  d[ND];  // the n-th dimension (size) in ND
		int  s[ND];  // dimension size (in 1D)

		bool set(int d1, int d2) {d[0]=d1;d[1]=d2;s[0]=d1;s[1]=d2*d1;return true;}
		inline int operator()(int const& i1, int const& i2) { return i1*s[0]+i2; }
	};

	// ==========   3D   =======================
	template<>
	class Indexer<3>
	{
	public:
		enum { ND = 3 };
		int  d[ND];  // the n-th dimension (size) in ND
		int  s[ND];  // dimension size (in 1D)

		bool set(int d1, int d2, int d3) {d[0]=d1;d[1]=d2;d[2]=d3;s[0]=d1;s[1]=d2*d1;s[2]=d3*d2*d1; return true;}
		inline int operator()(int const& i1, int const& i2, int const& i3) { return i1*s[1]+i2*s[0]+i3; }
	};

	// ==========   4D   =======================
	template<>
	class Indexer<4>
	{
	public:
		enum { ND = 4 };
		int  d[ND];  // the n-th dimension (size) in ND
		int  s[ND];  // dimension size (in 1D)

		bool set(int d1, int d2, int d3, int d4) {d[0]=d1;d[1]=d2;d[2]=d3;d[3]=d4;s[0]=d1;s[1]=d2*d1;s[2]=d3*d2*d1;s[3]=d4*d3*d2*d1; return true;}
		inline int operator()(int const& i1, int const& i2, int const& i3, int const& i4) { return i1*s[2]+i2*s[1]+i3*s[0]+i4; }
	};

	template<typename T, int ND, template<int> class INDEXER=Indexer >
	class ArrayND
	{
	public:
		T*           data;
		int          size;
		INDEXER<ND>  idx;

		// Constructors. MUST have number of d# matches ND !!!
		ArrayND():data(0),size(0)                               {}
		ArrayND(int d1):data(0),size(0)                         { resize(d1); }
		ArrayND(int d1, int d2):data(0),size(0)                 { resize(d1,d2); }
		ArrayND(int d1, int d2, int d3):data(0),size(0)         { resize(d1,d2,d3); }
		ArrayND(int d1, int d2, int d3, int d4):data(0),size(0) { resize(d1,d2,d3,d4); }

		virtual ~ArrayND() { clear(); }

		// Copy Construct
		ArrayND(ArrayND const& src);

		// Copy Assignment
		ArrayND& operator=(ArrayND const& src);

		// Copy from another C-style array, default construct the un-used space
		bool copy(const T* src, int srcLen, int space);

		// Assign to a single value
		ArrayND& operator=(T const& val);


		// Access data. MUST have number of indices matches ND !!!
		inline T& operator() (int const& i)        { return data[idx(i)]; }
		inline T& operator() (int const& i, int const& j) { return data[idx(i,j)]; }
		inline T& operator() (int const& i, int const& j, int const& k) { return data[idx(i,j,k)]; }
		inline T& operator() (int const& i, int const& j, int const& k, int const& l) { return data[idx(i,j,k,l)]; }

		// Resize. MUST have number of indices matches ND !!!
		// All original data are deleted.
		inline bool resize(int d1)                 { return (allocSpace(d1) && idx.set(d1)); }
		inline bool resize(int d1, int d2)         { return (allocSpace(d1*d2) && idx.set(d1,d2)); }
		inline bool resize(int d1, int d2, int d3) { return (allocSpace(d1*d2*d3) && idx.set(d1,d2,d3)); }
		inline bool resize(int d1, int d2, int d3, int d4) { return (allocSpace(d1*d2*d3*d4) && idx.set(d1,d2,d3,d4)); }

		// Access dimensions. MUST have number of indices matches ND !!!
		inline int d1() const { return idx.d[0]; }
		inline int d2() const { return idx.d[1]; }
		inline int d3() const { return idx.d[2]; }
		inline int d4() const { return idx.d[3]; }

		// Allocate Space
		bool allocSpace(int sz)
		{
			if(sz <= size)
				return true;
			else
			{
				clear();
				data = new T[sz];
				if(data)
				{
					size = sz;
					return true;
				}
			}
			return false;
		}
		// Resize KEEP original data
		bool keepDataResize(int newlen)
		{
			if( newLen > size )
			{
				// Allocate new data space
				int newSpace = m_space*2; // double original size
				T* new_data = (T*)new T[newSpace];

				// Return on allocation error
				if (new_data == 0)
				{
					m_space = 0; 
					m_len = 0;
					return false;
				}

				// Copy construct up to srcLen elements into the new array
				int i;
				for (i = 0;  i < m_len;  ++i) ::new((NewPlacement*)(new_data+i)) T(m_data[i]);
				
				//	Class-specific-default-construct any remaining elements
				while (i < newLen) ::new((NewPlacement*)(new_data+i++)) T();

				m_len = newLen;
				m_space = newSpace;

				// MUST delete current array and redefine 
				redefine(new_data, m_len, m_space); 
				return true;

			}
		}

		void clear() { if(data) delete [] data; data = NULL; }
	};


} // End of namespace DSA


#include <DSA/ArrayND.inl>

#endif