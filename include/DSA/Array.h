// ================= DSA DLL Files =====================
// File: Array.h
// XG	02/22/2009	Create
// XG	07/16/2009	Add to DSA library
// XG   06/26/2012	Add addUnique(), remove(), find()
// XG   07/04/2015  Add CArray and Irregular2DArray for FPGrowth
// =======================================================
// Note:
//

#ifndef DSA_ARRAY_H
#define DSA_ARRAY_H
#include <cstdlib>
#include <cstring> // memcpy
#include <DSA/DSA.h>
#include <DSA/ClassID.h>
namespace DSA
{
	class Stream;
}
//#pragma
namespace DSA
{
	//XG: 06/16/15: test this faster version of Array<> !!!
	template<class T>
	class CArray
	{
	protected: // Data Members
		T*    m_data;    // Data (points to the 1st element)
		int   m_len;     // Number of defined elements
		int		m_space;   // Reserved space
	public:
		static const size_t UnitSize = sizeof(T);
		CArray() : m_data(nullptr), m_len(0), m_space(0) {};
		CArray(int len, int space=0): m_data(nullptr), m_len(0), m_space(0){ construct(len, space); }
		virtual ~CArray() { dealloc(); };
		// Access:
		inline int size() const    { return m_len; }
		inline int bufsize() const { return m_space; }
		inline T* begin() const    { return m_data; }

		inline       T& operator[](int i)       { return m_data[i]; }
		inline const T& operator[](int i) const { return m_data[i]; }

		inline T&       set(int i)       {return m_data[i];}
		inline const T& get(int i) const {return m_data[i];}

		T* last()			{return m_data+m_len-1;}//CAREFUL ! if m_data=0, m_len=0 then last()!=0
		const T* last() const	{return m_data+m_len-1;}

		// Grow with default construct, without the bound checks!!!
		T& grow()   
		{
			resize(m_len+1);
			return * ::new(&m_data[m_len-1]) T();
		}
		// Append by copy construct, return the actual appended item
		T& append(const T& src) {
			resize(m_len+1);
			return * new (&m_data[m_len-1]) T(src);
		}
		// Resize without default construct new data
		virtual void resize(int len){
			if (len > m_space)
				realloc(len*2); // 0, 2, 6, 14, 30...
			m_len = len; 
		}
		// Fast memcpy, without invoking copy constructors
		CArray<T>& operator=(const T& a0){
			for(int i=0; i<m_len; ++i)
				memcpy((char*)m_data+i*UnitSize, &a0, UnitSize);
			return *this;
		}

		// Swap with another CArray
		CArray<T>& swap(CArray<T>& src) {
			T*  tmp = src.m_data;  src.m_data = m_data; m_data = tmp;
			int len = src.m_len;   src.m_len  = m_len;  m_len  = len;
			int sz  = src.m_space; src.m_space= m_space;m_space= sz;
			return *this;
		};
		// Default construct given length (Time consuming)
		CArray<T>& construct(int len, int bufSpace=0){
			realloc(bufSpace<len? len : bufSpace);
			// Note: class T should have a default constructor defined T(), to avoid
			// C4345 compiler warning.
			int i = 0;
			while (i < len) ::new((T*)(m_data+i++)) T();
			m_len = len;

			return *this;
		}

		CArray<T>& destruct(){
			for(int i = 0; i < m_len; i++)
				~(T*)(m_data+i);
			m_len = 0;
		}

		// Find the first occurance when the given condition is ture, return nullptr on failure
		// Lambda can be: 
		// [&test](const Element& item) {return test==item;} // 1st match item
		// [](const Element& item) {return item<0;} // 1st negative item
		template<typename Lambda> //[](const Element&)->bool {return true}
		T* find1st(Lambda testtrue)
		{
			T* found = nullptr;
			for (int i=0; !found && i<m_len; ++i){
				if (testtrue(m_data[i]))
				found = &m_data[i];
			}
			return found;
		}

