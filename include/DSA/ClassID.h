// ================= DSA DLL Files =====================
// File: ClassID.h  
// Data Structure and Algorithm Library
// XG	07/14/2009	Created.
//	
// =======================================================
#ifndef DSA_CLASSID_H
#define DSA_CLASSID_H 1
#include <DSA/DSA.h> //Data types


namespace DSA
{

	// ----- Examples of using/declaring ClassID
//	struct MyClass
//	{
//		...
//		// ========= Common class interfaces  =========================
//	public:
//		typedef NewClassID<12345,0,ID_DLL> IDClass;
//	OR	typedef NewTemplateID<54321,0,0,TestArray,P1,P2> IDClass; // For TestArray<P1,P2> class
//		typedef MyClass BaseClass;
		// Run-time I/O on Stream
//		DSA_Export bool put(      Stream& stream, UChar ver) const;
//		DSA_Export bool get(const Stream& stream, UChar ver);
//		// ============================================================
//	}

#if defined(_MSC_VER)
// compile with Microsoft compiler
#pragma pack(push, DSA_ClassID_Packed, 1)
//#pragma pack(show)
#define	DSA_ClassID_PACKED

#elif defined(__GNUC__)
// compile with Altera tools
#define	DSA_ClassID_PACKED __attribute ((__packed__))
#else
// unknown compiler
#define	DSA_ClassID_PACKED
#endif

	struct ClassID
	{
		// Data Member:
		ULong  idfull; // FullClassID:   [Version(8b)|DLL(8b)|rand(16)]
		ULong  tppid;  // scrambled ids for template parameters

		// Constructors:
		explicit ClassID() : idfull(0), tppid(0)  {}
		explicit ClassID(ULong const& id, ULong const& tpp) : idfull(id), tppid(tpp)  {}

		// Copy and Assignment
		ClassID(ClassID const& src)        { idfull = src.idfull; tppid = src.tppid;}
		void operator=(ClassID const& src) { idfull = src.idfull; tppid = src.tppid;}

		// Check
		inline bool isSame(const ClassID& id2) const { return (idfull&0x00FFFFFF)==(id2.idfull&0x00FFFFFF) && (tppid==id2.tppid); }
		// IsSame (w/o check template parameters)
		inline bool isSame(const ULong id2)    const { return (idfull&0x00FFFFFF)==(id2&0x00FFFFFF); }
		inline bool isSameTPP(const ULong tpp) const { return tppid == tpp; }

		// Content subtraction
		inline UChar  version() const { return idfull>>24; }
		inline UShort rnd()     const { return idfull & 0x0000FFFF; }
		inline UChar  dll()     const { return (idfull & 0x00FF0000)>>16; }

	};

#if defined(_MSC_VER)
// compile with Microsoft compiler
#pragma pack(pop, DSA_ClassID_Packed)
//#pragma pack(show)
#endif


	// == ==  Static Template Type  == == 
	// A design to define a group of Enum, static const int, within one type defintion.
	// Use this ONLY AS part of a typedef:
	//       typedef NewClassID<ClassID,Version,dllID> myID;
	template<const UShort Rand, const UChar verID, const UChar dllID>
	struct NewClassID
	{
	public:
		// Declared "Tags":
		static const ULong CID     = (verID<<24|dllID<<16|Rand); // Complete ID
		static const ULong id      = (          dllID<<16|Rand); // ID w/o version
		static const UChar version = verID;                      // Version ID  
		// Template Parameter IDs
		static const ULong TPPID   = 0;

//		static const ClassID classid; // Wait for C++11 !!!
		// Now use static function...
		static const ClassID& get(){ static const ClassID m_id(CID,TPPID); return m_id; }

	private:
		// All constructor/destructor are private !!!
		NewClassID()  {}
		virtual ~NewClassID() {}
		NewClassID( NewClassID<Rand,verID,dllID> const& ) {}
		NewClassID<Rand,verID,dllID>& operator=(const NewClassID<Rand,verID,dllID>& src) {}
	};

