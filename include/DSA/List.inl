// ================= DSA DLL Files =====================
// File: List.inl
// XG  07/14/2009	Create
// =======================================================
// Note:
//

#ifndef DSA_LIST_INL
#define DSA_LIST_INL

namespace DSA
{
	// A Table/Cursor Based List(Array) Implementation:
	template<typename OBJ>
	bool Lists<OBJ>::alloc(int maxEntry, int len)
	{
		if(list.alloc(len, len) && log.alloc(maxEntry, maxEntry) )
		{
			list = -1;

			for(int i = 0; i < log.space(); i++)
				log[i].next = i+1;
			//if(log.space()>0) 
			if(log.begin()) // log must be valid 
			{
				log.last()->next = -1;
				avail = 0;
			}
			else
				avail = -1;
			return true;
		}
		return false;
	};

	// N is the number of list. OBJ is the type of list objects.
	// M is the total number of list items reserved...

	// Insert to the front of list #i: // Faster than insert to the end...
	template<typename OBJ>
	bool Lists<OBJ>::insert(OBJ const& o, int i)
	{
		if(avail < 0) growLog( log.space()+1 );

		if(i >= 0 && i < list.len() )
		{
			// Get avail's put infront of the existing log.
			int nextAvail = log[avail].next;

			log[avail].obj  = o;
			log[avail].next = list[i];

			list[i]         = avail;
			avail = nextAvail;
			return true;
		}
		return false;
	};
	// Similiar to above, but inserted object can be fined in a separate step.
	template<typename OBJ>
	OBJ* Lists<OBJ>::insert(int i)
	{
		if(avail < 0) growLog( log.space()+1 );

		if( i >= 0 && i < list.len() )
		{
			// Get avail's put infront of the existing log.
			int nextAvail = log[avail].next;

//			log[avail].obj  = o;
			OBJ* ob = & (log[avail].obj) ; //undefined object
			log[avail].next = list[i];

			list[i]         = avail;
			avail = nextAvail;
			return ob;
		}
		return 0;
	};

	template<typename OBJ>
	bool Lists<OBJ>::insertAtEnd(OBJ const& o, int i)
	{
		if( avail < 0) growLog( log.space()+1 );

		if( i >= 0 && i < list.len() )
		{
			// Get to the last
			LinkNext* p = getLast(i);
			// Register the new object's starting point
			if(p)
				p->next = avail;
			else
				list[i] = avail;

			// Update log
			int nextAvail = log[avail].next;
			log[avail].obj  = o;
			log[avail].next = -1;
			avail = nextAvail;
		}
		return false;
	};
	// Similiar to above, but inserted object can be fined in a separate step.
	template<typename OBJ>
	OBJ* Lists<OBJ>::insertAtEnd(int i)
	{
		if(avail < 0) growLog( log.space()+1 );

		if( i >= 0 && i < list.len() )
		{
			// Get to the last
			LinkNext* p = getLast(i);
			// Register the new object's starting point
			if(p)
				p->next = avail;
			else
				list[i] = avail;

			// Update log
			int nextAvail = log[avail].next;
//			log[avail].obj  = o;
			log[avail].next = -1;
			avail = nextAvail;
			return &(log[avail].obj);
		}
		return 0;
	};

	// delete/remove from list index i
	template<typename OBJ>
	bool Lists<OBJ>::remove(OBJ const& o, int i)
	{
		if( i < 0 || i >= list.len())
			return false;
		if( list[i] < 0) 
			return false;

		int j = list[i];
		// Check the first one.
		if(log[j].obj == o)
		{
			list[i] = log[j].next;   // parent->grandchild
			log[j].next = avail;	 // update avail
			avail = j;
			return true;
		}
		else
		{
			// Find the one whose next is o
			while( log[j].next >= 0 && !(log[log[j].next].obj == o) ) j= log[j].next;
			if(j>=0)
			{
				int k = log[j].next;
				log[j].next = log[k].next;   // parent->grandchild
				log[k].next = avail;		 // update avail
				avail = k;
				return true;
			}
		}
		return false;
	}