		void dealloc() {
			if(m_data)
				free(m_data);
			m_data = nullptr;
			m_len  = 0;
			m_space = 0;
		}
		// TBD a faster version, without constructing each elements:
		bool realloc(int space){
			if(space > m_space)
			{
				void* mem = malloc(UnitSize*space);
				if(mem == nullptr){
					return false;  // Failed allocation...
				}
				if(m_data){
					if(m_len > 0)
						memcpy(mem, m_data, sizeof(T)*m_len);
					free(m_data);
				}
				m_data  = (T*)mem;
				m_space = space;
			}
			return true;
		}
	};

	
	// FPHeads is an irregular 2D array, (rows have different length).
	// Size of each row MUST be fixed !
	// (0,0), (0,1), (0,2)
	// (1,0), (1,1)
	// (2,0), (2,1), (2,2), (2,3)
	// (3,0) 
	// 
	template<typename T>
	struct Irregular2DArray
	{
		CArray<T*>   row;
		CArray<T>    buf;
		Irregular2DArray()  {};
		~Irregular2DArray() {};
		// Access
		T&       get(int iRow, int iCol=0)       { return row[iRow][iCol]; }
		const T& get(int iRow, int iCol=0) const { return row[iRow][iCol]; }
		T*       getLastOne()  { return buf.last(); }

		int      numRows()     { return row.size(); }

		// Setup each row from each row size, WITHOUT initialize data !
		// Use get(i,j) to set/get the values.
		// NOTE: row size MUST be correct, otherwise get(i,j) will misalign data !!!
		void  setupEachRow(const CArray<int>& rowSizes) 
		{
			int totSize = 0;
			for(int i=0; i<rowSizes.size(); ++i)
				totSize += abs(rowSizes[i]);
			row.resize(rowSizes.size());
			buf.resize(totSize);
			// Align each row ptr
			totSize = 0;
			for(int i=0; i<rowSizes.size(); ++i)
			{
				row[i] = &buf[totSize];
				totSize += rowSizes[i];
			}
		}
		// initialize buffer data!
		void initBuf() {buf = T();}
	};

	//==============================================================

	template<typename T, int SPACE=0>
	class Array;

	// Array with dynamic storage =========================
	template<class T>
	class Array<T,0>
	{
	protected: // Data Members
		T* 	m_data;    // Data (points to the 1st element)
		int		m_len;     // Number of defined elements
		int		m_space;   // Reserved space
	public:
		// Constructors:
		Array();
		virtual ~Array() { dealloc(); }
		// Construct an array of given length(=space).
		Array(int len);
		// Construct an array of given space(>len).
		Array(int len, int space);

		// Copy-construct and assignment
		Array(const Array& src);
		Array<T>& operator=(const Array& src);

		// Move construct and assignment
		Array(Array&& rval);
		Array<T>& operator=(Array&& rval);

		// Swap with another array, same as move!
		void swap(Array& src);

		// Copy-construct from a C-style array
		Array(const T (*src), int srcLen, int space = 0);
		template <typename T1, int N>
		Array(T1 const(&src)[N] ) : m_data(0), m_len(0), m_space(0) //C2552, MS doesn't work for this...
		{
	      	copy(src, N, N);
		}


		template <typename T1, int N>
		Array<T1>& operator=(T1 const(&src)[N] ) // TBD for aggregate? Add to Array<T,N> as well...
		{
	      	copy(src, N, N);
			return *this;
		}
		template <typename T1, int N>
		Array<T1>& operator=(T1 const(&&src)[N] ) // TBD for aggregate? Add to Array<T,N> as well...
		{
	      	copy(src, N, N);
			return *this;
		}

		// Copy-construct elements from the source C-style array, 
		// and class-specific-default-construct the unused space.
		virtual bool copy(const T (*src), int srcLen, int space);

		// Assign all current elements to a single value
		Array<T>& operator=(const T& value);

		// (Re)alloc array size, invalidate data, and use existing memory if possible.
		// Allocate to a given length and space. 
		virtual bool alloc(int len)            { if(allocSpace(len)) {m_len=len; return true;} return false;}
		virtual bool alloc(int len, int space) { if(allocSpace(space<len? len : space)) {m_len=len; return true;} return false;}
		virtual bool allocSpace(int space);

