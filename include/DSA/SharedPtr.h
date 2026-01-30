// ================= DSA DLL Files =====================
// File: SharedPtr.h
// XG	11/04/2010	Create
// XG	12/02/2010	Combine run-time & compile-time shared pointers
// XG	06/17/2011	Use 4-point link design... 
// XG   02/12/2012	Use Hidden RT wrapper design (RTT)
// XG   11/11/2011  Tried design 4: Use AtomicCounter, use hasObj() check to detect cycles.
// XG	12/12/2011	Try compile time cycle detection
// XG	02/23/2012	Added WPtr<>
// XG	02/01/2013	Run-time & compile-time error check on Class registration.
// =======================================================
// To Do:
// 1) Support SPtr<T const>
// 2) Add a dumb pointer to unavoided cycles.
// 3) Figure out compile time cycle detection. 
//    a) Put class common methods, such as ID, in front of constructors?
//       Use the translation order embedded in class headers...? Not work for point to base class.
// 4) Add deepcopy() or copy(SPtr<S>const&);
// Note:
// 6/21/11  - - - Design thoughts - - -
// 1) Ptr<> class should be extremely light weight; while MRTObj<T> holder
//    class handles all the ownership relations and need not worry 
//    about size.
// 2) Holder<>
// --- Circular reference detection at class header (compile-time) ------
// 3) Class-header circular reference should be checked on Base classes:
//    A2 : A1 : A ;  B2 : B1 : B; {A1->B1; B2->A2} can produce a circular reference
// 4) Allow class-header circular reference, but check for run time non-heap references:
//    stackPtr<>;  // Used for scoped non-member ptr declaration;
//    memberPtr<>; // Used inside a class member header, or a function parameter
//    HeapObject has two counters for non-class ptr, and class member ptr.
//    when a non-member is out-of-scope, check the whole linked oject, see if other non-member
//    ptrs' (non-member) counts, if zero, delete the whole linked objets.
//    
///   class T; // Nonheap
//    {
//       virtual bool isOnHeap() { return false;}
//       P<T> p(this);          // isOnHeap() return true for New<T>.p !!!
//    }
//    template<typename T>
//    class New : public T  // Only call through Ptr<>.make(...) functions !
//    {
//       virtual bool isOnHeap() { return true;}
//       int   cnt1;
//       int   cnt2; 
//    }
// 5) When non-member ptrs are copy assigned, there is NO need to check and delete object !!!
//

/*
// ============================================================
// Shared Pointer for compile-time and run-time TYPEs
		// ========= Common class interfaces  =========================
		public:
		typedef NewClassID<12345,0,ID_DLL> IDClass;
		typedef Model BaseClass;
		// Run-time Stream I/O
		DSA_Export bool put(      Stream& stream, UChar ver) const;
		DSA_Export bool get(const Stream& stream, UChar ver);
		// ============================================================

		// Class also HAS TO be register by DSA::ClassRegistry through a static Class in cpp:
		// Static class registering
		static DSA::RegClass<MyClass> regMyClass;
*/

#ifndef DSA_SharedPtr_H
#define DSA_SharedPtr_H
#include <DSA/DSA.h>
#include <DSA/ClassID.h>
#include <DSA/ClassRegistry.h>
#include <atomic>
#include <cstdio>
//#include <DSA/String.h>
//#pragma
//#pragma warning(disable:4522)  // Turn off C4522 warning.

// TBD: 1/15(hrs)
// Need compile time ClassName to register a class, for template class that need different
// make/delete/cast functions for each specialization...
//		Array<double>; Array<FileName>
//		Matrix<float>;  Matrix<double>
// 
// Design 1:
// template class ID that also append with 3 or more specialization class IDs:
// Char[4](32Bit) ClassID, VerID; DLLID;  //Native class
// Char[4](32Bit) ClassID, VerID; DLLID;  //Specialization class
// Char[4](32Bit) ClassID, VerID; DLLID;  //Specialization2 class

// Design 2:
// Use Built-in complete class name (run-time or compile time), for hashing class types
// template class, will have "Template<Type>" as complete class name for hashing.
// 
// Use template class ID for hashing, then use specialized class name to avoid collision...
// This is OK, since each class specialization type only has one collision ONCE per registration.
// 
// OR, simply use class name for hashing, and keep the registry at each registration.
//
// Design 3:
// Use Complete class name by macro:
//                                   REGISTER_CLASS(BaseTest, NewClassID<12345,0,ID_DLL> );
//
//	// ========= Common class interfaces  =========================
//	typedef BaseTest BaseClass;
//	static const DSA::RTTRegistry registry; 
//	// Run-time Stream I/O
//	DSA_Export bool put(      DSA::Stream& stream, DSA::UChar ver) const {return false;}
//	DSA_Export bool get(const DSA::Stream& stream, DSA::UChar ver)       {return false; }
//	// ============================================================
//
// In this design, user can access STATIC class management methods directly from a static member from
// class objects. 
// 

