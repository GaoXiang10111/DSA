// ================= DSA DLL Files =====================
// File: ClassRegistry.h
// ClassRegistry holds static bonding info to manage classes in any DLL.
//
// XG	12/07/2010	Create
// =======================================================
// Note:
//
// static RegClass<MyConcreteClass>      regMyConcreteClass;
// static RegAbstractClass<MyAbstract>   regMyAbsract;

#ifndef DSA_CLASSREGISTRY_H
#define DSA_CLASSREGISTRY_H
#include <DSA/DSA.h>
#include <DSA/ClassID.h>
#include <DSA/Hash.h>
//#pragma hdrstop



namespace DSA
{

	// === Registry based design ===
	// Statically bond RTT functions:
	// Available RTT static function supports:
	struct AnyType;
	template<typename T>
	struct SPtr;
	class  Stream;

	// Run-time template API
	// Note: a MRTObj<S>[RTTHeader|S] 's CID (m_ID) can be different from its SPtr<T>'s CID, since T can be a base 
	// class type or "AnyType"! So, NEVER use SPtr<T>'s T::IDClass::CID to retrieve an MRTObj<S>'s registry information ! ! !
	struct ClassRegistry
	{
		typedef void  (*RTTDelete) (void* p);
		typedef bool  (*RTTIsA)    (ClassID const& cid);
		typedef void* (*RTTMake)   (); // Make MRTObj<T>
		typedef void* (*RTTCopy)   (void* ptr); // Copy T to MRTObj<T>
		typedef bool  (*RTTPut)    (SPtr<AnyType> const&, Stream&, UChar const ver);
		typedef bool  (*RTTGet)    (SPtr<AnyType>&, const Stream&, UChar const ver);
	public: 
		ClassRegistry(): destroy(0),isA(0),make(0),copy(0),put(0),get(0) {}
		ClassRegistry(ClassID const& id){ID = id;}

		ClassRegistry(RTTDelete de, RTTIsA is, RTTMake mk, RTTCopy cp, RTTPut pt, RTTGet gt) :
		destroy(de), isA(is), make(mk), copy(cp), put(pt), get(gt) {}

		// Members
		ClassID     ID;
		RTTDelete   destroy;
		RTTIsA      isA;   
		RTTMake     make;  // Make a new MRTObj<T>*
		RTTCopy     copy;  // Copy T into a new MRTObj<T>*
		RTTPut      put;
		RTTGet      get;
	};


	//
	// Class Registry Methods supported by one global 
	// static ClassRegistry hash table.
	// NOTE: Currently it supports AT MOST 2048 classes without collision. 
	//
	class DSA_Export ClassRegistryMethods
	{
	public:
		// Add one registry; If registered through SPtr<>
		DSA_Export static bool add(ClassRegistry& reg, bool allowReRegister=false);

		// Find one registry from class ID.
		DSA_Export static ClassRegistry* find(ClassID const& id);

		// Make managed class from a class ID
//		DSA_Export static Ptr<> make(ULong const& id);

		// Put a run-time ptr<> in a stream
//		DSA_Export static bool putPtr(Ptr<> const&, Stream&);

		// Get a run-time ptr<> from a stream
//		DSA_Export static bool getPtr(Ptr<> &, Stream const&);

		// IsValid check on ClassRegistry*
		// TBD: quickly check if a given registry* is owned by the hash table by
		// looking at its address space. Since the hash owns all its contents!
		// This could be used for a quick check if MRTObj has a valid Header.
		bool isPossible(ClassRegistry* t) { (void)t; return false; }

	private:
		// A static registry hash table
		static DSA_Export Hash<ClassID, ClassRegistry*> hashTable;
		// TBD. use ClassID::rnd(key) ??? and Warning on collisions???
		static inline int hashing(ClassID const& key) { return (key.idfull & 0x00FFFFFF) + key.tppid; }
		static inline bool matching(ClassID const& key0, ClassID const& key1) { return key0.isSame(key1);}
	};

		// ===== RTTHeader stores helper RTT routines ======
	// The RTT routines can also be stored in a global index table of classIDs (If they are 
	// sequential numbers produced by templates.) This design saves run-time memory usages
	// for Object instantiation. The global DSA registry holds ALL supported classes defined
	// by different translation units, from client DLLs other than DSA. The DSA registry
	// also performs run-time & compile-time checks for collisions, duplications of incorrect
	// ClassIDs accross the client DLLs.  [Powerful Design by Xiang Gao] 

	// TBD: With C++11, we can declare static object in class header. Then we can register 
	// class in the header file.  NOT the CPP file !!!

	// Search CID only ONCE per given CID. Avoids replicated searches at compile time...
	// In this way, we can catch any duplicated CID at compile time !
	// TBD. rename; since method is only used in MRTObj<>::make(), warning should always abort !!!
//	ClassRegistry* findClassRegistry(ClassID const& CID)
//	{
		// One-time-only static search
//		ClassRegistry* reg = ClassRegistryMethods::find(CID);
//		if( reg == 0 )
//		{
			// If class CID is not found in the DSA registry, meaning class is NOT
			// registered in their .cpp definition. Use an un-register class will make s.w.
			// unstable.
//			static char txt[128];
//			static Array<char,128> txt; 
//			sprintf_s(txt, 128, "Failed to find class registry [%d] for Library[%d]. Abort?", CID.rnd(), CID.dll() );
//			int yes = MessageBoxA(NULL, &txt[0], "Warning", MB_YESNO);
//			if( yes == IDYES )
//				abort();
//		}
//		return reg;
//	};
} // End of namespace DSA

//#include <DSA/Stream.inl>

#endif
