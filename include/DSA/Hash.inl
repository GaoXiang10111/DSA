// ================= DSA DLL Files =====================
// File: Hash.inl
// XG  08/25/2009	Create
// =======================================================
// Note:
//

#ifndef DSA_HASH_INL
#define DSA_HASH_INL

namespace DSA
{
	template<class L, class C>
	typename Hash<L,C>::Entry* Hash<L,C>::add(L const& lbl)
	{
		typename Lists<Hash<L,C>::Entry>::LinkNext* nxt = nullptr;
		int i = hash(lbl) % SIZE; // Additional hash by "%SIZE"
		nxt = table.getList(i);
		while (nxt)
		{
			if(match(nxt->obj.label, lbl))
			{
				return &(nxt->obj);
			}
			nxt = table.getNext(*nxt);
		};
		// Now cannot find any match
		Entry* ent = table.insert(i);
		if(ent) ent->label = lbl;
		return ent;
	}

	template<class L, class C>
	bool Hash<L,C>::add(L const& key, C const& content)
	{
		return table.insert(Entry(key,content), hash(key)%SIZE );  // Additional hash by "%SIZE"
	}
	template<class L, class C>
	bool Hash<L,C>::add(Entry const& ent)
	{
		return table.insert(ent, hash(ent.label)%SIZE );  // Additional hash by "%SIZE"
	}

	// Delete one entry (by its label) from the Hash Table
	// Using "match()" to find matching entry
	template<class L, class C>
	bool Hash<L,C>::del(L  const& lbl)//  { table.remove(ent, hash(ent.label)); }
	{
		int i = hash(lbl) % SIZE;  // Additional hash by "%SIZE"
		typename Lists<Entry>::LinkNext* o = table.getList(i);
		if(o)
		{
			if(match(o->obj.label, lbl) )	// match on the 1st
			{
				table.popList(i);
				return true;
			}
			else
			{
				while(o)// match on next
				{
					typename Lists<Entry>::LinkNext* o2 = table.getNext(*o);
					if(o2 && match(o2->obj.label, lbl) )
					{
						table.popNext(*o);//remove the matched LinkNext
						return true;
					}
					o = o2;
				}
			}
		}
		return false;
	}

	template<class L, class C>
	typename Hash<L,C>::Entry* Hash<L,C>::find(L  const& key)
	{
		typename Lists<Entry>::LinkNext* o;
		int i = hash(key) % SIZE;  // Additional hash by "%SIZE"
		o = table.getList(i);
		while (o)
		{
			if(match(o->obj.label, key))
			{
				return &(o->obj);
			}
			o = table.getNext(*o);
		};
		return NULL;
	}

	template<class L, class C>
	C* Hash<L,C>::findContent(L  const& key)
	{
		typename Lists<Entry>::LinkNext* o;
		int i = hash(key) % SIZE;  // Additional hash by "%SIZE"
		o = table.getList(i);
		while (o)
		{
			if(match(o->obj.label, key))
			{
				return &(o->obj.content);
			}
			o = table.getNext(*o);
		};
		return NULL;
	}

	template<class L, class C>
	bool Hash<L,C>::findAll(L  const& key, Array<Entry>& res)
	{
		typename Lists<Entry>::LinkNext* o;
		res.resize(0);
		int i = hash(key) % SIZE;  // Additional hash by "%SIZE"
		o = table.getList(i);
		while (o)
		{
			if(match(o->obj.label, key))
			{
				res.grow();
				res.last() = o->obj;
//					return &(o->obj);
			}
			o = table.getNext(*o);
		};
		return res.len()>0;
	}

	template<class L, class C>
	int Hash<L,C>::collision(const L &key)
	{
		int i = hash(key) % SIZE; // Additional hash by "%SIZE"
		if( table.getList(i) )    // Collision
			return i;
		else
			return -1;            // No collision
	}



} // End of namespace DSA
#endif