namespace DSA
{
	// ============================================================
	// Atomic Counter (Cross-platform using std::atomic)
	// ============================================================
	struct AtomicCounter
	{
	public:
		// Constructor
		AtomicCounter(): m_cnt(0)        {}
		AtomicCounter(int z) : m_cnt(z)  {}

		// Methods
		bool isZero() const              { return m_cnt == 0;    }
		bool isNotZero() const           { return m_cnt != 0;    }
		inline int32_t cnt() const       { return m_cnt; }

		// Add n to counter and return the result
		inline int32_t add(int32_t n)    { return m_cnt.fetch_add(n) + n; }
		// Increment and return the result
		inline int32_t increment()       { return ++m_cnt; }

		// Subtract n from counter and return the result
		inline int32_t subtract(int32_t n) { return m_cnt.fetch_sub(n) - n; }
		// Decrement and return the result
		inline int32_t decrement()       { return --m_cnt; }
	private:
		std::atomic<int32_t>    m_cnt;
	};

	// XG 2/12/12
	// Latest Design ==========================
	// Hidden wrapper; use DSA global class registry, and error check; ...

	// ==============================================================
	// Run-Time Template routines. (Of course template does not work in run-time, but it is just a name !)
	// ==============================================================


	// ===== Run-time Routine Definition =====
	// Release resources // Should ONLY be called by SPtr<> or maker
//	typedef void (*RTTDelete) (void*); // TYPE Declaration
//	template<typename T>
//	static void rttDelete(void* p)    { delete static_cast<MRTObj<T>*>(p);  };





	// Note: a MRTObj<S>[RTTHeader|S] 's CID (m_ID) can be different from its SPtr<T>'s CID, since T can be a base 
	// class type or "AnyType"! So, NEVER use SPtr<T>'s T::IDClass::CID to retrieve an MRTObj<S>'s registry information ! ! !
	
	// RTT Header for Managed-Run-Time Object, with data structure:
	// [AtomicCounter, CID, ClassRegistry]  <-----   RTTHeader
	class RTTHeader
	{
	public:
		// template< class T >	RTTHeader() {}   // TBD C++ compiler does not fully support template in constructors!
		// Otherwise, I could use something like:
		//	template<ULong CID>
		//  RTTHeader() : m_cnt(), m_ID(CID)
		// {
		//	static ClassRegistry* reg = ClassRegistryMethods::find(CID);
		//	if(reg == 0)
		//	{}
		//	m_method = reg;
		// }
		// Similar are conversion functions. Here is a note from the C++ standards 2005 draft that is quite self-explantory: (section 14.8.1 paragraph 7)
		//[NOTE]
		// Note: because the explicit template argument list follows the function template name, and because conversion member function templates and constructor member function templates are called without using a function name, there is no way to provide an explicit template argument list for these function templates. �end note ]

		RTTHeader():m_cnt(){}
		RTTHeader(ClassID const& id, ClassRegistry* reg) : m_cnt(), m_ID(id), m_method(reg) {}

		inline ClassID const&  ID()  const      { return m_ID; }
		inline int32_t cnt() const              { return m_cnt.cnt(); }
		
		// Public methods for reference counting (used by SPtr and MRTObj)
		inline int32_t increment() { return m_cnt.increment(); }
		inline int32_t decrement() { return m_cnt.decrement(); }
		
	private:
		// Data members:
		mutable AtomicCounter     m_cnt;     // countings...
		ClassID                   m_ID;      // MRTObj<T>'s actual CID, which can be different from SPtr<S> 's CID ! 
		ClassRegistry*            m_method;  //TBD. Make it a reference??? ClassRegistry other DLLs loaded to DSA space

		// Since compiler does not support template constructor, I have to use member function 
		// instead... Ideally, I would like to use:
		// template<class T> RTTHeader(): m_delete(&rttDelete<T>), ...{} //   02/13/2012

		template < class U >
		friend struct SPtr;
		template < class U >
		friend class  MRTObj;
	};

	// A Helper EMPTY class to represent any class type.
	struct AnyType
	{
		typedef NewClassID<0,0,0> IDClass;
	};