		// Array length (number of elements) and total reserved space:
		virtual int len() const           {return m_len;}
		virtual int size() const          {return m_len;}
		virtual int space() const         {return m_space;}
		virtual int spaceLeft() const     {return m_space-m_len;}
		
		// Resize the array to a different size. ALWAYS keep existing contents, and use
		// existing memory space if possible.
		virtual bool resize(int newLen);
		void set_size(int newLen) { m_len = newLen; } //XG
//		virtual bool resize(int newLen, int newSpace);
		virtual bool shrink(int by=1)				{return resize(len()-by);}
		virtual bool grow(int by=1)					{return resize(len()+by);}

		// Copy-construct one or more elements to the end of the array
		bool append(const T& t);
		bool append(const T* src, int n);

		// Copy-construct one element Uniquely to the array; if already exist return found index;
		int  addUnique(const T& t); // TBD.
		// Find the first match, from given start index.
		int  findFirst(const T& t, int istart=0);
		int  remove(const T& t);    // Remove ALL matched items

		// Return a pointer to the data area as a pointer to the T.
		T* begin()                {return m_data;}
		const T* begin() const    {return m_data;}
		T* end()                  {return m_data+m_len;}
		const T* end() const      {return m_data+m_len;}
		T* last()                 {return m_data+m_len-1;}//CAREFUL ! if m_data=0, m_len=0 then last()!=0
		const T* last() const     {return m_data+m_len-1;}

		// Access to any element as a T reference.
		T& operator[](int i)              {return m_data[i];}
		const T& operator[](int i) const  {return m_data[i];}
		T& element(int i)                 {return m_data[i];}
		const T& element(int i) const     {return m_data[i];}

		// Get common statistical property of the array. User has to make sure array is NOT empty!!!
		void getMinMax(T& min, T& max); 

	protected:
		//	Redefine to a new array. Old array objects are destroyed.
		virtual void redefine(T* data, int len, int space);
		// deallocate memory
		// To be compatible with stack Array<T,N>, do not check on m_data.
		inline void dealloc() { if(m_space>0) { delete [] m_data; m_space = 0;} }

		// ========= Common class interfaces  =========================
		public:
		typedef Array BaseClass;
		typedef NewClassID<37295, 0, ID_DLL> IDClass;
		// Run-time Stream I/O
		DSA_Export bool put(      Stream& stream, UChar ver) const;
		DSA_Export bool get(const Stream& stream, UChar ver);
		// ============================================================
	};


	// A rval buffer version of Array:
	// ArrayBuf can be used in any place that accept Array<>. Besides ArrayBuf<>
	// is a static buffer, using Cache and is faster.
	// TBD...
	// NOTE: ArrayBuf<> should NOT swap contents with Array<> ! (to avoid dangling pointer)
	// 

	// The Array<T,100> has MAXIMUM space 100 !
	template<typename T, int SPACE>
	class Array : public Array<T, 0>
	{
	public:
		// Data buffer
		T  data[SPACE];

		// Constructor
		Array() : Array<T, 0>() 
		{
			Array<T,0>::m_data = &data[0]; // Pointer to data buffer
			Array<T,0>::m_len  = 0;        // data length
			Array<T,0>::m_space = -SPACE;  // Indicate static buffer. (No Heap allocated).
		}
		// Destructor
		virtual ~Array() {Array<T,0>::m_data=NULL;}

		// Conversion:
		operator T*(){ return &data[0]; }
		// ========= Common class interfaces  =========================
		public:
		typedef Array<T,0> BaseClass;
		typedef NewClassID<56322, 0, ID_DLL> IDClass;
		// Run-time Stream I/O
		DSA_Export bool put(      Stream& stream, UChar ver) const;
		DSA_Export bool get(const Stream& stream, UChar ver);
		// ============================================================
	};

	// Find the min and max values of an array.
	template <typename T>
	void getMinMax(Array<T>& v, T& min, T& max);

} // End of namespace DSA

//	Define a placement new
//	Grammar--  ::new(Placement) typename [initializer]
struct NewPlacement {};
inline void* operator new(size_t /*size*/, NewPlacement* addr)	{ return addr; }
inline void  operator delete(void* /*p*/,  NewPlacement* /*addr*/)	{}

#include <DSA/Array.inl>

#endif