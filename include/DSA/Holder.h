// ================= DSA DLL Files =====================
// File: Holder.h
// XG	04/22/2010	Create
// =======================================================
// Note:
//

#ifndef DSA_HOLDER_H
#define DSA_HOLDER_H
#include <DSA/DSA.h>
//#pragma
#include <time.h>          // time(NULL)
#include <math.h>
//#pragma warning(disable:4522)  // Turn off C4522 warning.

namespace DSA
{

	// NEW<T> facilitates constructing/destructing managed objects in heap.
	// Unlike smart pointers, NEW<> distinctively OWNS objects created in heap.
	// and it has a much smaller overhead. It's very similar to Boost's scope_ptr, 
	// but with a distinctive name "NEW" to highlight its purpose (to create new)
	// and the ownship.

	// For objects shared among units, threads, if object created by NEW<> is destroyed
	// all shared pointers should be null, thus make it safer to use pointers.
	// This can be achieved through an doubly linked LINK<T>.
	// When speed is a primary concern, user can always utilize the 
	// original pointer, accessable by NEW<T>::ptr()
	template < typename T> struct LINK;
	template < typename T>
	struct NEW
	{
		// Default Construct. No object is created.
		NEW() : ptr(0), nxt(0) {}

		// Copy Construct
		explicit NEW( T const& src) : nxt(0)       { ptr = new T(src); }
//		explicit NEW( NEW<T> const & src ): nxt(0) { ptr = new T(* src); }
		// Use "new" keyword. In case object donot have default constructors,
		// otherwise use make() instead.
		// Use as: NEW<T> myNewT( new T(p1,p2,...) );
		explicit NEW(T* p) : ptr(p), nxt(0) {} // Must from "new" command !

		// Destruct will release object.
		virtual ~NEW()	{ destroy(); 		}

		// Pointer operations
		T& operator*()  const { return *ptr; }
		T* operator->() const { return ptr;  }
		bool isNull()         { return ptr == 0; }

		// TBD: AVOIDS: Convert to T* (implicit conversion operator)
//		operator T*() {return ptr; } 

		// Swap	contents with another NEW objects
		// Faster than copying
		void swap(NEW<T>& src)  { T* tmp  = ptr; ptr = src.ptr; src.ptr = tmp; }

		// Make a new object
		// Make from default constructor
		void make() { destroy(); ptr = new T; }
		// Make by copying. 
		void make(T const& src) { destroy(); ptr = new T(src); }
 		// Make using "new" command. Allow specifying constructor parameters.
		void make(T* new_one)   { destroy(); ptr = new_one; } // Must from "new" command !

		// Destroy old object.
		void destroy() 
		{
			while(nxt)
			{
				nxt->p = NULL; 
				nxt = nxt->nxt;
			}
			if(ptr) { delete ptr; ptr = 0; }
		}

	private:
		// Pointer to new object:
		T* ptr;
 		mutable LINK<T>*  nxt;	// Next LINK<T>

		// Illegal assignments
		NEW<T>& operator= (T* p);
		NEW<T>& operator= (NEW<T> const&);
		friend LINK<T>;
	};

	// A link to object created by NEW<T>. LINK does not own objects !!!
	// Object life span is controlled by its scope. 
	// It can be used just like any C pointer type, when the original object 
	// is destroyed, all LINKs to the object are safely Null().
	template < typename T >
	struct LINK
	{
		T*        p;    // Object pointed to
		LINK<T>*  pre;  // Pre object;
		LINK<T>*  nxt;	// Next object

		// Construct
		LINK() : p(0), pre(0), nxt(0) {}
		explicit LINK( NEW<T> const & t):pre(0), nxt(0) { p = &t; }
		explicit LINK( NEW<T>&        t):pre(0), nxt(0) { p = &t; }

		// Delocate
		virtual ~LINK() 
		{
			// Detach from current links
			if(nxt)	nxt->pre = pre;
			if(pre)	pre->nxt = nxt;
			p = NULL; 
		}