	// Trait Wrapper for simplification of SPtr<> definition
	template<typename T>
	struct TypeInfo
	{
		typedef T* PTR;
		typedef typename T::BaseClass BaseClass; //If T is has compile time cycle, struct T {SPtr<T> t; }; compiler might issue error...
	};
	template<>
	struct TypeInfo<float>
	{
		typedef void* PTR;
		typedef float BaseClass;
	};
	template<>
	struct TypeInfo<double>
	{
		typedef void* PTR;
		typedef double BaseClass;
	};
	template<>
	struct TypeInfo<AnyType>
	{
		typedef void* PTR;
		typedef AnyType BaseClass;
	};
	template<>
	struct TypeInfo<AnyType const>
	{
		typedef void* PTR;
		typedef AnyType BaseClass;
	};

	// TBD rename to CommonClassTrait, OR CommonClassStaticMethods
	// TBD: add class Registering in MRTObj<> constructors ?!!!

	// Managed-Run-Time Object, with data structure:
	// [RTTHeader, T]  <-----   MRTObj<T>
	// [AtomicCounter, CID, ClassRegistry]  <-----   RTTHeader
	// 4/2/13: Could make class MRTObj<T> : public RTTHeader, public T {...} ??, in this
	// way, only need a static_cast<MRTObj<T> >( T& ); to access its header etc...
	// 1/20/14: Make ClassRegistry a static member of MRTObj<T> ???
	// 

	template<typename T>
	class MRTObj;  // Forward declaration

	template<typename T>
	struct ClassTrait_SPtr
	{
		template < class U >
		friend class  MRTObj;
		// Delete MRTObj<T>* from a Header pointer
		static void destroy(void* p)  { delete static_cast< MRTObj<T>*>(p); }

		// Check isA (isDerived from)
		//TBD. Merge with "bool rttIsA<T>(Long) " in SharedPtr.h?
		// Match Class and all its based class to the given ClassID.
		// Then also match the template parameters by their ids !!!
		// This works for template class:
		// ArrayDerived<double> : Array<double>
		// ArrayDerived<int>   NOT:   Array<float>
		static bool isA(ClassID const& id)
		{
			if( T::IDClass::get().isSame(id.idfull) )
			{
				return T::IDClass::get().isSameTPP(id.tppid); // Found match
			}
			else
			{
				if( TypeInfo<T>::BaseClass::IDClass::get().isSame(T::IDClass::get()) )
					return false; // Reached the top in the hierarchy
				else
					return ClassTrait_SPtr< typename TypeInfo<T>::BaseClass >::isA(id);
			}
		}

		// Make a new object
		static void* make()  { return (new MRTObj<T>()); } // Not dependent on SPtr.h.

		// Make a copy of given "T" object, (from its address void*)
		static void* copy(void* src)    { return (new MRTObj<T>(*(T*)src));	}

		friend struct Registry;
		template<typename U>
		friend struct ClassTrait_SPtr;
		template < class U >
		friend class  MRTObj;
	};

	template< typename T>
	class RegForSPtr : public ClassRegistry
	{
	public:
		// Class Registration through Constructor
		// A version number, base class registry are needed
		RegForSPtr()
		{
			ID      = T::IDClass::get();
			destroy = &(ClassTrait_SPtr<T>::destroy); //MRTObj<T> is always concrete
			isA     = &(ClassTrait_SPtr<T>::isA);
			make    = &(ClassTrait_SPtr<T>::make);    //MRTObj<T> is always concrete
//NO		copy    = &(ClassTrait_SPtr<T>::copy);    // Some Class disallow copy_construct...
//NO		put     = &(ClassTrait<T>::putPtr);
//NO		get     = &(ClassTrait<T>::getPtr);
			// Register this class
			ClassRegistryMethods::add(*this, true);
		};
	private:
		// Disallow copy/assignment
		RegForSPtr( RegForSPtr const& src )    {};
		void operator = ( RegForSPtr const& src ) {};
	};

	template< typename T>
	class RegClass;
	template< typename T>
	class RegNoCopyClass;

	template<typename T=AnyType>
	class MRTObj : public RTTHeader
	{
		T      t;

		// TBD Add compile time ClassRegistry check...
		// NOTE: ClassRegistry is RUN-TIME information, so this is a tough problem !!!
		// A MRTObj object requires certain classTraits needed for ClassRegistry.

		// NOTE: TBD, if findClassRegistry<ClassID> fails, it SHOULD register this class registry !!!!

		// TBD... Somehowever, ClassRegistry methods takes ClassID as key, while I only
		// used IDClass::CID... It has to be fixed !!!