	// locate query
	template<typename OBJ>
	OBJ*  Lists<OBJ>::locate(OBJ  const& o, int i)
	{
		LinkNext* np = getList(i);
		while(np)
		{
			if( np->obj == o)
			{
				return &(np->obj); // found the match
			}
			np = getNext(*np);
		};
		return 0; // No match
	}

	// Get the last object on list #i. Null on empty list.
	template<typename OBJ>
	typename Lists<OBJ>::LinkNext* Lists<OBJ>::getLast(int i)
	{
		Lists<OBJ>::LinkNext* np = getList(i);
		while(np)
		{
			if(np->next < 0) return np;
			np = getNext(*np);
		};
		return 0;
	}

	// Get the object in front of the given object
//	template<typename OBJ>
//	typename Lists<OBJ>::LinkNext* Lists<OBJ>::getInFrontOf(const OBJ &o, int i)
//	{
//		Lists<OBJ>::LinkNext* np = getList(i);
//		while(np && np->next >= 0)
//		{
//			if(log[np->next]==o) return np;
//			np = getNext(*np);
//		};
//		return 0;
//	}

	// Count
	template<typename OBJ>
	int Lists<OBJ>::length(int i)
	{
		int cnt = 0;
		LinkNext* np = getList(i);
		while(np)
		{
			cnt++;
			np = getNext(*np);
		};
		return cnt;
	}

	template<typename OBJ>
	int Lists<OBJ>::growLog(int n)
	{
		// TBD>>> 11/22/09:
		// If there is enough space left already.
		if ( n <= log.spaceLeft() )
			return 0;

		int nOld = log.len();

		if( log.grow(n) )
		{
			int nLast = log.space()-1;
//			for( int i = nOld; i < log.space(); i++)
			for( int i = nOld; i < nLast; i++)
				log[i].next = i+1;
//			if(log.begin())
			if(nLast>=0) 
				log[nLast].next = -1;
//				log.last()->next = -1;
			
			if(avail < 0)
			{
				avail = nOld; //New avail...
			}
			else
			{
				int nAvail = avail;
				while( nAvail >= 0 && nAvail < nOld && log[nAvail].next >= 0 )
				{
					nAvail = log[nAvail].next;
				}
				log[nAvail].next = nOld;
			}
		}
		return n;
	}

	// Cache Version:
	// N is the number of list. OBJ is the type of list objects.
	// M is the total number of list items reserved...
	template<int N, typename OBJ, int M>
	bool ListsCache<N, OBJ, M>::add(int i, const OBJ o)
	{
		if(list[i] < 0)
		{
			if(available >= 0)
			{
				list[i] = available;
				log[available].obj = o;
				available = log[available].next;
				log[available].next = -1;
				return true;
			}
			return false;
		}
		else
		{
			int j = i;
			while(j >= 0 && list[j].next > 0) j = list[j].next;
			list[j].next = available;						   //TBD TBD.. TBD... 
			log[available].obj = o;
			available = log[available].next;
			log[available].next = -1;
			return true;
		}
		return false;
	};
		// delete/remove from list index i
	template<int N, typename OBJ, int M>
	bool ListsCache<N, OBJ, M>::del(int i, const OBJ o)
	{
		if(list[i] < 0) 
			return false;

		int j = list[i];
		// Check the first one.
		if(log[j].next < 0 && log[j].obj == o)
		{
			list[i] = -1;
			log[j].next = available;
			available = j;
			return true;
		}
		else
		{
			// Find the one whose next is o
			while( log[j].next>=0 && log[log[j].next].obj != o ) j= list[j].next;
			if(log[j].next>=0)
			{
				int k = log[j].next;
				log[j].next = log[k].next;
				log[j].next = available;
				available = k;
				return true;
			}
		}
		return false;
	}
		// locate query
	template<int N, typename OBJ, int M>
	int ListsCache<N, OBJ, M>::find(int i, const OBJ o)
	{
		int j = list[i];
		while( j>=0 && log[j].obj != o ) j= log[j].next;
		return j;
	}



} // End of namespace DSA


//#include <DSA/List.inl>
#endif