	// Default NullType for NewTemplateID<>
	// Traits for C++ native types
	template<typename U>
	            struct TPInfo           { typedef typename U::IDClass    IDClass; };
	template<>	struct TPInfo<void>     { typedef NewClassID<0x0001,0,0> IDClass; };
	template<>	struct TPInfo<UChar>    { typedef NewClassID<0x0002,0,0> IDClass; };
	template<>	struct TPInfo<UShort>   { typedef NewClassID<0x0003,0,0> IDClass; };
	template<>	struct TPInfo<ULong>    { typedef NewClassID<0x0004,0,0> IDClass; };
	template<>	struct TPInfo<ULongLong>{ typedef NewClassID<0x0005,0,0> IDClass; };

	template<>	struct TPInfo<SChar>    { typedef NewClassID<0x0006,0,0> IDClass; };
	template<>	struct TPInfo<SShort>   { typedef NewClassID<0x0007,0,0> IDClass; };
	template<>	struct TPInfo<SLong>    { typedef NewClassID<0x0008,0,0> IDClass; };
	template<>	struct TPInfo<SLongLong>{ typedef NewClassID<0x0009,0,0> IDClass; };

	template<>	struct TPInfo<float>    { typedef NewClassID<0x0010,0,0> IDClass; };
	template<>	struct TPInfo<double>   { typedef NewClassID<0x0011,0,0> IDClass; };

	template<const UShort Rand, const UChar verID, const UChar dllID, typename T, typename P1, typename P2=void, typename P3=void>
	struct NewTemplateID : public NewClassID<Rand,verID,dllID>
	{
	public:
		// Template Parameter ID components: baseClass template parameter, P1,P2..., class ID, 
		// P1,P2.., template ID, 
		static const ULong TPPID = (TPInfo<P1>::IDClass::id) | (TPInfo<P2>::IDClass::id<<16) ^ (TPInfo<P3>::IDClass::id<<24);
//			T::BaseClass::IDClass::TPPID ^ 
//			((TPInfo<P1>::IDClass::id) ^ (TPInfo<P2>::IDClass::id<<3) ^ (TPInfo<P3>::IDClass::id<<6) ) ^ 
//			(TPInfo<P1>::IDClass::TPPID<<7)^(TPInfo<P2>::IDClass::TPPID<<9)^(TPInfo<P3>::IDClass::TPPID<<10);
		// TBD. with C++11, we can declare static const ClassID<> within the class header..., 
		// then we won't need this get() method !!!
//Linux		static const ClassID& get(){ static const ClassID m_id(CID, TPPID); return m_id; }

	private:
		// All constructor/destructor are private !!!
		NewTemplateID()  {}
		virtual ~NewTemplateID() {}
		NewTemplateID( NewTemplateID<Rand,verID,dllID,T,P1,P2,P3> const& ) {}
		NewTemplateID<Rand,verID,dllID,T,P1,P2,P3>& operator=(const NewTemplateID<Rand,verID,dllID,T,P1,P2,P3>& src) {}
	};

	// Char defined in 32bit/64bit bucket; for superior sorting, etc.
	// TBD>>> For class name etc...
//	template<int nChar>
//	struct BucketChars
//	{
//		typedef ULong Bucket;
//		Bucket   data[ceil(double(nChar)/double(sizeof(Bucket)))];
//	};

	// rand(16)rand(16)
//	typedef unsigned __int8   UChar;     // 1-byte (8-bit) unsigned int [0; 255]
//	typedef unsigned __int16  UShort;    // 2-byte (16-bit) unsigned int short [0; 65,535]
//	typedef unsigned __int32  ULong;     // 4-byte (32-bit) unsigned int long  [0; 4,294,967,295]
//	typedef unsigned __int64  ULongLong; // 8-byte (64-bit) unsigned int

}
#endif