	private: // Only friend can use MRTObject!
		MRTObj() : RTTHeader(T::IDClass::get(),registry()), t() {}//static RegForSPtr<T> reg; m_method = &reg;} //XG...`
//		MRTObj() : RTTHeader(T::IDClass::CID, regForSPtr() ), t() { }//static RegForSPtr<T> reg; m_method = &reg;} //XG...
		template< typename P1 >
		MRTObj(  P1 const& p1) : RTTHeader(T::IDClass::get(),registry()), t(p1){}
		template< typename P1,  typename P2 >
		MRTObj(  P1 const& p1, P2 const& p2) : RTTHeader(T::IDClass::get(),registry()), t(p1, p2){}
		template< typename P1,  typename P2,  typename P3 >
		MRTObj(  P1 const& p1, P2 const& p2, P3 const& p3) : RTTHeader(T::IDClass::get(),registry()), t(p1, p2, p3){}
		template< typename P1,  typename P2,  typename P3,  typename P4 >
		MRTObj(  P1 const& p1, P2 const& p2, P3 const& p3, P4 const& p4) : RTTHeader(T::IDClass::get(), registry()), t(p1, p2, p3, p4){}
		template< typename P1,  typename P2,  typename P3,  typename P4,  typename P5 >
		MRTObj(  P1 const& p1, P2 const& p2, P3 const& p3, P4 const& p4, P5 const& p5) : RTTHeader(T::IDClass::get(), registry()), t(p1, p2, p3, p4, p5){}
		~MRTObj(){}
	private:
		// All copy construction is disallowed
		MRTObj(T const& src) : RTTHeader(T::IDClass::get(), registry()), t(src) {}
//		MRTObj(T&       src) : RTTHeader(T::IDClass::get(), registry()), t(src) {}

		MRTObj<T>& operator=(MRTObj<T> const& src) { abort(); } // Never call this
		template < class U >
		friend struct SPtr;

		template < class U >
		friend struct ClassTrait;
		template < class U >
		friend struct ClassTrait_SPtr;

		template< typename U>
		friend class RegClass;
		template< typename U>
		friend class RegNoCopyClass;

		// static ClassRegistry for each "T" type
		static ClassRegistry* registry()
		{
			static RegForSPtr<T> reg;
			return &reg;
		};

		void verify()
		{
			if( !m_method )
			{
				static char txt[128];
				sprintf_s(txt, sizeof(txt)/sizeof(txt[0]), "Class[%d] Library[%d]is NULL in class registry", T::IDClass::get().dll(), T::IDClass::get().rnd() );
#ifdef _WIN32
				if( MessageBox(NULL, txt, "Developer's Warning", MB_YESNO) == IDYES)
					abort();
#else
				// On non-Windows platforms, just print to stderr
				fprintf(stderr, "Developer's Warning: %s\n", txt);
				abort();
#endif
			}
		};
	};


	// static ClassRegistry Info
//	template<typename T>
//	ClassRegistry MRTObj<T>::regForSPtr(&(ClassTrait<T>::destroy), &(ClassTrait<T>::isA),
//		&(ClassTrait<T>::make), &(ClassTrait<T>::copy), 0, 0); 

	// ==============================================================
	// Smart Shared Pointer, collectively owns pointed object.
	// ==============================================================
	// All cast, copy and assignment operation is guarded against operation on the same object! 
	// Including SPtr<> is cast to itself, and casting, copying among two shared SPtr<>s.
	// NOTE:
	// When using SPtr<T> as a function parameter, declare it as:
	// bool myFunc( SPtr<T> const & p1, SPtr<T> p2);
	// without "const", SPtr<T> can NOT be used as a reference to take a temporary variable!!!
	template<typename T=AnyType>
	struct SPtr
	{
	private:
		typedef  typename TypeInfo<T>::PTR   PtrT;
		PtrT              m_ptr; // ONE-AND-ONLY data member
	public:
		// Constructors
		SPtr() : m_ptr(0) {}

		// Copy construct from the same type; Allow implicit: SPtr<T> mP = srcPtr;
		SPtr( SPtr<T> const & src ) : m_ptr(0){ attach(src);}
		// Copy construct from different type
		template< typename S >
		SPtr( SPtr<S> const & src ) : m_ptr(0){ attach(src); }

		// Destructor
		~SPtr()                               { detach(); }

		// ===== Pointer Usages =====
		inline       PtrT ptr()               { return m_ptr; }
		inline const PtrT ptr()  const        { return m_ptr;  }
		inline       T&    operator*()        { return *m_ptr; }
		inline const T&    operator*()  const { return *m_ptr; }
		inline       PtrT  operator->()       { return m_ptr;  }
		inline const PtrT  operator->() const { return m_ptr;  }

		// Check null
		bool isNull()    const   { return m_ptr == 0; }
		bool isNotNull() const   { return m_ptr != 0; }

