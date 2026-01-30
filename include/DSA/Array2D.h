// ================= DSA DLL Files =====================
// File: Array2D.h
// XG	02/22/2009	Create
// XG	07/16/2009	Add to DSA library
// XG   06/26/2012	Add addUnique(), remove(), find()
// =======================================================
// Note:
//

#ifndef DSA_ARRAY2D_H
#define DSA_ARRAY2D_H
#include <DSA/DSA.h>
#include <DSA/Array.h>
#include <DSA/ClassID.h>
namespace DSA
{
	class Stream;
}
//#pragma
namespace DSA
{


	// Array2D<T>[x/i][y/j]
	// Use "Row-Major" storage, as in C, PL/I, Python
	// For Matrix:  // first index (i) gives the Row,
	// ----------> [y/j] ColN
	// |------> Row1
	// |
	// |
	// |------> RowN
	// V [x/i]    

	// Its transpose yields bmp data struct: 
	// ^ [y] ColN
	// |^       ^
	// ||       |
	// ||       |
	// ----------> [x] RowN


	template<typename T, int XSPACE=0, int YSPACE=0>
	class Array2D;

	// Array 2D with dynamic storage ===============

	template<class T>
	class Array2D<T,0,0> : protected Array<T>
	{
	protected: // Data Members
		int		m_rows;     // number of rows
		int		m_cols;     // number of columns

	public:
		// Constructors:
		Array2D() : m_rows(0), m_cols(0) {};
		// Default construct an array of given length(=space).
		Array2D(int nRow, int nCol);
		// Default construct an array of given space(>len).
		Array2D(int nRow, int nCol, int space);

		// Copy Construct
		Array2D(const Array2D<T>& src);

		// Copy Assignment
		Array2D<T>& operator=(const Array2D<T>& src);

		// Assign all current elements from a given value
		Array2D<T>& operator=(const T& value);
		//		~Array2D() {if(m_data) delete [] m_data;}

		// (Re)alloc array size, invalidate data, and use existing memory if possible.
		virtual bool alloc(int newRow, int newCol);

		// Resize the array to a different size. ALWAYS keep existing contents, and use
		// existing memory space if possible.
		virtual bool resize(int newRow, int newCol);
		
		// Access number of rows or columns
		inline int  rows() const { return m_rows; }
		inline int  cols() const { return m_cols; }

		// Access to given Row (row-major) as i => X|Row; j => Y|Col. 
		inline T*       operator[](int iRow)              {return m_data+iRow*m_cols;}
		inline const T* operator[](int iRow) const        {return m_data+iRow*m_cols;}

		inline T&       element(int iRow, int jCol)       {return m_data[iRow*m_cols+jCol];}
		inline const T& element(int iRow, int jCol) const {return m_data[iRow*m_cols+jCol];}

		// Return a pointer to the data area as a pointer to the T.
		T* begin()										  {return m_data;}
		const T* begin() const							  {return m_data;}

		void getMinMax(T& min, T& max) { Array<T>::getMinMax(min,max); } 

		// ========= Common class interfaces  =========================
		public:
		typedef DSA::NewClassID<12874,0,ID_DLL> IDClass;
		typedef Array2D BaseClass;
		// Run-time Stream I/O
		DSA_Export bool put(      Stream& stream, UChar ver) const {return true;}
		DSA_Export bool get(const Stream& stream, UChar ver)       {return true;}
		// ============================================================
	};

	// A static Array2D buffer in Cache
	// NOTE: Never swap Array2DBuf<>
	//
	template<class T, int NRow, int NCol>
	class Array2D : protected Array2D<T,0,0>
	{
	public:
		T    data[NRow*NCol]; // Data buffer

		Array2D() : Array2D<T,0,0> ()
		{
			m_data  = &data[0];
			m_len   = NRow * NCol;
			m_space = -m_len;          // Indicate static buffer
			m_rows  = NRow;
			m_cols  = NCol;
		}

		virtual ~Array2D() {}

		// ========= Common class interfaces  =========================
		public:
		typedef Array2D<T,0,0> BaseClass;
		typedef DSA::NewClassID<34664,0,ID_DLL> IDClass;
		// Run-time Stream I/O
		DSA_Export bool put(      Stream& stream, UChar ver) const {return true;}
		DSA_Export bool get(const Stream& stream, UChar ver)       {return true;}
		// ============================================================
	};

} // End of namespace DSA

#include <DSA/Array2D.inl>

#endif