		// Pointer operations
		T& operator*() const  { return *p;}
		T* operator->() const { return p; }
		T* ptr() const        { return p; }

		// IsNull
		bool isNull() const   { return p == 0; }

		// Assignment from the same type
		void operator = (LINK<T> const& src)    
		{
			// Detach from current links
			if(nxt)	nxt->pre = pre;
			if(pre)	pre->nxt = nxt;
			// Attach to next of src
			if(src.nxt)
			{
				src.nxt->pre = this;
				nxt = src.nxt;
			}
			nxt = this;
			pre = src;
			// Assign T*
			p = src.p; 
		}
//		void operator = (LINK<T>&       src)    { p = src.p; }
		// Assignment from NEW<T> object directly //TBD syntax might look odd.
		void operator = ( NEW<T> const&    src) 
		{
			// Detach from current links;
			if(nxt)	nxt->pre = pre;
			if(pre)	pre->nxt = nxt;

			// 
			if(src.nxt && src.nxt != this)
			{
				src.nxt->pre = this;
				nxt = src.nxt;
			}
			pre = NULL;
			src.nxt = this;

			p = src.ptr; 
		}
//		void operator = (NEW<T>&          src) { p = &src;  }
//		void pointTo( NEW<T> const& src)       { p = &src;  } // TBD better syntax

		// Use as a direct pointer, under rare circumstances.
//		void set(T* t)  { p = (NEW<T>*) t; }

		// Detach current chain of LINK
		// Attach to be the next of LINK.
		
	private:
		// Illegal assignments
		LINK<T>& operator = (T*);	 // Can not assign by direct-pointers
	};


	// Shared smart pointer. (No-intrusive. But beware of circular referencing).

	// Testing code...
	template<typename T>
	struct Combo
	{
		Combo() : t(), cnt(0){}
		Combo(T const& src) : t(src), cnt(0) {}
		virtual ~Combo() {}
		void cntIncrease() { cnt++;}
		void cntDecrease() { cnt--;}
		bool isCntZero()   { return cnt <= 0; }
		T*   ptr()         { return &t; }
		int size()         { return sizeof(Combo<T>); }
	private:
		mutable int cnt;
		T   t;
	};

	//TBD make a <typename T, int N=1>
	//TBD use attach first then detach, to avoid self pointing checking...
	//Smart pointer...
	template<typename T>
	struct SP
	{
		typedef Combo<T> ComboT;
		// Default Construct. No object is created.
		SP() : p0(0), p(0) {}

		// Copy Construct
		explicit SP( T const& src ) : p0(0), p(0)       { attach(*(new Combo<T>(src)) ); }
		explicit SP( SP<T> const & src ) : p0(0), p(0) { attach(*src.p0); }

		// Destruct will release object.
		virtual ~SP() { detachWithoutReset(); }

		// Pointer operations
		T& operator*()  const { return *p; }
		T* operator->() const { return p;  }

		// Assignments
		void operator= (SP<T> const&       tar)   
		{
			if(isPtrInsideComboT(this,tar.p0)) 
				return; //Avoid self pointing
			if(tar.isNull()) detach();
			else             attach(*tar.p0);
		}
//		void operator= (NULL) { detach(); }

		bool isNull() const        { return p == 0; }
		// Make null pointer.
		void makeNull() { detach(); }

		// Convert to T* (implicit conversion operator)
//		operator T*(){return p; }

		// Swap	with another pointer without detach/attach
		void swap(SP<T>& src)  
		{ 
			ComboT* tmp  = p0; p0 = src.p0; src.p0 = tmp;
			T*      tmp2  = p;  p  = src.p;  src.p  = tmp2;
		}

		// Make a new object
		// Make from default constructor
		void make() { attach( *(new ComboT) ); }
		// Make by copying. Also when need non-default construct to make a new object.  
		void make(T const& src) { attach( *(new ComboT(src)) ); }
 		// TBD?? Make using "new" command. Allow specifying constructor parameters.
//		void make(T* new_one)   { detach(); ptr = new_one; } // Must from "new" command !

