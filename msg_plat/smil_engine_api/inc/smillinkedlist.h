/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: smillinkedlist  declaration
*
*/



#ifndef SMILLINKEDLIST_H
#define SMILLINKEDLIST_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
template <class T>
class CLinkedList : public CBase
    {
	private:

		class ListNode
			{
			public:
				T iValue;
				ListNode* iNext;
			};

    public:  // Constructors and destructor

		CLinkedList()
			{
			iFirst = iLast = 0;
			}

		~CLinkedList()			
			{
			Empty();
			}

		void Empty()
			{
			while (iFirst)
				{
				ListNode* del = iFirst;
				iFirst = iFirst->iNext;
				delete del;
				}
			iFirst = iLast = 0;
			}
		
		void AddFrontL(const T& aVal)
			{
			ListNode* node = new (ELeave) ListNode;
			node->iValue = aVal;
			node->iNext = iFirst;
			if (!iLast)
				iLast = node;
			iFirst = node;			
			}		

		void AddBackL(const T& aVal)
			{
			ListNode* node = new (ELeave) ListNode;
			node->iValue = aVal;
			node->iNext = 0;
			if (iLast)
				iLast->iNext = node;				
			if (!iFirst)
				iFirst = node;			
			iLast = node;
			}

		void AddL(const T& aVal)
			{
			AddBackL(aVal);
			}

		void PushL(const T& aVal)
			{
			AddFrontL(aVal);
			}

		T& Peek() const
			{
			return iFirst->iValue;
			}

		T Pop()
			{
			ListNode* del = iFirst;
			T tmp = iFirst->iValue;
			iFirst = iFirst->iNext;
			delete del;
			return tmp;
			}

		TBool IsEmpty() const
			{
			return iFirst==0;
			}

		typedef TBool (*CompFunc)(T l,T r);

		void Sort()
			{
			Sort(&SimpleComp);
			}

		void Sort(CompFunc aCompFunc)
			{
			iFirst = MergeSort(iFirst,aCompFunc);
			for (ListNode* n=iFirst;n;n=n->iNext)
				iLast = n;
			}

	private:
		TBool SimpleComp(T l, T r)
			{
			return l<r;
			}

		ListNode* MergeSort(ListNode* c,CompFunc aCompFunc)
			{
			if (c==0||c->iNext==0) return c;
			ListNode *a = c, *b=c->iNext;
			while ((b!=0) && (b->iNext!=0))
				{ c = c->iNext; b = b->iNext->iNext; }
			b = c->iNext; c->iNext = 0;
			return Merge(MergeSort(a,aCompFunc),MergeSort(b,aCompFunc),aCompFunc);
			}

		ListNode* Merge(ListNode* a,ListNode* b,CompFunc aCompFunc)
			{
			ListNode dummy; ListNode *head = &dummy, *c = head;
			while ((a!=0) && (b!=0))
				if ((*aCompFunc)(a->iValue,b->iValue))
					{ c->iNext=a; c=a; a=a->iNext; }
				else
					{ c->iNext=b; c=b; b=b->iNext; }
			c->iNext = (a==0)?b:a;
			return head->iNext;
			}			
	public:
		class Iter 
			{
			private:
				ListNode* iNode;
			public:

				Iter(const CLinkedList& aList)
					{
					iNode = aList.iFirst;
					}

				TBool HasMore() const { return iNode!=0; }
				T& Next() { 
					ListNode* node = iNode;
					iNode = node->iNext;
					return node->iValue;
					};
			};
		friend class Iter;
	private:
		ListNode* iFirst;
		ListNode* iLast;
	};

#endif      // ?INCLUDE_H   
            
// End of File