		// ===== Assignment     =====
		void operator = (SPtr<T> const& src) // Same type
		{
			if( src.m_ptr != m_ptr ) // Guard against assignment of the same object
			{detach(); attach(src);}
		};
		template<typename S>
		void operator = (SPtr<S>const& src)  // Different type  
		{ 
			if( src.m_ptr != m_ptr ) // Guard against assignment to the same object
			{detach(); attach(src);} // Compile time cast compability check
		}

		// ===== Swap objects between SPtr<T> of the SAME type "T" =====
		void swap(SPtr<T>& src)
		{
			T* tmp    = m_ptr;
			m_ptr     = src.m_ptr;
			src.m_ptr = tmp;
		}


		// ===== Pointer comparison     =====
		bool operator == (SPtr<T> const& src)       { return m_ptr == src.m_ptr; }
		bool operator == (SPtr<T> const& src) const { return m_ptr == src.m_ptr; }
		template<typename S>
		bool operator == (SPtr<S> const& src)       { return m_ptr == src.m_ptr; }
		template<typename S>
		bool operator == (SPtr<S> const& src) const { return m_ptr == src.m_ptr; }

		// Null
		inline void nullify()      { detach(); }

		// Try cast if Shared MRTObject's (src) can be cast into target SPtr<T>'s T, as follows:
		// Allowable case 1:  target SPtr<T>'s T is "AnyType"
		// Allowable case 2:  source MRTObject is or derived from target SPtr<T>'s T
		// Otherwise, null the pointer if cast is invalid.
		// Note: Always use the assignement operator directly, since that is much more efficient 
		// than trycast(), unless when cast down, i.e., from "AnyType", or "Base" into "Derived".
		template<typename S>
		SPtr<T>const& trycast(SPtr<S>const& src)
		{
			if(src.m_ptr != m_ptr) // Guard against casting of the same object
			{
				detach();
				if( src.m_ptr && (T::IDClass::CID == AnyType::IDClass::CID
					|| src.header()->m_method->isA( T::IDClass::get()) ) )// Use run-time isA() !!!
				{
					m_ptr = (PtrT)(src.m_ptr); // Cast from derived (src) into base. No need to use static_cast<PtrT>
					header()->increment();
				}
			}
			return *this;
		}

		// ========= Other ===================
		// Check if SPtr<T> held object is a given type or derived from it.
		inline bool isA(const ClassID& id) {return m_ptr? header()->m_method->isA(id) : false;}

		// Get MRTObject's actual ID at run-time, which could be different from the compile-time
		// type "T"!
		inline ClassID const& ID_object() const  { static ClassID nullID; return m_ptr? header()->ID() : nullID; }

		// Make object on the heap, by constructors =============
 		SPtr<T>& make()
		{
			detach(); // Must
			return attach( *(new MRTObj<T>()) );
		};

		// Convenient make() method invoking non-default constrs of "T(a), T(a,b), T(a,b,c)..."
		// It can invoke copy constructor as well. However the copy is always T(P1) type.
		// To make a copy as a derived Type [T|S], use pT.makeCopy( TS& ).
		template< typename P1 >
		SPtr<T>& make( P1 const& p1)
		{
			detach(); // Must
			return attach( *(new MRTObj<T>(p1)) );
		}
		template< typename P1, typename P2 >
		SPtr<T>& make( P1 const& p1, P2 const& p2)
		{
			detach(); // Must
			return attach( *(new MRTObj<T>(p1, p2)) );
		}
		template<       typename P1,  typename P2,  typename P3 >
		SPtr<T>& make( P1 const& p1, P2 const& p2, P3 const& p3)
		{
			detach(); // Must
			return attach( *(new MRTObj<T>(p1, p2, p3)) );
		}
		template< typename P1, typename P2, typename P3, typename P4 >
		SPtr<T>& make( P1 const& p1, P2 const& p2, P3 const& p3, P4 const& p4)
		{
			detach(); // Must
			return attach( *(new MRTObj<T>(p1, p2, p3, p4)) );
		}
		template< typename P1, typename P2, typename P3, typename P4, typename P5 >
		SPtr<T>& make( P1 const& p1, P2 const& p2, P3 const& p3, P4 const& p4, P5 const& p5)
		{
			detach(); // Must
			return attach( *(new MRTObj<T>(p1, p2, p3, p4, p5)) );
		}

		template<typename U>
		SPtr<T>& makeDerived()
		{
			detach();
			if(MRTObj<U>::registry()->isA(T::IDClass::get()))
			{
				SPtr<U> p;
				p.make();
				attach(p);
			}
			return *this;
		};