		// Detach from current object.
		void detach()
		{ 
			if(p0)
			{
				p0->cntDecrease();
				if( p0->isCntZero() )
					delete p0;
				p0 = 0;
				p  = 0;
			}
		}
		void detachWithoutReset()
		{
			if(p0)
			{
				p0->cntDecrease();
				if( p0->isCntZero() ) delete p0;
			}
		}
		// Attach to existing combo object
		void attach(ComboT& combo)
		{
			detachWithoutReset(); // Detach from current object first
			p0 = &combo;
			if(p0)
			{
				p0->cntIncrease();
				p  = p0->ptr();
			}
			else
				p = 0;
		}
		bool isPtrInsideComboT(SP<T>* p, ComboT* combo)
		{
			void* begin = combo;
			if(begin==0) return false; //combo is null
			void* end   = combo + sizeof(ComboT);
			void* pAddr = p;
			if( pAddr >= begin && pAddr <= end)
				return true;
			else
				return false;
		}

	private:
		// Pointer to new object:
		Combo<T> *         p0;
		T mutable*         p;  // Direct access to pointer
	};

	// Return a static null() SP
	template<typename T, int N>
	struct CountedT
	{
		CountedT() : cnt(0) {}
		CountedT(T const& src) : cnt(0) { for(int i = 0; i < N; i++) t[i]=src;}
		virtual ~CountedT() {}
		void cntIncrease() { cnt++;}
		void cntDecrease() { cnt--;}
		bool isCntZero()   { return cnt <= 0; }
		T*   ptr(int i=0)    { return &t[i]; }
		T*   operator [](int i) { return &t[i]; }
	private:
		mutable int cnt;
		T   t[N];
	};

	// Smart Pointer to an array of type T[N]
	template<typename T, int N=1>
	struct PtrArray
	{
		typedef DSA::CountedT<T,N> CountedT;
		// Default Construct. No object is created.
		PtrArray() : p0(0) {}

		// Copy Construct
		explicit PtrArray( T const& src ) : p0(0)  { attach(*(new CountedT(src)) ); }
		explicit PtrArray( PtrArray<T> const & src ) : p0(0) { attach(*src.p0); }

		// Destruct will release object.
		virtual ~PtrArray() { detachWithoutReset(); }

		// Pointer operations
		T* operator [](int i) { return p0->operator [](i); }

		// Assignments
		void operator= (PtrArray<T> const&       tar)   
		{
			if(tar.isNull()) detach();
			else             attach(*tar.p0);
		}
		// Check if pointer is null
		bool isNull() const        { return p0 == 0; }
		// Make null pointer.
		void makeNull() { detach(); }

		// Swap	with another pointer without detach/attach
		void swap(PtrArray<T>& src)  
		{ 
			auto tmp  = p0; p0 = src.p0; src.p0 = tmp;
		}

		// Make a new object
		// Make from default constructor
		void make() { attach( *(new CountedT) ); }
		// Make by copying. Also when need non-default construct to make a new object.  
		void make(T const& src) { attach( *(new CountedT(src)) ); }
	protected:
		// Detach from current object.
		void detach()
		{ 
			if(p0)
			{
				p0->cntDecrease();
				if( p0->isCntZero() )
					delete p0;
				p0 = 0;
			}
		}
		void detachWithoutReset()
		{
			if(p0)
			{
				p0->cntDecrease();
				if( p0->isCntZero() ) delete p0;
			}
		}
		// Attach to existing combo object
		void attach(CountedT& combo)
		{
			detachWithoutReset(); // Detach from current object first
			p0 = &combo;
			if(p0)
			{
				p0->cntIncrease();
			}
		}

	private:
		// Pointer to new object:
		CountedT *         p0;
	};

