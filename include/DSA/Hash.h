// ================= DSA DLL Files =====================
// File: Hash.h
// XG  07/22/2009	Create
// =======================================================
// Note:
//

#ifndef DSA_HASH_H
#define DSA_HASH_H

#include <DSA/Array.h>
#include <DSA/List.h>

namespace DSA
{
	// HashTable Entry Type	(Analog to a drawer in a cabinet).
	template<class L, class C>
	struct HashEntry
	{
	public:
		L	label;
		C	content;
		HashEntry() : label(), content() {};
		HashEntry(L l, C c) : label(l), content(c) {};
		bool operator ==(HashEntry<L,C> const& he) { return label == he.label; }
		virtual ~HashEntry() { };
	};

	// Open Hash Class Template
	// L: Label, C: Content
	template<class L, class C>
	class Hash
	{
	public:
		// Hash function output should be (unsigned int).  
		// The returned hash value WILL be hashed AGAIN by "%SIZE" to be [0, SIZE-1],
		// so there is NO need to make sure that return hash value to be in range.
		typedef int (*Hashing) (L const&);
		typedef bool(*Matching)(L const&, L const&); // Matching function for labels
		typedef HashEntry<L,C> Entry;

	public:
		// Constructors:
		Hash(){};
		// Given hash function and a key matching function.
		// Number of entries should be smaller than hashSize. the Keys and hashing function
		// are designed in such a way that clision doesn't happen, hashSize can be close to 
		// number of entries... otherwise make hashSize twice as large of nEntryMax.
		Hash(Hashing f, Matching m): hash(f), match(m), SIZE(0) { table.alloc(0,SIZE);};
		Hash(Hashing f, Matching m, int nEntryMax, unsigned int hashSize)//: hash(f), match(m), SIZE(hashSize)
		{hash = f; match = m; SIZE = hashSize; table.alloc(nEntryMax,SIZE); };

		// Given hash function and table directly
//		Hash(function f, const Array<Entry>& tab) : table(tab) { hash = f; }

		// Allocate the table
		// NOTE: hasSize MUST equal to the "SIZE" in Hash function ! ! !
		bool allocate(int nMaxEntry, unsigned int hashSize) { SIZE = hashSize; return table.alloc(nMaxEntry, hashSize); }

		// Access hash table size
		int getNumMaxEntry() { return table.totalSpace(); }
		int getHashSize()    { return table.numLists();   }	// Should be the same as "SIZE"
		unsigned int hashSize() { return SIZE; }            // Same value as getHashSize()

		// Destructor
		virtual ~Hash() {};

		// Add one lable into the Hash, leaving content to be defined later.
		// if key matches existing keys, the matched entry is returned.
		// If hashTable is full, return 0!
		Entry* add(L const&); //

		// Insert (label, content) pair as a new entry. Duplicated label will replace the content:
		bool add(L const& key, C const& content);

		// Add One Entry into the Hash. Entries with the same labels are logged regardless.
		bool add(Entry const& ent);

		// Delete one entry (by label) from the Hash Table
		// Using "match()" to find matching entry whose label matches the given label.
		// return false, if there is no match to the given key.
		bool del(L const& key);//  { table.remove(ent, hash(ent.label)); }

		// Find entry whose label matches the given "key".
		// return NULL if cannot find any match
		Entry* find(L const& key);//
		//TBD. the indexing operator conformal to the hash_map in standard library.
		C*     operator[](L const& key) { Entry* found = find(key); return found? &found->content : 0; }

		// Find the content whose label matches the given "key"
		C* findContent(L const& key); 

		// TBD:
		bool findAll(L const& key, Array<Entry>& res);

		// Check if the given key collide with current table, return collided Hash Entry index
		// Return -1, if no collision is found.
		int collision(L const& key); // Very fast

	protected:
		// Hash Table as a ListS of entries
		Lists<Entry>  table; // Hash table

	private:
		unsigned int SIZE;   // hash size
		Hashing	     hash;   // hash function  
		Matching     match;	 // match function
	};

} // End of namespace DSA


#include <DSA/Hash.inl>
#endif