		// Copy and DeepCopy of the run-time object ===============

		// Make a shallow copy of the object it holds
		// Make a run-time (dynamic) correct (shallow) copy:
		// Objects: [A], [A|B}, [A|B|C...] are copied AS IS, although the SPtr<A> is still
		// only points to the base [A] object !!!
		SPtr<T>& makeCopy()
		{
			if( m_ptr )
			{
				RTTHeader* oldMRT = header();
				// Attach to NEW MRT
				attach( *(MRTObj<T>*)(header()->m_method->copy( m_ptr ) ) );
				// Now MUST detach/delete the OLD MRT
				if( oldMRT->decrement() <= 0 )
				{
					ClassRegistry::RTTDelete  clear(oldMRT->m_method->destroy);
					clear(oldMRT);
				}
			}
			return *this;
		}


		// Make FULL shallow copy of a given compatible object [S] itself.
		// "S" must be "T", or derived from "T". A derived T is copied as 
		// derived T!    SPtr<>.makeCopy( S )  works as well!
		template<typename S>
		SPtr<T>& makeCopy(S const& src)
		{
			if( (&src) != m_ptr ) // Self copy guard; 
			{
				detach();
				attach( *(new MRTObj<S>(src)) );
			}
			return *this;
		}

		// Copy construct from a SPtr<S>. S is compatible with "T", if not, a compile-time error
		// is generated due to non-compatible cast, such as casting from [*B] into [*BD]
		// ALSO, if "T" and "S" are compatible, makeCopy() is also run-time correct: meaning
		// source "S" type is makeCopy() as is, without change it to new "T" type.
		// SPtr<>.makeCopy(SPtr<S>) also works. 
		// Example:
		// pA   ->  [A], [A|B], [A|B|C]
		// pAB  ->  [A|B], [A|B|C]
		// pABC ->  [A|B|C]
		// pA.makeCopy( pAB );   // Compile-time OK. Run-time make a copy of [A|B|.]
		// pABC.makeCopy( pA );  // Compile-time error! [A] cannot cast into [A|B|C] !
		// pA1.makeCopy( pA2 );  // Compile OK. Run-time, pA2 can be [A], [A|B], [A|B|C], ... 
		template<typename S>
		SPtr<T>& makeCopy(SPtr<S> const& src) 
		{
			// First assign to the SRC MRTObject. For compile-time cast check!
			if( src.m_ptr != m_ptr ) // Guard against assignment of the same object
			{detach(); attach(src);}

			// Then make a shallow copy of the SRC MRTObject now it holds
			return makeCopy();
		}


		// Make a DEEP copy of the object it holds
		// Make a run-time (dynamic) correct (DEEP) copy:
		// Objects: [A], [A|B}, [A|B|C...] are copied AS IS, although the SPtr<A> is still
		// only points to the base [A] object !!!
		SPtr<T>& deepCopy()
		{
			if( m_ptr )
			{
				RTTHeader* oldMRT = header();
				// Deep copy
/*XG circular dep
 				static Stream st;
				st.rewind();
				st.put(*this);  // Put this
				st.rewind();

				// Attach to NEW MRT from default constructor
				attach( *(MRTObj<T>*)(oldMRT->m_method->make() ) );

				// Get from stream 
				st.get(*this);
*/
				// Now MUST detach/delete the old MRT
				if( oldMRT->decrement() <= 0 )
				{
					ClassRegistry::RTTDelete  clear(oldMRT->m_method->destroy);
					clear(oldMRT);
				}
			}
			return *this;
		}

		// Make FULL deep copy of a given compatible object [S] itself.
		// "S" must be "T", or derived from "T". A derived T is copied as 
		// derived T!    SPtr<>.makeCopy( S )  works as well!
		template<typename S>
		SPtr<T>& deepCopy(S const& src)
		{
/* citcular
			static Stream st;
			st.rewind();
			st.put(src);
			st.rewind();

			// Re-attach to a default "S" 
			detach();
			attach( *(new MRTObj<S>()) );

			// Get from memory buffer
			st.get(*this);
*/
			return *this;
		}

		// A deep copy from save/load (to memory buffer) operations.
		template<typename S>
		SPtr<T>& deepCopy(SPtr<S>& src) 
		{
			// First assign to the SRC MRTObject. For compile-time cast check!
			if( src.m_ptr != m_ptr ) // Guard against assignment of the same object
			{detach(); attach(src);}

			// Then make a DEEP copy of the SRC MRTObject now it holds
			return deepCopy();
		} // <<< TBD 