	struct Wheel;
	struct Car
	{
		int type;
		Car() : type(0) {}
		Car(int t) : type(t) {}
		NEW<Wheel> w1;
		NEW<Wheel> w2;
		NEW<Wheel> w3;
		NEW<Wheel> w4;
		~Car()
		{
		}
	};
	struct Truck : Car
	{
		NEW<Wheel> w5;
		NEW<Wheel> w6;
	};
	struct Wheel
	{
		int size;
		Wheel() : size(23) {}
		Wheel(int d) : size(d) {}
		LINK<Car>  car;
	};


	
	//	(Undefined) Run-time Object, whose definition might not be defined until run time
	//	The RTO must support static ID:
	//	enum{ ID = 0x##### };  // static ID in the run time object class itself.
	//
	//	1) RTO does not have copy construct, or default construct of real object, since
	//	the real objects are always constructed in run time. Otherwise, i.e., if a class has to 
	//	be constructed in the cache, it should not be a RTO in the first place.

	//	2) Only references can be passed, when RTO is an argument in a function...

	//	FIXED TYPE at compile time and run time.
	//	Copy constructor is disallowed, since RTO doesnot need to be constructed on construction.
	//	rather it is constructed in run time...
	template< typename T = void>
	struct RTO
	{
		friend RTO<void>;
		RTO() : p(0), isOwner(false) {}
		~RTO() { clear(); }

		// Assign to the same type
		void operator= (RTO<T> const&  src)
		{
			// IGNORE shared RTOs.
			if( p == src.p)
				return;

			clear();
			p = src.p;
		}
/*2018		void operator= (RTO<void> const&  src)
		{
			// IGNORE shared RTOs.
			if( p == src.ptr<T>() )
				return;

			clear();
			if( T::IDClass::CID != src.type() ) // different type?
				return;
			
			p = src.ptr<T>();
		}
*/
		// Make from the default constructor
		void make() 
		{
			clear();
			p = new T(); 
			if(p)
			{
				isOwner = true;
			}
		}
		// Make by copying the existing object.
/* 2018: forming reference to void?
 		void make(T const& src)
		{
			clear();
			p = new T(src); 
			if(p)
			{
				isOwner = true;
			}
		}
*/
		// Make by copy construct another RTO.
		// The source RTO must be type "T"
		void make(RTO<T> const& src)
		{
			clear();
			// Copy from source
			if(src.p)
			{
				p = new T(*src.p); 
				if(p)
				{
					isOwner = true;
				}
			}
		}
		// Make by copy construct from another RTO
		template<typename U>
		void make(RTO<U> const& src)
		{
			clear();
			// Copy from source
			if(src.p)
			{
				p = new T(*(T*)(src.p) ); 
				if(p)
				{
					isOwner = true;
				}
			}
		}
 		// Make from "new" command, allowing constructor parameters.
		// :	rto.make<MyType>( new MyType(arg0, arg1, arg2) );
//		template< typename T >
		void makeFromNew(T* new_one) // MUST from "new" command !!!
		{
			clear();
			p = new_one;
			isOwner = true;
		} // Must from "new" command !

 		// Clear object.
		void clear() 
		{
			if(isOwner) 
				delete p; 
			p = NULL; 
			isOwner = false; 
		}

	private:
		T*      p;       // Direct access to pointer
		bool    isOwner; // ownership. TBD use a reference counter???
	};
	//	FLEXIBLE TYPE at compile time
	template< >
	struct RTO <void>
	{
		// Default Construct. No object is created.
		RTO() : t(0), p(0), isOwner(false) {}

		// Copy construct, Share the source object.
		// Make a make() method instead???
		explicit RTO( RTO const & src ) : isOwner(false) 
		{
			p = src.p;
			t = src.t;
		}

		// Destruct will clear/release object.
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wdelete-incomplete"
		~RTO() { if(isOwner) delete p; }  //XG 1/29/2026: TBD. delete void* p is undefined in C++
		#pragma GCC diagnostic pop

		// Assignments
		void operator= (RTO<> const&  src)
		{
			// IGNORE shared RTOs.
			if( p == src.p)
				return;

			clear();
			t = src.t;
			p = src.p;
		}
		template<typename T>
		void operator= (RTO<T> const&  src)
		{
			// IGNORE shared RTOs.
			if( p == src.p)
				return;

			clear();
			t = T::IDClass::CID;
			p = src.p;
		}

