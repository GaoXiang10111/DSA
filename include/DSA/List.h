// ================= DSA DLL Files =====================
// File: List.h
// XG  07/14/2009	Create
// =======================================================
// Note:
//

#ifndef DSA_LIST_H
#define DSA_LIST_H

#include <list>
namespace DSA
{
	// Last Edit: 08/19/2009
	// A Table and Cursor Based Lists(Array) Implementation
	// Every list in the lists(array) share the same data for best memory efficiency.
	// It can also be used as one single list.
	template<typename OBJ>
	class Lists
	{
	public:
		typedef struct linkNext // LinkNext data type
		{
		public:
			int next;
			OBJ obj;
			linkNext()  {}
			~linkNext() {}
		} LinkNext;

		Lists()  { avail = -1; }
		virtual ~Lists() {}
		// allocate size of lists.
		// len is the number of list; maxEntry is the space reserved for entries for all lists.
		bool alloc(int maxEntry, int len=1);

		// access list info:
		int totalSpace() {return log.space(); }
		int numLists()   {return list.len();  }

		// Insert to the front of list #i: // Faster than insert to the end...
		bool insert(OBJ const& o, int i = 0);
		OBJ* insert(int i = 0); // Return the undefined object 

		// Insert to end of list #i	//Less efficient than insert()
		bool insertAtEnd(OBJ const& o, int i = 0);
		OBJ* insertAtEnd(int i = 0); // Return the undefined object

		// delete/remove from list #i  // Must have ==(OBJ,OBJ) defined ! ! !
		bool remove(OBJ const& o, int i = 0);
		// locate query	in list #i [operator == for OBJ must be defined]. "i" MUST be [0 len)
		// Used for check if list #i already has given obj
		OBJ* locate(OBJ const& o, int i = 0);

		// ======  Access objects on the list =======
		// Get the 1st object on list #i
		LinkNext* getList(int i=0)    { if(i>=0 && i<list.len()) return list[i]>=0? &(log[list[i]]) : 0; else return 0; }
		// Get the next object after the given LinkNext
		LinkNext* getNext(const LinkNext& o) { return o.next >=0? &(log[o.next])  : 0; }
		// Get the last object on list #i. Null on empty list.
		LinkNext* getLast(int i=0);

		// Remove all list elements
		void clearList(int i=0) 
		{
			LinkNext* last = getLast(i); 
			if(last) {last->next = avail; avail = log[list[i]];	list[i]=-1;}
		}
		// pop-out the 1st element on list #i
		void popList(int i=0) {if(list[i]>=0) { int j = list[i]; list[i]=log[j].next; log[j].next = avail; avail = j; } }
		// pop-out the element next(after) to given LinkNext
		void popNext(LinkNext& o) 
		{
			int k = o.next; 
			if(k>=0) 
			{
				o.next = log[k].next;
				log[k].next = avail;
				avail = k;
			}
		}


		// Count objects on list #i
		int  length(int i=0);


	private:
		Array<int>       list;	// List item(s).
		Array<LinkNext>  log;   // Shared data entries/strage for all lists.
		int              avail; // Next available space in "log"
		int growLog(int n);     // When number of entries exceeds limits.
	};

//	std::list<int> lst;	//test

	// TBD:
	// Cache version
	// N is the number of list. OBJ is the type of list objects.
	// M is the total number of list items reserved...
	template<int N, typename OBJ, int M>
	class ListsCache
	{
		struct Linked // Linked storage
		{
			int next;
			OBJ obj;
			Linked(){};
			~Linked();
		};

		ListsCache()
		{
			for(int i = 0; i < M-1; i++)
				log[i].next = i+1; 
			log[M-1].next = -1;
			available = 0;
		}
		// add to list index
		bool add(int i, const OBJ o);
		// delete/remove from list index i
		bool del(int i, const OBJ o);
		// locate query
		int find(int i, const OBJ o);

	private:
		int      list[N];
		Linked   log[M]; // an linked
		int      available;// 
	};



} // End of namespace DSA


#include <DSA/List.inl>
#endif