		// ===== Validity run-time check, see if object has correct header =====
		// For example: a base "pA" could points to a [A|D] object, and if that is
		// the case, return false; if a "pAE" points to [A|E] return true;
		bool isOriginalType() const  
		{
			return m_ptr? header()->ID() == T::IDClass::get() : true; // Check if header matches m_ptr
		}

	private:
		// [Base(this)]    <= [Derived (src)]; // OK
		// [Derived(this)] <= [Base (src)];    // Compile error casting error
		template<typename S>
		void attach(SPtr<S> const& src)
		{
			if(src.m_ptr)
			{
				m_ptr  =  src.m_ptr; // MUST use implicit cast. DO NOT use " (T*)src.m_ptr " !!!
//BAD           m_ptr  = (static_cast<PtrT>( src.m_ptr ) ); // Explicit cast is BAD! same as implicit
				header()->increment();
			}
		}

		template<typename S>
		SPtr<T>& attach(MRTObj<S>& src)
		{
			src.increment();
			m_ptr  =  &(src.t); // MUST use implicit cast. DO NOT use: " (T*)&src.t " !!!
//BAD		m_ptr = static_cast<TypeInfo<S>::PTR>(&src.t); // Re-algin from MRTObj<S> to S !
			return *this;
		}

		void detach() throw()
		{
			if( m_ptr )
			{
				RTTHeader* hdr = header();
				if( hdr->decrement() <= 0 )
				{
					if(hdr->m_method) // Use registry delete. (Not affected by class castings).
					{
						ClassRegistry::RTTDelete   clear(hdr->m_method->destroy);
						if(clear)
							clear(hdr); // Avoid delete abstract class
					}
					else
					{
//						delete (MRTObj<T>*)(hdr); //TBD
						m_ptr = 0;
					}
				}
				// Always null the pointer
				m_ptr = 0;
			}
		}
		// Do not allow assignment to dumb pointers. 
		void operator = (void* i)               { detach();  }

		// Helper
		// Be extremely careful: [1/12/2012]
		// [4/3/2012]: Could make MRTObj<T> derived from both Header AND T, and use static_cast<> to avoid 
		// the reinterpret_cast<>...
		inline RTTHeader*  header() const       { return reinterpret_cast<RTTHeader*>(m_ptr)-1;  }
		template < class U >
		friend struct SPtr;
		template < class U >
		friend struct WPtr;
		friend class Stream;
	};

	// ==============================================================
	// Weak(Watch) Pointer takes a SPtr<T> but does not own 
	// the SPtr<T> pointee
	// ==============================================================
	template<typename T>
	struct WPtr
	{
	private:
		typedef  T*   PtrT;
		PtrT              m_ptr; // ONLY data member
	public:
		// Constructors
		WPtr() : m_ptr(0) {}

		// Copy construct from the same type; Allow implicit: WPtr<T> mP = srcPtr;
		WPtr( SPtr<T> const & src ) : m_ptr(0) { attach(src);}
		// Copy construct from different type
//		template< typename S >
//		WPtr( SPtr<S> const & src ) : m_ptr(0) { attach(src); }

		// Destructor
		~WPtr() {} // Do nothing

		// ===== Pointer Usages =====
		inline  PtrT ptr()               { return m_ptr; }
		inline  PtrT ptr()  const        { return m_ptr;  }
		inline  T&    operator*()        { return *m_ptr; }
		inline const T&    operator*()  const { return *m_ptr; }
		inline  PtrT  operator->()       { return m_ptr;  }
		inline  PtrT  operator->() const { return m_ptr;  }

		// Lazy Check MRTObject's existence, not thread safe, MRTObject pointed should
		// NOT be destroyed, while WPtr<> is still accessing it...
		// Does not work if MRTObject is destroyed and then re-created at the same
		// memory location...
		bool exists()
		{
			if( m_ptr != 0 )
			{
				// Check MRTObject<T>'s header for consistency
				RTTHeader* hdr = header();
				if( hdr && hdr->ID() == T::IDClass::get() && hdr->cnt() > 0)
					return true;
				else
					m_ptr = 0;
			}
			return false;
		}

		// ===== Assignment     =====
		// MUST assignment to the same type...
		inline void operator = (WPtr<T> const& src) { m_ptr = src.m_ptr; };
		// Somehow if "B.h" has struct B{ WPtr<A> a; }; where "A" can ONLY be forward declared, not "A.h"
		// 
//		void operator = (SPtr<T> const& src) // Do not allow assignment from SPtr<ST>, header might not have ST defined yet... 
//		{
//			attach(src);
//		};
//		template<typename S>
//		void operator = (SPtr<S>const& src)  // Different type  
//		{ 
//			if( (void*)&src != (void*)this ) 
//			{detach(); attach(src);}
//		}
		// Comparison operators
		bool operator == (WPtr<T> const& src)       { return m_ptr == src.m_ptr; }
		bool operator == (WPtr<T> const& src) const { return m_ptr == src.m_ptr; }
		template<typename S>
		bool operator == (WPtr<S> const& src)       { return m_ptr == src.m_ptr; }
		template<typename S>
		bool operator == (WPtr<S> const& src) const { return m_ptr == src.m_ptr; }