		// is null check
		bool isNull() const  { return (p == 0); }
		bool isOwned() const { return isOwner; }
	
		// Pointer operations
//		template< typename T >
//		T& operator*()  const { return *p; }

//		template< typename T >
//		const T& operator->() const { return *((T*)p);  }

		//
		// Unsafe but fast casting
		//
		// Direct cast, without type safety check
		// The following examples are tested:
		// :	rto.cast<MyType>()       . mytype_member;
		// :	rto.cast<const MyType>() . mytype_function(arg1);
		// :	rto.cast<MyType const>() . mytype_function(arg1);

		// :	rto.cast<MyType*>()       -> mytype_member;
		// :	rto.cast<MyType* const>() -> mytype_member;
		// :	rto.cast<const MyType*>() -> mytype_member;

		// :	rto.cast<MyType&>()       . mytype_member;
		// :	rto.cast<const MyType&>() . mytype_function(arg1);

		//	: Only reference/pointers are returned from cast<>()
		template<typename T>
		class CastInfo
		{
		public:
			typedef T&  RT;
			static  RT cast(void* p)  { return *( (T*)(p) ); }
		};
		// Pointer types
		template<typename T>
		class CastInfo< T* >
		{
		public:
			typedef T* RT;
			static RT cast(void* p)  { return (T*)(p); }
		};
		template<typename T>
		class CastInfo< const T* >
		{
		public:
			typedef const T* RT;
			static RT cast(void* p)  { return (const T*)(p); }
		};
		// Reference types
		template<typename T>
		class CastInfo<T&>
		{
		public:
			typedef T& RT;
			static  RT cast(void* p)  { return *( (T*)(p) ); }
		};

		// Unsafe cast, return reference/pointer.
		template< typename T >
		T cast() const { return CastInfo<T>::cast(p); }

		
		//
		// Safe conversion:
		//
		// Implicit type conversion:
		//	:	T* p = rto;
		//	:	((T*)rto) -> m_a;
		// Convert to T* (implicit conversion operator)
		template< typename T >
		operator T*() const 
		{ 
			if(t == T::IDClass::CID) 
				return (T*)p; 
			else
				return 0; // TBD>> if return a static "T", then sw might never crash...??
		}
		// Explicit pointer conversion
		template< typename T >
		T* ptr()  const 
		{
			if( t == T::IDClass::CID )
				return (T*)p; 
			else
				return 0;
		}

		// Check data type, return true if ID matches.
		template< typename T >
		bool isA() const
		{
			return t == T::IDClass::CID;
		}

		// Access the type/ID directly
		int type() const { return t; }

		// Swap	with another RTO
		void swap(RTO& src)  
		{
			// Shared case:
			if ( p == src.p )
				return;

			int tt = src.t;
			src.t = t;
			t = tt;

			void *pp = src.p;
			src.p = p;
			p = pp;

			bool b = src.isOwner;
			src.isOwner = isOwner;
			isOwner = b;
		}

		// Rescue a RTO that is about to be out of scope
		// Use with caution, copy using make(src) is always safer.
		// Or use swap() instead...
		void rescue( RTO& src )
		{
			if( p != src.p) // Not shared:
			{
				clear();
				
				// Copy data
				p = src.p;
				t = src.t;
			}

			// Change ownership
			src.isOwner = false;
			isOwner     = true;
		}

		//
		// Make a new RTO, the old object will be cleared, unless rescued by another RTO.
		//

