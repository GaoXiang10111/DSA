// ================= DSA DLL Files =====================
// File: ClassRegistry.cpp
// ClassRegistry holds static bonding info to manage classes in any DLL.
//
// XG	08/12/2010	Create
// =======================================================
// Note:
//
#include <DSA/ClassRegistry.h>


namespace DSA
{
	// Construct the global registry hash table
	Hash<ClassID, ClassRegistry*> ClassRegistryMethods::hashTable
	(
		&(ClassRegistryMethods::hashing),
		&(ClassRegistryMethods::matching),
		2048,
		2048
	);

	// Add one class registry. Issue warning if the registry cause collision in the hash table!
	bool ClassRegistryMethods::add(ClassRegistry& reg, bool allowReRegister) 
	{
		static char txt[128];
		// Run-time (DLL loading time) check on Hash collision for best DLL performance:
		if( hashTable.collision(reg.ID) >= 0 )
		{
			if( !hashTable.find(reg.ID) ) // If found identical matched CID
			{
				snprintf(txt, sizeof(txt)/sizeof(txt[0]), "Class[%d] Library[%d]collision in class registry! Abort?", reg.ID.rnd(), reg.ID.dll());
//MS			sprintf_s(txt, sizeof(txt)/sizeof(txt[0]), "Class[%d] Library[%d]collision in class registry! Abort?", reg.ID.rnd(), reg.ID.dll());
				printf("Warning: %s\n", txt);
//				if( MessageBox(NULL, txt, "Developer's Warning", MB_YESNO) == IDYES) 
					abort();
			}
			else if(!allowReRegister)
			{
				snprintf(txt, sizeof(txt)/sizeof(txt[0]), "Class[%d] Library[%d] duplicated in class registry! Abort?!", reg.ID.rnd(), reg.ID.dll() );
//MS				sprintf_s(txt, sizeof(txt)/sizeof(txt[0]), "Class[%d] Library[%d] duplicated in class registry! Abort?!", reg.ID.rnd(), reg.ID.dll() );
				printf("Warning: %s\n", txt);
//				if( MessageBox(NULL, txt, "Developer's Warning", MB_YESNO) == IDYES )
					abort(); // Abort to avoid potential hazardous code
			}
		}
		Hash<ClassID, ClassRegistry*>::Entry* ent = hashTable.add(reg.ID);
		if( ent )
		{
			ClassRegistry* cr = ent->content;
			if( ! cr )
				(ent->content) = &reg;
			else
			{
				// Additive update
				if(reg.destroy) cr->destroy = reg.destroy;
				if(reg.isA)     cr->isA     = reg.isA;
				if(reg.make)    cr->make    = reg.make;
				if(reg.copy)    cr->copy    = reg.copy;
				if(reg.put)     cr->put     = reg.put;
				if(reg.get)     cr->get     = reg.get;
			}
			return true;
		}
		return false;
	}

	// Find one registry
	ClassRegistry* ClassRegistryMethods::find(ClassID const& id)
	{
		Hash<ClassID, ClassRegistry*>::Entry* ent = hashTable.find(id);
		if( ent )
			return ent->content;
		else
			return 0;
	}

	// Make managed class from a class ID
/*	Ptr<> ClassRegistryMethods::make(ULong const& id)
	{
		Hash<ULong, ClassRegistry*>::Entry* ent = hashTable.find(id);
		if( ent && ent->content)
			return ent->content->maker();
		else
		{
			Ptr<> p;
			return p;
		}
	}

	bool ClassRegistryMethods::putPtr(DSA::Ptr<> const& p, Stream & stream)
	{
		Hash<ULong, ClassRegistry*>::Entry* ent = hashTable.find(p.typeID());
		if( ent && ent->content)
			return ent->content->put(p,stream);
		else
			return false;
	}

	bool ClassRegistryMethods::getPtr(DSA::Ptr<> &p, Stream const& stream)
	{
		Hash<ULong, ClassRegistry*>::Entry* ent = hashTable.find(p.typeID());
		if( ent && ent->content)
			return ent->content->get(p,stream);
		else
			return false;
	}
*/
} // End of namespace DSA