		// Null
		inline void nullify()      { detach(); }

		// Try cast if Shared MRTObject's can be cast into target SPtr<T>'s T, as follows:
		// Allowable case 1:  target SPtr<T>'s T is "AnyType"
		// Allowable case 2:  Shared MRTObject is or derived from target SPtr<T>'s T
		// Otherwise, null the pointer if cast is invalid.
		// Note: Always use the assignement operator directly, since that is much more efficient 
		// than trycast(), unless when cast down, i.e., from "AnyType", or "Base" to "Derived".
		template<typename S>
		WPtr<T>& trycast(SPtr<S>const& src)
		{
			if(src.m_ptr != m_ptr) // Guard against casting of the same object
			{
				detach();
				if( src.m_ptr && (T::IDClass::CID == AnyType::IDClass::CID || 
					src.header()->m_method->isA(T::IDClass::get())) )
				{
					m_ptr = (static_cast<PtrT>(src.m_ptr));
				}
			}
			return *this;
		}

		// ========= Other ===================
		// Check if SPtr<T> held object is a given type or derived from it.
		inline bool isA(const ClassID& id) {return m_ptr? header()->m_method->isA(id) : false;}

		// Get MRTObject's original ID
		inline ClassID ID_original() const   {return m_ptr? header()->ID() : 0; }


		// ===== Validity run-time check, see if object has correct header =====
		bool isOriginalType() const 
		{
			return m_ptr? header()->ID() == m_ptr->ID : true; // Check if header matches m_ptr
		}

	private:
		// A base SPtr<B> can be attached to derived SPtr<BD>, if a derived SPtr<BD> is
		// attached to a base SPtr<B>, it produce compile error!
		template<typename S>
		void attach(SPtr<S> const& src)
		{
			m_ptr  =  src.m_ptr; // MUST use implicit cast
			// TBD. Add check sums to src.header()...
			// Add static counts to src.header() to keep track the total number of WPtr<> 
			// created...
		}
		void detach() throw() { m_ptr = 0; }

		// Do not allow assignment to dumb pointers. 
		void operator = (void* i)              { detach();  }

		// Helper
		// Be extremely careful: [1/12/2012]
		inline RTTHeader*  header() const       { return reinterpret_cast<RTTHeader*>(m_ptr)-1;  }
		template < class U >
		friend struct SPtr;
	};
	// 
	// End of Design #1 ================================================================
	//


	// Example classes for testing 
    struct DSA_Export BaseTest
    {
    public:
        // ========= Common class interfaces  =========================
        typedef DSA::NewClassID<12345,0,DSA::ID_DLL> IDClass;
        typedef BaseTest BaseClass;
    //	static const DSA::RTTRegistry registry; 
        // Run-time Stream I/O
        bool put(      DSA::Stream& /*stream*/, DSA::UChar /*ver*/) const {return false;}
        bool get(const DSA::Stream& /*stream*/, DSA::UChar /*ver*/)       {return false; }
        // ============================================================
    };

    struct DSA_Export DeriTest : public BaseTest
    {
    public:
        // ========= Common class interfaces  =========================
        typedef DSA::NewClassID<23456,0,DSA::ID_DLL> IDClass;
        typedef BaseTest BaseClass;
    //	static const DSA::RTTRegistry registry; 
        // Run-time Stream I/O
        bool put(      DSA::Stream& /*stream*/, DSA::UChar /*ver*/) const {return false;}
        bool get(const DSA::Stream& /*stream*/, DSA::UChar /*ver*/)       {return false; }
        // ============================================================
    };
    template<typename T>
    struct DSA_Export TemTest : public BaseTest
    {
    public:
        // ========= Common class interfaces  =========================
        typedef DSA::NewClassID<43456,0,DSA::ID_DLL> IDClass;
        typedef BaseTest BaseClass;
    //	static const DSA::RTTRegistry registry; 
        // Run-time Stream I/O
        bool put(      DSA::Stream& stream, DSA::UChar ver) const {return false;}
        bool get(const DSA::Stream& stream, DSA::UChar ver)       {return false;}
        // ============================================================
    };



} // End of namespace DSA

#endif