		// Make from the default constructor
		template< typename T >
		void make() 
		{
			clear();
			p = (void *) new T(); 
			if(p)
			{
				t = T::IDClass::CID;
				isOwner = true;
			}
		}
		// Make by copying the existing object.
		template< typename T >
		void make(T const& src)
		{
			clear();
			p = (void *) new T(src); 
			if(p)
			{
				t = T::IDClass::CID;
				isOwner = true;
			}
		}
		// Make by copy construct another RTO.
		// The source RTO must be type "T"
		template< typename T >
		void make(RTO const& src)
		{
			clear();
			// Copy from source
			T* srcPtr = src;
			if(srcPtr)
			{
				p = (void *) new T(*srcPtr); 
				if(p)
				{
					t = T::IDClass::CID;
					isOwner = true;
				}
			}
		}
 		// Make from "new" command, allowing constructor parameters.
		// :	rto.make<MyType>( new MyType(arg0, arg1, arg2) );
		template< typename T >
		void make(T* new_one) // MUST from "new" command !!!
		{
			clear();
			t = T::IDClass::CID;
			p = new_one;
			isOwner = true;
		} // Must from "new" command !

 		// Clear object.
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wdelete-incomplete"
		void clear() 
		{
			if(isOwner) 
				delete p;  //XG 1/29/2026: TBD. delete void* p is undefined in C++
			p = NULL; 
			t = 0; 
			isOwner = false; 
		}
		#pragma GCC diagnostic pop
		template<typename T>
		friend struct RTO;
	private:
		int                t;       // Registered RTO types...
		void mutable*      p;       // Direct access to pointer
		bool               isOwner; // ownership. TBD use a reference counter???
	};


	// 3/18/2011: A simple and efficient Handle/Link (shared pointer) type.
	// BUT it has problem with the following case:
/* {
	 Link<T> outside;
	 {
		 Handle<T> myHandle;
		 myHandle.make();
		 myHandle->myFunction();

		 outside = myHandle;
	 }
	 if( !outside.isNull() )
		 outside->myFunction(); // Handle is not updated as NULL !!!
 }
*/
	// If above problem is truly bothersome, we can use the precedent "NEW"-"LINK" pair.
	// Otherwise, the code below is extremely simple. and can be used for
	// framework, if user is able to set Handle and Link's scope correctly...
	// Another advantage with this design, is that "Handle" can be customized to store 
	// static information of class types, such as ClassID, default build function etc...
	// 
	// It is also not very difficult to debug the crash, since the problem is caused by outscope handle...
	template< typename T >
	struct Handle_0
	{
		T* p;
		// ======================
		Handle_0() : p(0) {}
		~Handle_0()
		{
			clear();
		}

		// Swap with another handle...
		// Replace by another handle:
		// Clear current objet, and then 
		// Pointer operations
		T& operator*()  const { return *p; }
		T* operator->() const { return p;  }
		bool isNull()   const { return p == 0; }

		// Make a new object
		void make() 
		{
			clear();
			p = (T *) new T();
		}
		//
		void clear()
		{
			if( p ) // Only once can object can be deleted...
			{
				delete p;
				p = 0;
			}
		}
	};

	// "SHARED" Link to any type "T", through the handle of "T".
	template< typename T >
	struct Link
	{
	public:
		Link<T>* self;
		Link() : handle(0) 
		{
			self = this;
		}
		~Link() 
		{ }

		// Pointer operator
//		T& operator*()  const { return *p; }
		// When certain that handle is NOT null.
		T* operator->() const { return handle->p; }
		T* ptr() const 	{ return handle ? handle->p : 0; }
		
		bool isNull() const   { return handle == 0 || handle->isNull(); }

		// Take the ownership from another holder
		Link<T>& share( Link<T> const & src ) {return *this;}

		// Assign to the same type, i.e. share the same "Handled" object.
		void operator= (Link<T> const&  src)
		{
			handle = src.handle;
		}
		// Assign to a given Handle of the same type.
		void operator= ( Handle_0<T>const& src )
		{
			handle = &src;
		}
	private:
//		friend H<void>;
		const Handle_0<T>*    handle;
	};


	struct TestPH
	{
		TestPH() : h() 
		{
		}
		~TestPH() {}

		Link<TestPH> h;
	};

} // End of namespace DSA

//#include <DSA/Fitting.inl>

#endif
