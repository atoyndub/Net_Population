
/***************************************************************************************
Programmer:		Wagner, Anthony
Last Update:	08/02/2017
File Name:		MemoryTools.h
Description:	a stand-alone header file containing all definitions and implementations
				associated with some memory managment tools
***************************************************************************************/

#ifndef MEMORYTOOLS_H_INCLUDED
#define MEMORYTOOLS_H_INCLUDED

#include <iostream>

template<typename T> class UniformNode
{

private:

	//points to the prevoius node in the list
	UniformNode<T> *priorNode;

	//points to the next node in the list
	UniformNode<T> *nextNode;

	//modified constructor for use by UniformList, assumes parameters
	//provided are those before and after the insertion point (or NULL
	//if node is to be inserted at list start or list end
	UniformNode(UniformNode<T> *aPriorNode, UniformNode<T> *aNextNode)
		: priorNode(aPriorNode), nextNode(aNextNode) {}

	//modified constructor for use by UniformList, assumes parameters
	//provided are those before and after the insertion point (or NULL
	//if node is to be inserted at list start or list end
	UniformNode(const T &aContent, UniformNode<T> *aPriorNode = NULL,
		UniformNode<T> *aNextNode = NULL) :content(aContent),
		priorNode(aPriorNode), nextNode(aNextNode) {}

public:

	//The class or type object of functional purpose contained by the node
	T content;

	//default constructor
	UniformNode()
	{

		nextNode = NULL;
		priorNode = NULL;

	}

	//default destructor
	~UniformNode() {}

	//getter
	UniformNode<T> *get_NextNode() const
	{

		return nextNode;

	}

	//getter
	UniformNode<T> *get_PriorNode() const
	{

		return priorNode;

	}

	//gives UniformList access to private members
	template<typename S> friend class UniformList;

};

/*
class design notes

1)	when the list is empty, firstNode and lastNode are NULL, when the list contains one
	member, both point to it, and when the list contains multiple members, the pointers
	point to the respective first and last nodes in list order
2)	each list node's nextNode pointer points to the next node in list order (or NULL if
	the node iteself is last in list order); likewise, priorNode points to the previous
	node in list oder (or NULL if the node itself is the first in list order)
*/

template<typename T> class UniformList
{

private:

	//points to the first list member (in list order)
	//or NULL if the list is empty
	UniformNode<T> *firstNode;

	//points to the last list member (in list order)
	//`or NULL if the list is empty
	UniformNode<T> *lastNode;

	//tracks number of nodes contained within this list
	unsigned long long listLength;

public:

	//defaul constructor
	UniformList() : firstNode(NULL), lastNode(NULL), listLength(0) {}

	//copy constructor
	UniformList(const UniformList<T>& otherList)
	{

		UniformNode<T> *otherListNode(otherList.firstNode);

		//otherList is populated
		if (otherListNode)
		{

			//create the first node in the group
			UniformNode<T> *thisLeadingNode = new UniformNode<T>(otherListNode->content);
			firstNode = thisLeadingNode;

			otherListNode = otherListNode->nextNode;

			//create and link any remaining nodes in the group
			if (otherListNode)
			{

				UniformNode<T> *thisLaggingNode(thisLeadingNode);

				do
				{

					thisLeadingNode = new UniformNode<T>(otherListNode->content, thisLaggingNode, NULL);
					thisLaggingNode->nextNode = thisLeadingNode;

					thisLaggingNode = thisLeadingNode;
					otherListNode = otherListNode->nextNode;

				} while (otherListNode);

			}

			//wrap up
			lastNode = thisLeadingNode;
			listLength = otherList.get_ListLength();

		}

		//otherList is empty
		else 
		{

			firstNode = NULL;
			lastNode = NULL;
			listLength = 0;

		}

	}

	//copy assignment constructor
	UniformList<T>& operator=(UniformList<T> const & otherList)
	{

		//initial setup
		UniformNode<T> *otherListNode(otherList.firstNode);
		UniformNode<T> *thisListNode(firstNode);

		//main loop
		do
		{

			//otherList exhausted
			if (!otherListNode)
			{

				//otherList and this list simultaneously exhausted - all iterations complete
				if (!thisListNode)
					return *this;

				//only otherList is exhausted
				else
				{

					UniformNode<T> *thisListLastNode(thisListNode->priorNode);

					//delete excess nodes in this list
					do
					{

						otherListNode = thisListNode->nextNode;

						delete thisListNode;
						thisListNode = otherListNode;

					} while (thisListNode);

					//stitch up this list
					if (thisListLastNode)
						thisListLastNode->nextNode = NULL;

					else
						firstNode = NULL;
					
					lastNode = thisListLastNode;

					//wrap up and return
					listLength = otherList.listLength;
					return *this;

				}

			}

			//only this list iteration is complete
			else if (!thisListNode)
			{

				//add first new node to this list
				UniformNode<T> *thisListTrailingNode(lastNode);

				thisListNode = new UniformNode<T>(otherListNode->content, thisListTrailingNode);

				if (thisListTrailingNode)
					thisListTrailingNode->nextNode = thisListNode;

				else
					firstNode = thisListNode;

				thisListTrailingNode = thisListNode;
				otherListNode = otherListNode->nextNode;

				//add subsequent new node(s) to this list
				while (otherListNode)
				{

					thisListNode = new UniformNode<T>(otherListNode->content, thisListTrailingNode);
					
					thisListTrailingNode->nextNode = thisListNode;

					thisListTrailingNode = thisListNode;
					otherListNode = otherListNode->nextNode;

				}

				//stitch up this list
				thisListNode->nextNode = NULL;
				lastNode = thisListNode;

				//wrap up and return
				listLength = otherList.listLength;
				return *this;

			}

			//copy content from source to target node
			thisListNode->content = otherListNode->content;

			//iterate the iterators
			otherListNode = otherListNode->nextNode;
			thisListNode = thisListNode->nextNode;

		} while (true);

		//this should never be reached
		return *this;

	}

	//default destructor
	~UniformList()
	{

		UniformNode<T> *laggingNode(firstNode);
		UniformNode<T> *leadingNode;

		while (laggingNode)
		{

			leadingNode = laggingNode->nextNode;
				
			delete laggingNode;
				
			laggingNode = leadingNode;

		}

	}

	//adds a new node to this list with copied content from another
	//node (internal or external); assumes nodeToCopy != NULL and insertPoint
	//is NULL or a member of this list; returns the new node's address
	UniformNode<T> *add_CopiedNode(UniformNode<T> *nodeToCopy,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		UniformNode<T> *newNode;

		if (!insertPoint)
		{

			if (insertAfter)
			{

				newNode = new UniformNode<T>(nodeToCopy->content, lastNode);

				if (lastNode)
					lastNode->nextNode = newNode;

				else
					firstNode = newNode;

				lastNode = newNode;

			}

			else
			{

				newNode = new UniformNode<T>(nodeToCopy->content,
					NULL, firstNode);

				if (firstNode)
					firstNode->priorNode = newNode;

				else
					lastNode = newNode;

				firstNode = newNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newNode = new UniformNode<T>(nodeToCopy->content,
					insertPoint, insertPoint->nextNode);

				if (insertPoint == lastNode)
					lastNode = newNode;

				else
					insertPoint->nextNode->priorNode = newNode;

				insertPoint->nextNode = newNode;

			}

			else
			{

				newNode = new UniformNode<T>(nodeToCopy->content,
					insertPoint->priorNode, insertPoint);

				if (insertPoint == firstNode)
					firstNode = newNode;

				else
					insertPoint->priorNode->nextNode = newNode;

				insertPoint->priorNode = newNode;

			}

		}

		//wrap up
		listLength++;
		return newNode;

	}

	//adds a new range to this list with copied content from a selected list
	//range (internal or external); assumes sourceRangeFirstNode and
	//sourceRangeLastNode are not NULL and form a valid range in their respective
	//positions within the source list; insertPoint is either NULL
	//or a member of this list; returns address of the first or last node within
	//the new range if returnLast is false or true respectively
	UniformNode<T> *add_CopiedRange(UniformNode<T> *sourceRangeFirstNode,
		UniformNode<T> *sourceRangeLastNode, UniformNode<T> *insertPoint = NULL,
		const bool insertAfter = true, const bool returnLast = true)
	{

		//set starting parameter value
		sourceRangeLastNode = sourceRangeLastNode->nextNode;

		//create first new range node
		UniformNode<T> *proteanNode = new UniformNode<T>(sourceRangeFirstNode->content);
		UniformNode<T> *newRangeFirstNode(proteanNode);
		UniformNode<T> *newRangeLastNode(proteanNode);
		unsigned long long rangeNodeCount(1);

		sourceRangeFirstNode = sourceRangeFirstNode->nextNode;

		//create subsequent new range nodes
		while (sourceRangeFirstNode != sourceRangeLastNode)
		{

			proteanNode = new UniformNode<T>(sourceRangeFirstNode->content, newRangeLastNode);

			newRangeLastNode->nextNode = proteanNode;

			newRangeLastNode = proteanNode;
			sourceRangeFirstNode = sourceRangeFirstNode->nextNode;
			rangeNodeCount++;

		}

		//insert the new range
		if (!insertPoint)
		{

			if (insertAfter)
			{

				if (lastNode)
				{

					newRangeFirstNode->priorNode = lastNode;
					lastNode->nextNode = newRangeFirstNode;

				}

				else
					firstNode = newRangeFirstNode;

				lastNode = newRangeLastNode;

			}

			else
			{

				if (firstNode)
				{

					newRangeLastNode->nextNode = firstNode;
					firstNode->priorNode = newRangeLastNode;

				}
					
				else
					lastNode = newRangeLastNode;

				firstNode = newRangeFirstNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newRangeFirstNode->priorNode = insertPoint;

				if (insertPoint == lastNode)
					lastNode = newRangeLastNode;

				else
				{

					newRangeLastNode->nextNode = insertPoint->nextNode;
					insertPoint->nextNode->priorNode = newRangeLastNode;
					
				}
					
				insertPoint->nextNode = newRangeFirstNode;

			}

			else
			{

				newRangeLastNode->nextNode = inserPoint;

				if (insertPoint == firstNode)
					firstNode = newRangefirstNode;

				else
				{

					newRangeFirstNode->priorNode = insertPoint->priorNode;
					insertPoint->priorNode->nextNode = newRangeFirstNode;

				}

				insertPoint->priorNode = newRangeLastNode;

			}

		}

		//wrap up
		listLength += rangeNodeCount;

		if (returnLast)
			return newRangeLastNode;

		else
			return newRangeFirstNode;

	}

	//adds a new range to this list with copied content from a selected list range (internal or external) 
	//Assumes:	sourceRangeOriginNode is not NULL there are at least nodeCount nodes between and including
	//sourceRangeOriginNode and the source list's end or start if countForward is true or false respectively;
	//nodeCount > 0; insertPoint is NULL or member of this list; returns the last or first node im the new
	//range depening on whether returnLast is true or false respectively
	UniformNode<T> *add_CopiedRange(UniformNode<T> *sourceRangeOriginNode, const unsigned long long nodeCount,
		const bool countForward = true, UniformNode<T> *insertPoint = NULL,
		const bool insertAfter = true, const bool returnLast = true)
	{

		//begin to create first new range node
		UniformNode<T> *proteanNode = new UniformNode<T>(sourceRangeOriginNode->content);
		UniformNode<T> *newRangeFirstNode(proteanNode);
		UniformNode<T> *newRangeLastNode(proteanNode);
		unsigned long long rangeNodeCount(1);

		if (countForward)
		{

			sourceRangeOriginNode = sourceRangeOriginNode->nextNode;

			//create subsequent new range nodes
			while (rangeNodeCount < nodeCount)
			{

				proteanNode = new UniformNode<T>(sourceRangeOriginNode->content, newRangeLastNode);

				newRangeLastNode->nextNode = proteanNode;

				newRangeLastNode = proteanNode;
				sourceRangeOriginNode = sourceRangeOriginNode->nextNode;

				rangeNodeCount++;

			}

		}

		else
		{

			sourceRangeOriginNode = sourceRangeOriginNode->priorNode;

			//create subsequent new range nodes
			while (rangeNodeCount < nodeCount)
			{

				proteanNode = new UniformNode<T>(sourceRangeOriginNode->content, NULL, newRangeFirstNode);

				newRangeFirstNode->priorNode = proteanNode;

				newRangeFirstNode = proteanNode;
				sourceRangeOriginNode = sourceRangeOriginNode->priorNode;

				rangeNodeCount++;

			}

		}

		//insert the new range
		if (!insertPoint)
		{

			if (insertAfter)
			{

				if (lastNode)
				{

					newRangeFirstNode->priorNode = lastNode;
					lastNode->nextNode = newRangeFirstNode;

				}
					
				else
					firstNode = newRangeFirstNode;

				lastNode = newRangeLastNode;

			}

			else
			{

				if (firstNode)
				{

					newRangeLastNode->nextNode = firstNode;
					firstNode->priorNode = newRangeLastNode;

				}	

				else
					lastNode = newRangeLastNode;

				firstNode = newRangeFirstNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newRangeFirstNode->priorNode = insertPoint;

				if (insertPoint == lastNode)
					lastNode = newRangeLastNode;

				else
				{

					newRangeLastNode->nextNode = insertPoint->nextNode;
					insertPoint->nextNode->priorNode = newRangeLastNode;
					
				}
					
				insertPoint->nextNode = newRangeFirstNode;

			}

			else
			{

				newRangeLastNode->nextNode = inserPoint;

				if (insertPoint == firstNode)
					firstNode = newRangefirstNode;

				else
				{

					newRangeFirstNode->priorNode = insertPoint->priorNode;
					insertPoint->priorNode->nextNode = newRangeFirstNode;

				}

				insertPoint->priorNode = newRangeLastNode;

			}

		}

		//wrap up
		listLength += rangeNodeCount;
		
		if (returnLast)
			return newRangeLastNode;

		else
			return newRangeFirstNode;

	}
	
	//adds a new range to this list with copied content from an entire selected
	//list (this list or an external one); assumes sourceList is not empty; insertPoint
	//is either NULL or a member of this list; returns address of the
	//first or last node within the new range if returnLast is false or true respectively
	//note:	this method is faster than using the addRange_Copied method
	UniformNode<T> *add_CopiedWholeList(UniformList<T> &sourceList,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true,
		const bool returnLast = true)
	{

		//create first new range node
		UniformNode<T> *sourceRangeFirstNode(sourceList.firstNode);
		UniformNode<T> *proteanNode = new UniformNode<T>(sourceRangeFirstNode->content);
		UniformNode<T> *newRangeFirstNode(proteanNode);
		UniformNode<T> *newRangeLastNode(proteanNode);

		sourceRangeFirstNode = sourceRangeFirstNode->nextNode;

		//create subsequent new range nodes
		while (sourceRangeFirstNode)
		{

			proteanNode = new UniformNode<T>(sourceRangeFirstNode->content, newRangeLastNode);

			newRangeLastNode->nextNode = proteanNode;

			newRangeLastNode = proteanNode;
			sourceRangeFirstNode = sourceRangeFirstNode->nextNode;

		}

		//insert the new range
		if (!insertPoint)
		{

			if (insertAfter)
			{

				if (lastNode)
				{

					newRangeFirstNode->priorNode = lastNode;
					lastNode->nextNode = newRangeFirstNode;

				}

				else
					firstNode = newRangeFirstNode;

				lastNode = newRangeLastNode;

			}

			else
			{

				if (firstNode)
				{

					newRangeLastNode->nextNode = firstNode;
					firstNode->priorNode = newRangeLastNode;

				}
				
				else
					lastNode = newRangeLastNode;

				firstNode = newRangeFirstNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newRangeFirstNode->priorNode = insertPoint;

				if (insertPoint == lastNode)
					lastNode = newRangeLastNode;

				else
				{

					newRangeLastNode->nextNode = insertPoint->nextNode;
					insertPoint->nextNode->priorNode = newRangeLastNode;
					
				}
					
				insertPoint->nextNode = newRangeFirstNode;

			}

			else
			{

				newRangeLastNode->nextNode = insertPoint;

				if (insertPoint == firstNode)
					firstNode = newRangeFirstNode;

				else
				{

					newRangeFirstNode->priorNode = insertPoint->priorNode;
					insertPoint->priorNode->nextNode = newRangeFirstNode;

				}

				insertPoint->priorNode = newRangeLastNode;

			}

		}
		
		//wrap up
		listLength += sourceList.listLength;
		
		if (returnLast)
			return newRangeLastNode;

		else
			return newRangeFirstNode;

	}

	//cuts a node from an external list and adds it into this list;
	//assumes nodeToCut is not NULL and is a member of sourceList;
	//sourceList is not this list; insertPoint is either NULL or a
	//member of this list; returns the address of node which bounds
	//the inserted node on side opposite insertPoint, or the address
	//of node immediately prior to or after the inserted node node if
	//insertPoint is NULL and insertAfter is true or false respectively;
	//NULL if the intended return address is at list start or end
	UniformNode<T> *add_CutNode(UniformList<T> &sourceList, UniformNode<T> *nodeToCut,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		//stitch up sourceList
		UniformNode<T> *sourceAnteriorBoundaryNode(nodeToCut->priorNode);
		UniformNode<T> *sourcePosteriorBoundaryNode(nodeToCut->nextNode);

		if (sourceAnteriorBoundaryNode)
			sourceAnteriorBoundaryNode->nextNode = sourcePosteriorBoundaryNode;

		else
			sourceList.firstNode = sourcePosteriorBoundaryNode;

		if (sourcePosteriorBoundaryNode)
			sourcePosteriorBoundaryNode->priorNode = sourceAnteriorBoundaryNode;

		else
			sourceList.lastNode = sourceAnteriorBoundaryNode;
		
		//decrement/increment listLengths
		sourceList.listLength--;
		listLength++;

		//insert the cut node and return
		if (!insertPoint)
		{

			if (insertAfter)
			{

				nodeToCut->priorNode = lastNode;
				nodeToCut->nextNode = NULL;

				if (lastNode)
					lastNode->nextNode = nodeToCut;

				else
					firstNode = nodeToCut;

				lastNode = nodeToCut;

				return nodeToCut->priorNode;

			}

			else
			{

				nodeToCut->priorNode = NULL;
				nodeToCut->nextNode = firstNode;

				if (firstNode)
					firstNode->priorNode = nodeToCut;

				else
					lastNode = nodeToCut;

				firstNode = nodeToCut;

				return nodeToCut->nextNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				nodeToCut->priorNode = insertPoint;
				nodeToCut->nextNode = insertPoint->nextNode;

				if (insertPoint == lastNode)
					lastNode = nodeToCut;

				else
					insertPoint->nextNode->priorNode = nodeToCut;

				insertPoint->nextNode = nodeToCut;

				return nodeToCut->nextNode;

			}

			else
			{

				nodeToCut->priorNode = insertPoint->priorNode;
				nodeToCut->nextNode = insertPoint;

				if (insertPoint == firstNode)
					firstNode = nodeToCut;

				else
					insertPoint->priorNode->nextNode = nodeToCut;

				insertPoint->priorNode = nodeToCut;

				return nodeToCut->priorNode;

			}

		}

	}

	//cuts a range from an external list and adds it into this list
	//assumes sourceList is not this list; sourceRangeFirstNode and
	//sourceRangeLastNode are not NULL, are members of sourceList, and
	//encompass a valid range in their respective positions; insertPoint is
	//either NULL or a member of this list; returns the address of node
	//which bounds the inserted range on side opposite insertPoint, or
	//the address of node immediately prior to or after the inserted range
	//node if insertPoint is NULL and insertAfter is true or false respectively;
	//NULL if the intended return address is list start or end
	UniformNode<T> *add_CutRange(UniformList<T> &sourceList,
		UniformNode<T> *sourceRangeFirstNode, UniformNode<T> *sourceRangeLastNode,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		//set starting parameter and variable values
		UniformNode<T> *sourceAnteriorBoundaryNode(sourceRangeFirstNode);
		UniformNode<T> *sourcePosteriorBoundaryNode(sourceRangeLastNode->nextNode);
		unsigned long long rangeNodeCount(0);

		//count nodes in the range
		do
		{

			sourceAnteriorBoundaryNode = sourceAnteriorBoundaryNode->nextNode;
			rangeNodeCount++;

		} while (sourceAnteriorBoundaryNode != sourcePosteriorBoundaryNode);

		sourceAnteriorBoundaryNode = sourceRangeFirstNode->priorNode;

		//stitch up sourceList
		if (sourceAnteriorBoundaryNode)
			sourceAnteriorBoundaryNode->nextNode = sourcePosteriorBoundaryNode;

		else
			sourceList.firstNode = sourcePosteriorBoundaryNode;

		if (sourcePosteriorBoundaryNode)
			sourcePosteriorBoundaryNode->priorNode = sourceAnteriorBoundaryNode;

		else
			sourceList.lastNode = sourceAnteriorBoundaryNode;

		//update listLengths
		listLength += rangeNodeCount;
		sourceList.listLength -= rangeNodeCount;

		//insert the cut range and return
		if (!insertPoint)
		{

			if (insertAfter)
			{

				sourceRangeFirstNode->priorNode = lastNode;
				sourceRangeLastNode->nextNode = NULL;

				if (lastNode)
					lastNode->nextNode = sourceRangeFirstNode;

				else
					firstNode = sourceRangeFirstNode;

				lastNode = sourceRangeLastNode;

				return sourceRangeFirstNode->priorNode;

			}

			else
			{

				sourceRangeFirstNode->priorNode = NULL;
				sourceRangeLastNode->nextNode = firstNode;

				if (firstNode)
					firstNode->priorNode = sourceRangeLastNode;

				else
					lastNode = sourceRangeLastNode;

				firstNode = sourceRangeFirstNode;

				return sourceRangeLastNode->nextNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				sourceRangeFirstNode->priorNode = insertPoint;
				sourceRangeLastNode->nextNode = insertPoint->nextNode;

				if (insertPoint == lastNode)
					lastNode = sourceRangeLastNode;

				else
				{

					sourceRangeLastNode->nextNode = insertPoint->nextNode;
					insertPoint->nextNode->priorNode = sourceRangeLastNode;

				}

				insertPoint->nextNode = sourceRangeFirstNode;

				return sourceRangeLastNode->nextNode;

			}

			else
			{

				sourceRangeFirstNode->priorNode = insertPoint->priorNode;
				sourceRangeLastNode->nextNode = insertPoint;

				if (insertPoint == firstNode)
					firstNode = sourceRangeFirstNode;

				else
				{

					sourceRangeFirstNode->priorNode = insertPoint->priorNode;
					insertPoint->priorNode->nextNode = sourceRangeFirstNode;

				}

				insertPoint->priorNode = sourceRangeLastNode;

				return sourceRangeFirstNode->priorNode;

			}

		}

	}

	//cuts a range from an external list and adds it to this list
	//assumes sourceList is not this list, sourceRangeOriginNode is
	//not NULL and is a member of sourceList; there are at least
	//nodeCount nodes between and including sourceRangeOriginNode
	//and sourceList's end or start if countForward is true or false
	//respectively; nodeCount > 0; insertPoint is NULL or member of this
	//list; returns the address of node which bounds the inserted range on
	//side opposite insertPoint, or the address of node immediately prior
	//to or after the inserted range node if insertPoint is NULL and insertAfter
	//is true or false respectively; NULL if the intended return address is
	//list start or end
	UniformNode<T> *add_CutRange(UniformList<T> &sourceList,
		UniformNode<T> *sourceRangeOriginNode, const unsigned long long nodeCount,
		const bool countForward = true, UniformNode<T> *insertPoint = NULL,
		const bool insertAfter = true)
	{

		//additional variables
		UniformNode<T> *sourceRangeTerminalNode;
		UniformNode<T> *sourceAnteriorBoundaryNode;
		UniformNode<T> *sourcePosteriorBoundaryNode;
		unsigned long long rangeNodeCount(0);

		if (countForward)
		{

			//set starting parameter and variable values
			sourcePosteriorBoundaryNode = sourceRangeOriginNode;
			sourceAnteriorBoundaryNode = sourceRangeOriginNode->priorNode;

			//count nodes in the range
			do
			{

				sourcePosteriorBoundaryNode
					= sourcePosteriorBoundaryNode->nextNode;
				rangeNodeCount++;

			} while (rangeNodeCount != nodeCount);

			//stitch up sourceList (and prep for insertion in this list)
			if (sourceAnteriorBoundaryNode)
				sourceAnteriorBoundaryNode->nextNode = sourcePosteriorBoundaryNode;

			else
				sourceList.firstNode = sourcePosteriorBoundaryNode;

			if (sourcePosteriorBoundaryNode)
			{

				sourceRangeTerminalNode = sourcePosteriorBoundaryNode->priorNode;
				sourcePosteriorBoundaryNode->priorNode = sourceAnteriorBoundaryNode;

			}

			else
			{

				sourceRangeTerminalNode = sourceList.lastNode;
				sourceList.lastNode = sourceAnteriorBoundaryNode;

			}

			//update listLengths
			listLength += rangeNodeCount;
			sourceList.listLength -= rangeNodeCount;

			//insert the cut range and return
			if (!insertPoint)
			{

				if (insertAfter)
				{

					sourceRangeOriginNode->priorNode = lastNode;
					sourceRangeTerminalNode->nextNode = NULL;

					if (lastNode)
						lastNode->nextNode = sourceRangeOriginNode;

					else
						firstNode = sourceRangeOriginNode;

					lastNode = sourceRangeTerminalNode;

					return sourceRangeOriginNode->priorNode;

				}

				else
				{

					sourceRangeOriginNode->priorNode = NULL;
					sourceRangeTerminalNode->nextNode = firstNode;

					if (firstNode)
						firstNode->priorNode = sourceRangeTerminalNode;

					else
						lastNode = sourceRangeTerminalNode;

					firstNode = sourceRangeOriginNode;

					return sourceRangeTerminalNode->nextNode;

				}

			}

			else
			{

				if (insertAfter)
				{

					sourceRangeOriginNode->priorNode = insertPoint;
					sourceRangeTerminalNode->nextNode = insertPoint->nextNode;

					if (insertPoint == lastNode)
						lastNode = sourceRangeTerminalNode;

					else
					{

						sourceRangeTerminalNode->nextNode = insertPoint->nextNode;
						insertPoint->nextNode->priorNode = sourceRangeTerminalNode;

					}

					insertPoint->nextNode = sourceRangeOriginNode;

					return sourceRangeTerminalNode->nextNode;

				}

				else
				{

					sourceRangeOriginNode->priorNode = insertPoint->priorNode;
					sourceRangeTerminalNode->nextNode = insertPoint;

					if (insertPoint == firstNode)
						firstNode = sourceRangeOriginNode;

					else
					{

						sourceRangeOriginNode->priorNode = insertPoint->priorNode;
						insertPoint->priorNode->nextNode = sourceRangeOriginNode;

					}

					insertPoint->priorNode = sourceRangeTerminalNode;

					return sourceRangeOriginNode->priorNode;

				}

			}

		}

		else
		{

			//set starting parameter and variable values
			sourcePosteriorBoundaryNode = sourceRangeOriginNode->nextNode;
			sourceAnteriorBoundaryNode = sourceRangeOriginNode;

			//count nodes in the range
			do
			{

				sourceAnteriorBoundaryNode
					= sourceAnteriorBoundaryNode->priorNode;
				rangeNodeCount++;

			} while (rangeNodeCount != nodeCount);

			//update listLengths
			listLength += rangeNodeCount;
			sourceList.listLength -= rangeNodeCount;

			//stitch up sourceList (and prep for insertion in this list)
			if (sourceAnteriorBoundaryNode)
			{

				sourceRangeTerminalNode = sourceAnteriorBoundaryNode->nextNode;
				sourceAnteriorBoundaryNode->nextNode = sourcePosteriorBoundaryNode;

			}

			else
			{

				sourceRangeTerminalNode = sourceList.firstNode;
				sourceList.firstNode = sourcePosteriorBoundaryNode;

			}
				
			if (sourcePosteriorBoundaryNode)
				sourcePosteriorBoundaryNode->priorNode = sourceAnteriorBoundaryNode;

			else
				sourceList.lastNode = sourceAnteriorBoundaryNode;

			//insert the cut range and return
			if (!insertPoint)
			{

				if (insertAfter)
				{

					sourceRangeTerminalNode->priorNode = lastNode;
					sourceRangeOriginNode->nextNode = NULL;

					if (lastNode)
						lastNode->nextNode = sourceRangeTerminalNode;

					else
						firstNode = sourceRangeTerminalNode;

					lastNode = sourceRangeOriginNode;

					return sourceRangeTerminalNode->priorNode;

				}

				else
				{

					sourceRangeTerminalNode->priorNode = NULL;
					sourceRangeOriginNode->nextNode = firstNode;

					if (firstNode)
						firstNode->priorNode = sourceRangeOriginNode;

					else
						lastNode = sourceRangeOriginNode;

					firstNode = sourceRangeTerminalNode;

					return sourceRangeOriginNode->nextNode;

				}

			}

			else
			{

				if (insertAfter)
				{

					sourceRangeTerminalNode->priorNode = insertPoint;
					sourceRangeOriginNode->nextNode = insertPoint->nextNode;

					if (insertPoint == lastNode)
						lastNode = sourceRangeOriginNode;

					else
					{

						sourceRangeOriginNode->nextNode = insertPoint->nextNode;
						insertPoint->nextNode->priorNode = sourceRangeOriginNode;

					}

					insertPoint->nextNode = sourceRangeTerminalNode;

					return sourceRangeOriginNode->nextNode;

				}

				else
				{

					sourceRangeTerminalNode->priorNode = insertPoint->priorNode;
					sourceRangeOriginNode->nextNode = insertPoint;

					if (insertPoint == firstNode)
						firstNode = sourceRangeTerminalNode;

					else
					{

						sourceRangeTerminalNode->priorNode = insertPoint->priorNode;
						insertPoint->priorNode->nextNode = sourceRangeTerminalNode;

					}

					insertPoint->priorNode = sourceRangeOriginNode;

					return sourceRangeTerminalNode->priorNode;

				}

			}

		}

	}
	
	//cuts all nodes from an external list and adds them into this list;
	//assumes sourceList is not empty and is not this list; insertPoint is either NULL
	//or a member of this list; returns the address of node which bounds the inserted
	//range on side opposite insertPoint, or the address of node immediately prior
	//to or after the inserted range node if insertPoint is NULL and insertAfter
	//is true or false respectively; NULL if the intended return address is
	//list start or end; ***note: faster than using the addRange_Cut method***
	UniformNode<T> *add_CutWholeList(UniformList<T> &sourceList,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		//set starting parameter and variable values
		UniformNode<T> *sourceRangeFirstNode(sourceList.firstNode);
		UniformNode<T> *sourceRangeLastNode(sourceList.lastNode);

		//stitch up sourceList
		sourceList.firstNode = NULL;
		sourceList.lastNode = NULL;

		//update listLengths
		listLength += sourceList.listLength;
		sourceList.listLength = 0;

		//insert the cut range and return
		if (!insertPoint)
		{

			if (insertAfter)
			{

				sourceRangeFirstNode->priorNode = lastNode;
				sourceRangeLastNode->nextNode = NULL;

				if (lastNode)
					lastNode->nextNode = sourceRangeFirstNode;

				else
					firstNode = sourceRangeFirstNode;

				lastNode = sourceRangeLastNode;

				return sourceRangeFirstNode->priorNode;

			}

			else
			{

				sourceRangeFirstNode->priorNode = NULL;
				sourceRangeLastNode->nextNode = firstNode;

				if (firstNode)
					firstNode->priorNode = sourceRangeLastNode;

				else
					lastNode = sourceRangeLastNode;

				firstNode = sourceRangeFirstNode;

				return sourceRangeLastNode->nextNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				sourceRangeFirstNode->priorNode = insertPoint;
				sourceRangeLastNode->nextNode = insertPoint->nextNode;

				if (insertPoint == lastNode)
					lastNode = sourceRangeLastNode;

				else
				{

					sourceRangeLastNode->nextNode = insertPoint->nextNode;
					insertPoint->nextNode->priorNode = sourceRangeLastNode;

				}

				insertPoint->nextNode = sourceRangeFirstNode;

				return sourceRangeLastNode->nextNode;

			}

			else
			{

				sourceRangeFirstNode->priorNode = insertPoint->priorNode;
				sourceRangeLastNode->nextNode = insertPoint;

				if (insertPoint == firstNode)
					firstNode = sourceRangeFirstNode;

				else
				{

					sourceRangeFirstNode->priorNode = insertPoint->priorNode;
					insertPoint->priorNode->nextNode = sourceRangeFirstNode;

				}

				insertPoint->priorNode = sourceRangeLastNode;

				return sourceRangeFirstNode->priorNode;

			}

		}

	}

	//creates and adds a new node to this list; assumes insertPoint
	//is either NULL or a member of this list; returns address of the new node
	UniformNode<T> *add_NewNode(UniformNode<T> *insertPoint = NULL,
		const bool insertAfter = true)
	{

		UniformNode<T> *newNode;

		if (!insertPoint)
		{

			if (insertAfter)
			{

				newNode = new UniformNode<T>(lastNode, NULL);

				if (lastNode)
					lastNode->nextNode = newNode;

				else
					firstNode = newNode;

				lastNode = newNode;

			}

			else
			{

				newNode = new UniformNode<T>();
				newNode->nextNode = firstNode;

				if (firstNode)
					firstNode->priorNode = newNode;

				else
					lastNode = newNode;

				firstNode = newNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newNode = new UniformNode<T>(insertPoint, insertPoint->nextNode);

				if (insertPoint == lastNode)
					lastNode = newNode;

				else
					insertPoint->nextNode->priorNode = newNode;

				insertPoint->nextNode = newNode;

			}

			else
			{

				newNode = new UniformNode<T>(insertPoint->priorNode, insertPoint);

				if (insertPoint == firstNode)
					firstNode = newNode;

				else
					insertPoint->priorNode->nextNode = newNode;

				insertPoint->priorNode = newNode;

			}

		}

		//wrap up
		listLength++;
		return newNode;

	}

	//creates and adds a new node to this list and initializes its content;
	//assumes insertPoint is either NULL or a member of this list; returns
	//address of the new node
	UniformNode<T> *add_NewNode(const T &nodeContent,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		UniformNode<T> *newNode;

		if (!insertPoint)
		{

			if (insertAfter)
			{

				newNode = new UniformNode<T>(nodeContent, lastNode, NULL);

				if (lastNode)
					lastNode->nextNode = newNode;

				else
					firstNode = newNode;

				lastNode = newNode;

			}

			else
			{

				newNode = new UniformNode<T>(nodeContent, NULL, firstNode);

				if (firstNode)
					firstNode->priorNode = newNode;

				else
					lastNode = newNode;

				firstNode = newNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newNode = new UniformNode<T>(nodeContent,
					insertPoint, insertPoint->nextNode);

				if (insertPoint == lastNode)
					lastNode = newNode;

				else
					insertPoint->nextNode->priorNode = newNode;

				insertPoint->nextNode = newNode;

			}

			else
			{

				newNode = new UniformNode<T>(nodeContent,
					insertPoint->priorNode, insertPoint);

				if (insertPoint == firstNode)
					firstNode = newNode;

				else
					insertPoint->priorNode->nextNode = newNode;

				insertPoint->priorNode = newNode;

			}

		}

		//wrap up
		listLength++;
		return newNode;

	}

	//creates and adds a new range to this list; assumes nodeCount > 0;
	//insertPoint is either NULL or a member of this list; returns address
	//of the first or last node within the new range if returnLast is
	//false or true respectively
	UniformNode<T> *add_NewRange(const unsigned long long nodeCount,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true,
		const bool returnLast = true)
	{

		//create the first new range node
		UniformNode<T> *newRangeFirstNode = new UniformNode<T>;
		UniformNode<T> *newRangeLastNode(newRangeFirstNode);
		UniformNode<T> *proteanNode;

		//create subsequent new range nodes
		for (unsigned long long count = 1; count < nodeCount; count++)
		{

			proteanNode = new UniformNode<T>(newRangeLastNode, NULL);

			newRangeLastNode->nextNode = proteanNode;

			newRangeLastNode = proteanNode;

		}

		//insert the new range
		if (!insertPoint)
		{

			if (insertAfter)
			{

				if (lastNode)
				{

					newRangeFirstNode->priorNode = lastNode;
					lastNode->nextNode = newRangeFirstNode;

				}

				else
					firstNode = newRangeFirstNode;

				lastNode = newRangeLastNode;

			}

			else
			{

				if (firstNode)
				{

					newRangeLastNode->nextNode = firstNode;
					firstNode->priorNode = newRangeLastNode;

				}

				else
					lastNode = newRangeLastNode;

				firstNode = newRangeFirstNode;

			}

		}

		else
		{

			if (insertAfter)
			{

				newRangeFirstNode->priorNode = insertPoint;

				if (insertPoint == lastNode)
					lastNode = newRangeLastNode;

				else
				{

					newRangeLastNode->nextNode = insertPoint->nextNode;
					insertPoint->nextNode->priorNode = newRangeLastNode;

				}

				insertPoint->nextNode = newRangeFirstNode;

			}

			else
			{

				newRangeLastNode->nextNode = insertPoint;

				if (insertPoint == firstNode)
					firstNode = newRangeFirstNode;

				else
				{

					newRangeFirstNode->priorNode = insertPoint->priorNode;
					insertPoint->priorNode->nextNode = newRangeFirstNode;

				}

				insertPoint->priorNode = newRangeLastNode;

			}

		}

		//wrap up
		listLength += nodeCount;

		if (returnLast)
			return newRangeLastNode;

		else
			return newRangeFirstNode;

	}

	//delete a node from this list; assumes nodeToDelete is not NULL
	//and is a member of this list; returns address of the node after
	//or before the deleted node in list position if returnNext is true
	//or false respectively, or NULL if the intended return address is
	//end or start
	UniformNode<T> *delete_Node(UniformNode<T> *nodeToDelete,
		const bool returnNext = true)
	{

		//set values
		UniformNode<T> *anteriorBoundaryNode(nodeToDelete->priorNode);
		UniformNode<T> *posteriorBoundaryNode(nodeToDelete->nextNode);

		//stitch up around nodeToDelete
		if (anteriorBoundaryNode)
			anteriorBoundaryNode->nextNode = posteriorBoundaryNode;

		else
			firstNode = posteriorBoundaryNode;

		if (posteriorBoundaryNode)
			posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

		else
			lastNode = anteriorBoundaryNode;

		//delete the node, wrap up and return
		delete nodeToDelete;
		listLength--;

		if (returnNext)
			return posteriorBoundaryNode;

		else
			return anteriorBoundaryNode;

	}

	//delete a range from this list; assumes rangeFirstNode and rangeLastNode
	//are not NULL, are members of this list, and encompass a valid range in
	//their respective positions; returns address of the node after or before
	//the deleted range in list position if returnNext is true or false respectively,
	//or NULL if the intended return address is list start or end
	UniformNode<T> *delete_Range(UniformNode<T> *rangeFirstNode,
		UniformNode<T> *rangeLastNode, const bool returnNext = true)
	{

		//set starting parameter and variable values
		UniformNode<T> *rangeAnteriorBoundaryNode(rangeFirstNode->priorNode);
		UniformNode<T> *rangePosteriorBoundaryNode(rangeLastNode->nextNode);
		unsigned long long rangeNodeCount(0);

		//delete nodes in range until range end is reached
		do
		{

			rangeLastNode = rangeFirstNode->nextNode;

			delete rangeFirstNode;
			rangeFirstNode = rangeLastNode;

			rangeNodeCount++;

		} while (rangeLastNode != rangePosteriorBoundaryNode);

		//stitch up around the deleted range
		if (rangeAnteriorBoundaryNode)
			rangeAnteriorBoundaryNode->nextNode = rangePosteriorBoundaryNode;

		else
			firstNode = rangePosteriorBoundaryNode;

		if (rangePosteriorBoundaryNode)
			rangePosteriorBoundaryNode->priorNode = rangeAnteriorBoundaryNode;

		else
			lastNode = rangeAnteriorBoundaryNode;

		//wrap up and return
		listLength -= rangeNodeCount;

		//determine the return value
		if (returnNext)
			return rangePosteriorBoundaryNode;

		else
			return rangeAnteriorBoundaryNode;

	}

	//deletes a range from this list; assumes rangeOriginNode is NULL or a
	//member of this list; there are at least nodeCount nodes between and
	//including rangeOriginNode and list end or list start if countForward
	//is true or false respectively; nodeCount > 0; address of the node
	//after or before the deleted range in list position if returnNext is
	//true or false respectively, or NULL if the intended return address
	//list start or end
	UniformNode<T> *delete_Range(UniformNode<T> *rangeOriginNode,
		const unsigned long long nodeCount, const bool countForward = true,
		const bool returnNext = true)
	{

		//additional variables
		UniformNode<T> *rangeAnteriorBoundaryNode;
		UniformNode<T> *rangePosteriorBoundaryNode;
		unsigned long long rangeNodeCount(0);

		//delete nodes in range until range end is reached
		if (countForward)
		{

			rangeAnteriorBoundaryNode = rangeOriginNode->priorNode;

			do
			{

				rangePosteriorBoundaryNode = rangeOriginNode->nextNode;

				delete rangeOriginNode;
				rangeOriginNode = rangePosteriorBoundaryNode;

				rangeNodeCount++;

			} while (rangeNodeCount < nodeCount);

		}

		else
		{

			rangePosteriorBoundaryNode = rangeOriginNode->nextNode;

			do
			{

				rangeAnteriorBoundaryNode = rangeOriginNode->priorNode;

				delete rangeOriginNode;
				rangeOriginNode = rangeAnteriorBoundaryNode;

				rangeNodeCount++;

			} while (rangeNodeCount < nodeCount);

		}

		//stitch up around the deleted range
		if (rangeAnteriorBoundaryNode)
			rangeAnteriorBoundaryNode->nextNode = rangePosteriorBoundaryNode;

		else
			firstNode = rangePosteriorBoundaryNode;

		if (rangePosteriorBoundaryNode)
			rangePosteriorBoundaryNode->priorNode = rangeAnteriorBoundaryNode;

		else
			lastNode = rangeAnteriorBoundaryNode;

		//wrap up and return
		listLength -= rangeNodeCount;

		if (returnNext)
			return rangePosteriorBoundaryNode;

		else
			return rangeAnteriorBoundaryNode;

	}

	//delete all nodes from this list; **note: faster
	//than using the deleteRange method***
	void delete_WholeList()
	{

		//set starting values
		UniformNode<T> *laggingNode(firstNode);
		UniformNode<T> *leadingNode;

		//delete nodes
		while (laggingNode)
		{

			leadingNode = laggingNode->nextNode;

			delete laggingNode;

			laggingNode = leadingNode;

		}

		//wrap up
		firstNode = NULL;
		lastNode = NULL;

		listLength = 0;

	}

	//get a node address by passing a position index; assumes indexNumber < listLength
	//returns the address of the node at list position corresponding to indexNumber
	UniformNode<T> *get_NodeAddress(const unsigned long long indexNumber)
	{

		UniformNode<T> *iterationNode;
		unsigned long long count;

		if (indexNumber > (listLength / 2))
		{

			count = listLength - indexNumber - 1;
			iterationNode = lastNode;

			while (count)
			{
				
				iterationNode = iterationNode->priorNode;
				count--;

			}

		}

		else
		{

			count = 0;
			iterationNode = firstNode;

			while (count < indexNumber)
			{
				
				iterationNode = iterationNode->nextNode;
				count++;

			}

		}

		return iterationNode;

	}

	//find a node's list position index by passing its address; assumes targetNode
	//is not null and is a member of this list; returns targetNode's position index
	unsigned long long get_NodePosition(UniformNode<T> *targetNode) const
	{

		unsigned long long count(0);

		while (targetNode != firstNode)
		{

			targetNode = targetNode->priorNode;
			count++;

		}

		return count;

	}

	//gets the number of nodes in the list; returns current value of listLength
	unsigned long long get_ListLength() const
	{

		return listLength;

	}

	//get address of the list's first node; returns first list node address,
	//NULL if list is empty
	UniformNode<T> *get_FirstNode() const
	{

		return firstNode;

	}

	//get address of the list's last node; returns last list node address,
	//NULL if list is empty
	UniformNode<T> *get_LastNode() const
	{

		return lastNode;

	}

	//find the number of nodes contained within a range; assumes rangeFirstNode
	//and rangeLastNode are not NULL, are members of this list, and form a valid
	//range in their respective positions; returns the number of nodes contained
	//between and including rangeFirstNode and rangeLastNode
	unsigned long long get_RangeLength(UniformNode<T> *rangeFirstNode,
		UniformNode<T> *rangeLastNode) const
	{

		unsigned long long count = 1;

		while (rangeFirstNode != rangeLastNode)
		{

			rangeFirstNode = rangeFirstNode->nextNode;
			count++;

		}

		return count;

	}

	//find the terminal node of a counted range; assumes rangeOriginNode is not
	//NULL and is a member of this list; there are at least nodeCount nodes
	//between and including rangeOriginNode and list end or start if 
	//countForward is true or false respectively; nodeCount is > 0; returns the
	//address of the terminal node in the target range
	UniformNode<T> *get_RangeTerminalNode(UniformNode<T> *rangeOriginNode,
		unsigned long long nodeCount, const bool countForward) const
	{

		nodeCount--;

		if (countForward)
		{

			while (nodeCount)
			{

				rangeOriginNode = rangeOriginNode->nextNode;
				count--;

			}

		}

		else
		{

			while (nodeCount)
			{

				rangeOriginNode = rangeOriginNode->priorNode;
				count--;

			}

		}

		return rangeOriginNode;

	}

	//reverse the order of nodes in the range; assumes rangeFirstNode and
	//rangeLastNode are not NULL, are members of this list, and encompass
	//a valid range in their respective positions; returns address of the
	//node after or before the inverted range in list position if returnNext
	//is true or false respectively, or NULL if the intended return address
	//is at list start or end; ***note: ***has no effect save return if
	//target range contains only one node***
	UniformNode<T> *invert_Range(UniformNode<T> *rangeFirstNode,
		UniformNode<T> *rangeLastNode, const bool returnNext = true)
	{

		//initial setup
		UniformNode<T> *rangeAnteriorBoundaryNode(rangeFirstNode->priorNode);
		UniformNode<T> *rangePosteriorBoundaryNode(rangeLastNode->nextNode);

		if (rangeFirstNode != rangeLastNode)
		{

			//additional setup
			UniformNode<T> *laggingNode(rangeFirstNode);
			UniformNode<T> *leadingNode(rangeFirstNode->nextNode);

			//partial inversion of the range first node
			rangeFirstNode->nextNode = rangePosteriorBoundaryNode;

			//disjointed inversion of any/all intervening range nodes
			do
			{

				laggingNode->priorNode = leadingNode;

				laggingNode = leadingNode;
				leadingNode = leadingNode->nextNode;

				laggingNode->nextNode = laggingNode->priorNode;

			} while (laggingNode != rangeLastNode);

			//partial inversion of the range last node
			rangeLastNode->priorNode = rangeAnteriorBoundaryNode;

			//complete inversion at range boundary nodes
			if (rangeAnteriorBoundaryNode)
				rangeAnteriorBoundaryNode->nextNode = rangeLastNode;
			else
				firstNode = rangeLastNode;

			if (rangePosteriorBoundaryNode)
				rangePosteriorBoundaryNode->priorNode = rangeFirstNode;

			else
				lastNode = rangeFirstNode;

		}

		if (returnNext)
			return rangePosteriorBoundaryNode;

		else
			return rangeAnteriorBoundaryNode;

	}

	//reverse the order of nodes in the range; rangeOriginNode is not
	//NULL and is a member of this list; there are at least nodeCount
	//nodes between and including rangeOriginNode and list end or start
	//if countForward is true or false respectively; nodeCount is > 0;
	//returns address of the node after or before the inverted range in
	//list position if returnNext is true or false respectively, or NULL
	//if the intended return address is list start or end;
	//***note: has no effect save return if target range contains only one node***
	UniformNode<T> *invert_Range(UniformNode<T> *rangeOriginNode,
		unsigned long long nodeCount, const bool countForward = true,
		const bool returnNext = true)
	{
		
		if (nodeCount > 1)
		{

			//initial setup
			UniformNode<T> *rangeBoundaryNode;
			UniformNode<T> *leadingNode;
			UniformNode<T> *laggingNode;

			if (countForward)
			{

				//complete initial setup
				rangeBoundaryNode = rangeOriginNode->priorNode;
				laggingnode = rangeOriginNode;
				leadingNode = rangeOriginNode->nextNode;

				//disjointed inversion of any/all intervening range nodes
				do
				{

					laggingnode->priorNode = leadingNode;

					laggingnode = leadingNode;
					leadingNode = leadingNode->nextNode;

					laggingnode->nextNode = laggingnode->priorNode;

					nodeCount--;

				} while (nodeCount > 1);

				//rangeBoundaryNode is anterior boundary, laggingNode is
				//last node, rangeOriginNode is first node and 
				//leadingNode is posterior boundary

				//partial inversions of the range first and last nodes
				rangeOriginNode->nextNode = leadingNode;
				laggingNode->priorNode = rangeBoundaryNode;

				//complete inversion at range boundary nodes
				if (rangeBoundaryNode)
					rangeBoundaryNode->nextNode = laggingNode;

				else
					firstNode = laggingNode;

				if (leadingNode)
					leadingNode->priorNode = rangeOriginNode;

				else
					lastNode = rangeOriginNode;

				//return
				if (returnNext)
					return leadingNode;

				else
					return rangeBoundaryNode;

			}

			else
			{

				//complete initial setup
				rangeBoundaryNode = rangeOriginNode->nextNode;
				laggingnode = rangeOriginNode;
				leadingNode = rangeOriginNode->priorNode;

				//disjointed inversion of any/all intervening range nodes
				do
				{

					laggingnode->nextNode = leadingNode;

					laggingnode = leadingNode;
					leadingNode = leadingNode->priorNode;

					laggingnode->priorNode = laggingnode->nextNode;

					nodeCount--;

				} while (nodeCount > 1);

				//rangeBoundaryNode is posterior boundary, laggingNode is
				//first node, rangeOriginNode is last node and 
				//leadingNode is anterior boundary

				//partial inversions of the range first and last nodes
				rangeOriginNode->priorNode = leadingNode;
				laggingNode->nextNode = rangeBoundaryNode;

				//complete inversion at range boundary nodes
				if (rangeBoundaryNode)
					rangeBoundaryNode->priorNode = laggingNode;

				else
					lastNode = laggingNode;

				if (leadingNode)
					leadingNode->nextNode = rangeOriginNode;

				else
					firstNode = rangeOriginNode;

				//return
				if (returnNext)
					return rangeBoundaryNode;

				else
					return leadingNode;

			}

		}

		else
		{

			if (returnNext)
				return rangeOriginNode->nextNode;

			else
				return rangeOriginNode->priorNode;

		}

	}

	//reverse the order of all nodes in this list;
	//note: ***has no net effect if this list has only 1 node***
	void invert_WholeList()
	{

		//initial setup
		UniformNode<T> *laggingNode(firstNode);

		//this list is populated
		if (laggingNode)
		{

			UniformNode<T> *leadingNode = firstNode->nextNode;

			//partial inversion of the range first node
			laggingNode->nextNode = NULL;

			//disjointed inversion of any/all intervening range nodes
			while (leadingNode)
			{

				laggingNode->priorNode = leadingNode;

				laggingNode = leadingNode;
				leadingNode = leadingNode->nextNode;

				laggingNode->nextNode = laggingNode->priorNode;

			}

			//partial inversion of the range last node
			laggingNode->priorNode = NULL;

			lastNode = firstNode;
			firstNode = laggingNode;

		}

	}

	//relocate a node from current position to new position in this list
	//assumes nodeToMove is not NULL, is a member of this list; insertPoint
	//is NULL or a member of this list and is not nodeToMove; returns the
	//address of node which bounds the repositioned nodeToMove on side opposite
	//side of insertPoint, or the address of node immediately prior to or
	//after the inserted node if insertPoint is NULL and insertAfter is
	//true or false respectively; NULL if the intended return address is list
	//start or end; ***note: has no net effect save return if nodeToMove is already
	//at the intended relocation destination***
	UniformNode<T> *move_Node(UniformNode<T> *nodeToMove,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		//set starting values
		UniformNode<T> *anteriorBoundaryNode(nodeToMove->priorNode);
		UniformNode<T> *posteriorBoundaryNode(nodeToMove->nextNode);

		//stitch up around nodeToMove
		if (anteriorBoundaryNode)
		{

			anteriorBoundaryNode->nextNode = posteriorBoundaryNode;

			if (posteriorBoundaryNode)
				posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

			else
				lastNode = anteriorBoundaryNode;

		}

		else
		{

			if (posteriorBoundaryNode)
			{

				firstNode = posteriorBoundaryNode;
				posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

			}

			//list is empty save for nodeToMove; no further action
			//needed save return
			else
				return NULL;

		}

		//insert nodeToMove and return
		if (!insertPoint)
		{

			if (insertAfter)
			{

				nodeToMove->priorNode = lastNode;
				nodeToMove->nextNode = NULL;

				lastNode->nextNode = nodeToMove;
				lastNode = nodeToMove;

				return nodeToMove->priorNode;

			}

			else
			{

				nodeToMove->priorNode = NULL;
				nodeToMove->nextNode = firstNode;

				firstNode->priorNode = nodeToMove;
				firstNode = nodeToMove;

				return nodeToMove->nextNode;

			}

		}

		else
		{

			UniformNode<T> *insertionBoundaryNode;

			if (insertAfter)
			{

				insertionBoundaryNode = insertPoint->nextNode;

				nodeToMove->priorNode = insertPoint;
				nodeToMove->nextNode = insertionBoundaryNode;

				insertPoint->nextNode = nodeToMove;

				if (insertionBoundaryNode)
					insertionBoundaryNode->priorNode = nodeToMove;

				else
					lastNode = nodeToMove;

			}

			else
			{

				insertionBoundaryNode = insertPoint->priorNode;

				nodeToMove->priorNode = insertionBoundaryNode;
				nodeToMove->nextNode = insertPoint;

				if (insertionBoundaryNode)
					insertionBoundaryNode->nextNode = nodeToMove;

				else
					firstNode = nodeToMove;

				insertPoint->priorNode = nodeToMove;

			}

			return insertionBoundaryNode;

		}

	}

	//relocate a range from current position to new position in this list
	//assumes rangeFirstNode and rangeLastNode are not NULL, are members of this list,
	//and encompass a valid range in their respective positions; insertPoint is
	//either NULL or a member of this list, and is not contained within the
	//range to be relocated; returns the address of node which bounds the relocated
	//range on side opposite insertPoint, or the address of node immediately prior to
	//or after the relocated range node if insertPoint is NULL and insertAfter is true or
	//false respectively; NULL if the intended return address is list start or end;
	//***note: has no net effect save return if the range is already at the intended
	//relocation destination***
	bool move_Range(UniformNode<T> *rangeFirstNode, UniformNode<T> *rangeLastNode,
		UniformNode<T> *insertPoint = NULL, const bool insertAfter = true)
	{

		//set starting values
		UniformNode<T> *anteriorBoundaryNode(rangeFirstNode->priorNode);
		UniformNode<T> *posteriorBoundaryNode(rangeLastNode->nextNode);

		//stitch up around the range
		if (anteriorBoundaryNode)
		{

			anteriorBoundaryNode->nextNode = posteriorBoundaryNode;

			if (posteriorBoundaryNode)
				posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

			else
				lastNode = anteriorBoundaryNode;

		}

		else
		{

			if (posteriorBoundaryNode)
			{

				firstNode = posteriorBoundaryNode;
				posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

			}

			//list is empty save for the range to move; no further action
			//needed save return
			else
				return NULL;

		}

		//insert range and return
		if (!insertPoint)
		{

			if (insertAfter)
			{

				rangeFirstNode->priorNode = lastNode;
				rangeLastNode->nextNode = NULL;

				lastNode->nextNode = rangeFirstNode;
				lastNode = rangeLastNode;

				return rangeFirstNode->priorNode;

			}

			else
			{

				rangeFirstNode->priorNode = NULL;
				rangeLastNode->nextNode = firstNode;

				firstNode->priorNode = rangeLastNode;
				firstNode = rangeFirstNode;

				return rangeLastNode->nextNode;

			}

		}

		else
		{

			UniformNode<T> *insertionBoundaryNode;

			if (insertAfter)
			{

				insertionBoundaryNode = insertPoint->nextNode;

				rangeFirstNode->priorNode = insertPoint;
				rangeLastNode->nextNode = insertionBoundaryNode;

				insertPoint->nextNode = rangeFirstNode;

				if (insertionBoundaryNode)
					insertionBoundaryNode->priorNode = rangeLastNode;

				else
					rangeLastNode = lastNode;

			}

			else
			{

				insertionBoundaryNode = insertPoint->priorNode;

				rangeFirstNode->priorNode = insertionBoundaryNode;
				rangeLastNode->nextNode = insertPoint;

				if (insertionBoundaryNode)
					insertionBoundaryNode->nextNode = rangeFirstNode;

				else
					rangeFirstNode = firstNode;

				insertPoint->priorNode = rangeLastNode;

			}

			return insertionBoundaryNode;

		}

	}

	//relocate a range from current position to new position in this list
	//assumes rangeOriginNode is not NULL and is a member of this list;
	//there are at least nodeCount nodes between and including rangeOriginNode
	//and list end or start if countForward is true or false respectively;
	//nodeCount is > 0; insertPoint is either NULL or a member of this list,
	//and is not contained within the target range; returns the address of node
	//which bounds the relocated range on side opposite insertPoint, or the
	//address of node immediately prior to or after the relocated range node if
	//insertPoint is NULL and insertAfter is true or false respectively; NULL if
	//the intended return address is list start or end; ***note: has no effect
	//save return if the range is already at the intended relocation destination***

	bool move_Range(UniformNode<T> *rangeOriginNode, unsigned long long nodeCount,
		const bool countForward = true, UniformNode<T> *insertPoint = NULL,
		const bool insertAfter = true)
	{

		//initial setup
		UniformNode<T> *rangeTerminalNode(rangeOriginNode);
		UniformNode<T> *anteriorBoundaryNode;
		UniformNode<T> *posteriorBoundaryNode;

		if (countForward)
		{

			//set values
			anteriorBoundaryNode = rangeOriginNode->priorNode;

			while (nodeCount > 1)
			{

				rangeTerminalNode = rangeTerminalNode->nextNode;
				nodeCount--;

			}

			posteriorBoundaryNode = rangeTerminalNode->nextNode;

			//stitch up around the range
			if (anteriorBoundaryNode)
			{

				anteriorBoundaryNode->nextNode = posteriorBoundaryNode;

				if (posteriorBoundaryNode)
					posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

				else
					lastNode = anteriorBoundaryNode;

			}

			else
			{

				if (posteriorBoundaryNode)
				{

					firstNode = posteriorBoundaryNode;
					posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

				}

				//list is empty save for the range to move; no further action
				//needed save return
				else
					return NULL;

			}

			//insert the range at new position and return
			if (!insertPoint)
			{

				if (insertAfter)
				{

					rangeOriginNode->priorNode = lastNode;
					rangeTerminalNode->nextNode = NULL;

					lastNode->nextNode = rangeOriginNode;
					lastNode = rangeTerminalNode;

					return rangeOriginNode->priorNode;

				}

				else
				{

					rangeOriginNode->priorNode = NULL;
					rangeTerminalNode->nextNode = firstNode;

					firstNode->priorNode = rangeTerminalNode;
					firstNode = rangeOriginNode;

					return rangeTerminalNode->nextNode;

				}

			}

			else
			{

				UniformNode<T> *insertionBoundaryNode;

				if (insertAfter)
				{

					insertionBoundaryNode = insertPoint->nextNode;

					rangeOriginNode->priorNode = insertPoint;
					rangeTerminalNode->nextNode = insertionBoundaryNode;

					insertPoint->nextNode = rangeOriginNode;

					if (insertionBoundaryNode)
						insertionBoundaryNode->priorNode = rangeTerminalNode;

					else
						rangeTerminalNode = lastNode;

				}

				else
				{

					insertionBoundaryNode = insertPoint->priorNode;

					rangeOriginNode->priorNode = insertionBoundaryNode;
					rangeTerminalNode->nextNode = insertPoint;

					if (insertionBoundaryNode)
						insertionBoundaryNode->nextNode = rangeOriginNode;

					else
						rangeOriginNode = firstNode;

					insertPoint->priorNode = rangeTerminalNode;

				}

				return insertionBoundaryNode;

			}

		}

		else
		{

			//set values
			posteriorBoundaryNode = rangeOriginNode->nextNode;

			while (nodeCount > 1)
			{

				rangeTerminalNode = rangeTerminalNode->priorNode;
				nodeCount--;

			}

			anteriorBoundaryNode = rangeTerminalNode->priorNode;

			//stitch up around the range
			if (anteriorBoundaryNode)
				anteriorBoundaryNode->nextNode = posteriorBoundaryNode;

			else
				firstNode = posteriorBoundaryNode;

			if (posteriorBoundaryNode)
				posteriorBoundaryNode->priorNode = anteriorBoundaryNode;

			else
				lastNode = anteriorBoundaryNode;

			//insert the range at new position and return
			if (!insertPoint)
			{

				if (insertAfter)
				{

					rangeTerminalNode->priorNode = lastNode;
					rangeOriginNode->nextNode = NULL;

					lastNode->nextNode = rangeTerminalNode;
					lastNode = rangeOriginNode;

					return rangeTerminalNode->priorNode;

				}

				else
				{

					rangeTerminalNode->priorNode = NULL;
					rangeOriginNode->nextNode = firstNode;

					firstNode->priorNode = rangeOriginNode;
					firstNode = rangeTerminalNode;

					return rangeOriginNode->nextNode;

				}

			}

			else
			{

				UniformNode<T> *insertionBoundaryNode;

				if (insertAfter)
				{

					insertionBoundaryNode = insertPoint->nextNode;

					rangeTerminalNode->priorNode = insertPoint;
					rangeOriginNode->nextNode = insertionBoundaryNode;

					insertPoint->nextNode = rangeTerminalNode;

					if (insertionBoundaryNode)
						insertionBoundaryNode->priorNode = rangeOriginNode;

					else
						rangeOriginNode = lastNode;

				}

				else
				{

					insertionBoundaryNode = insertPoint->priorNode;

					rangeTerminalNode->priorNode = insertionBoundaryNode;
					rangeOriginNode->nextNode = insertPoint;

					if (insertionBoundaryNode)
						insertionBoundaryNode->nextNode = rangeTerminalNode;

					else
						rangeTerminalNode = firstNode;

					insertPoint->priorNode = rangeOriginNode;

				}

				return insertionBoundaryNode;

			}

		}

	}

	//copy contents from a source range into a target range, resizing the
	//target range as needed; sourceRangeFirstNode and SourceRangeLastNode
	//are not NULL, are members of the same list, and encompass a valid range
	//in their respective positions; targetRangeFirstNode and targetRangeLastNode
	//are not NULL, are members of this list, and encompass a valid range in their
	//respective positions; returns address of the node after or before the replaced
	//range in list position if returnNext is true or false respectively, or NULL
	//if the intended return address is list start or end
	UniformNode<T> *replace_Range(UniformNode<T> *sourceRangeFirstNode,
		UniformNode<T> *sourceRangeLastNode, UniformNode<T> *targetRangeFirstNode,
		UniformNode<T> *targetRangeLastNode, const bool returnNext = true)
	{

		//initial setup
		UniformNode<T> *sourceRangeNode = sourceRangeFirstNode;
		UniformNode<T> *targetRangeNode = targetRangeFirstNode;
		
		sourceRangeLastNode = sourceRangeLastNode->nextNode;
		targetRangeLastNode = targetRangeLastNode->nextNode;

		//main loop
		do
		{

			//copy content from source to target node
			targetRangeNode->content = sourceRangeNode->content;

			//iterate the iterators
			sourceRangeNode = sourceRangeNode->nextNode;
			targetRangeNode = targetRangeNode->nextNode;

			//test for various states
			if (sourceRangeNode == sourceRangeLastNode)
			{

				//source and target ranges are the same size - all iterations complete
				if (targetRangeNode == targetRangeLastNode)
				{

					if (returnNext)
						return targetRangeLastNode;

					else
						return targetRangeFirstNode;

				}

				//only source range iteration is complete
				else
				{

					//delete excess nodes in target range
					sourceRangeFirstNode = targetRangeNode->priorNode;
					unsigned long long deletionCount = 0;

					do
					{

						sourceRangeNode = targetRangeNode->nextNode;

						delete targetRangeNode;
						targetRangeNode = sourceRangeNode;

						deletionCount++;

					} while (targetRangeNode != targetRangeLastNode);

					//stitch up target range
					if (sourceRangeFirstNode)
						sourceRangeFirstNode->nextNode = targetRangeLastNode;

					else
						firstNode = targetRangeLastNode;

					if (targetRangeLastNode)
						targetRangeLastNode->priorNode = sourceRangeFirstNode;

					else
						lastNode = sourceRangeFirstNode;

					//wrap up and return
					listLength -= deletionCount;

					if (returnNext)
						return targetRangeLastNode;

					else
						return targetRangeFirstNode;

				}

			}

			//only target range iteration is complete
			else if (targetRangeNode == targetRangeLastNode)
			{

				//add additional node(s) in target range
				unsigned long long additionCount = 0;

				do
				{

					targetRangeNode = new UniformNode<T>();
					targetRangeNode->content = sourceRangeNode->content;

					sourceRangeFirstNode->nextNode = targetRangeNode;
					targetRangeNode->priorNode = sourceRangeFirstNode;

					sourceRangeFirstNode = targetRangeNode;

					sourceRangeNode = sourceRangeNode->nextNode;
					additionCount++;

				} while (sourceRangeNode != sourceRangeLastNode);

				//stitch up target range
				if (sourceRangeFirstNode)
					sourceRangeFirstNode->nextNode = targetRangeLastNode;

				else
					firstNode = targetRangeLastNode;

				if (targetRangeLastNode)
					targetRangeLastNode->priorNode = sourceRangeFirstNode;

				else
					lastNode = sourceRangeFirstNode;

				//wrap up and return
				listLength += additionCount;

				if (returnNext)
					return targetRangeLastNode;

				else
					return targetRangeFirstNode;

			}

		} while (true);

		//this should never be reached
		return NULL;

	}

	//test whether this list contains any node(s); returns true if so, false if not
	bool test_ListPopulated() const
	{
		if (firstNode)
			return true;

		else
			return false;

	}

	//test whether a node is contained in this list; assumes nodeToTest != NULL;
	//returns true if nodeToTest is a member of this list, false if not
	bool test_NodeListMembership(UniformNode<T> *nodeToTest,
		const bool iterateForward = true) const
	{

		UniformNode<T> *nodeAddress;

		if (iterateForward)
		{

			nodeAddress = firstNode;

			while (nodeAddress)
			{

				if (nodeAddress == nodeToTest)
					return true;

				nodeAddress = nodeAddress->nextNode;

			} while (nodeToTest);

		}

		else
		{

			nodeAddress = lastNode;

			while (nodeAddress)
			{

				if (nodeAddress == nodeToTest)
					return true;

				nodeAddress = nodeAddress->priorNode;

			} while (nodeToTest);

		}

		return false;

	}





	/***************************** FUNCTION DOCUMENTATION ******************************
	Function:	bool test_NodeRangeMembership() const
	Purpose:	test whether a node is contained within a bounded range
	Inputs:		const UniformNode<T> *nodeToTest - the node being searched for
				UniformNode<T> *rangeFirstNode - first node of the target range
				UniformNode<T> *rangeLastNode - last node of the target range
	Assumes:	nodeToTest is not NULL and is a member of this list, rangeFirstNode and
				rangeLastNode are not NULL, are members of this list, and encompass a
				valid range in their respective positions, general class assumptions
				(UniformNode, UniformList)
	Returns:	true if nodeToTest is found within the target range, false otherwise
	Effects:	(none)
	***********************************************************************************/
	bool test_NodeRangeMembership(const UniformNode<T> *nodeToTest,
		UniformNode<T> *rangeFirstNode, UniformNode<T> *rangeLastNode) const
	{

		rangeLastNode = rangeLastNode->nextNode;

		do
		{

			if (rangeFirstNode == nodeToTest)
				return true;

			rangeFirstNode = rangeFirstNode->nextNode;

		} while (rangeFirstNode != rangeLastNode);

		return false;

	}

	//test whether a node is contained within a counted range
	//assumes nodeToTest is not NULL and is a member of this list,
	//rangeOriginNode is not NULL and is a member of this list;
	//there are at least nodeCount nodes between and including
	//rangeOriginNode and capNode or rootNode if countForward is
	//true or false respectively; nodeCount is > 0; returns true
	//if nodeToTest is found within the target range, false otherwise
	bool test_NodeRangeMembership(const UniformNode<T> *nodeToTest,
		UniformNode<T> *rangeOriginNode, unsigned long long nodeCount,
		const bool countForward) const
	{

		if (countForward)
		{

			do
			{

				if (rangeOriginNode == nodeToTest)
					return true;

				rangeOriginNode = rangeOriginNode->nextNode;
				nodeCount--;

			} while (nodeCount);

			return false;

		}

		else
		{

			do
			{

				if (rangeOriginNode == nodeToTest)
					return true;

				rangeOriginNode = rangeOriginNode->priorNode;
				nodeCount--;

			} while (nodeCount);

			return false;

		}

	}

	//test whether the specified range first and last nodes encompass a valid
	//range in their respective positions; assumes rangeFirstNode and rangeLastNode
	//are not NULL and are members of this list; returns true if the two parameters
	//form a valid range in their respective positions, false otherwise
	bool test_RangeValidity(UniformNode<T> *rangeFirstNode,
		const UniformNode<T> *rangeLastNode) const
	{

		do
		{

			if (rangeFirstNode == rangeLastNode)
				return true;

			rangeFirstNode = rangeFirstNode->nextNode;

		} while (rangeFirstNode);

		return false;

	}

	//test whether the specified origin node, length, and direction define
	//a valid range; assumes rangeOriginNode is not NULL and is a member
	//of this list; nodeCount > 0; returns true if the counted range is valid,
	//false if list start or end is reached before count is completed
	bool test_RangeValidity(UniformNode<T> *rangeOriginNode,
		unsigned long long nodeCount, const bool countForward) const
	{

		nodeCount--;

		if (countForward)
		{

			while (nodeCount)
			{

				rangeOriginNode = rangeOriginNode->nextNode;
				nodeCount--;

				if (!rangeOriginNode)
					return false;

			}

		}

		else
		{

			while (nodeCount)
			{

				rangeOriginNode = rangeOriginNode->priorNode;
				nodeCount--;

				if (!rangeOriginNode)
					return false;

			}

		}

		return true;

	}

	//test whether the specified range first and last nodes encompass a valid
	//range in their respective positions and are members of this list;
	//assumes rangeFirstNode and rangeLastNode are not NULL; returns true
	//if the two parameters form a valid range in their respective positions
	//and are members of this list; false if a list end is reached
	//before the terminal boundary node, or if rangeFirstNode or
	//rangeLastNode are not members of this list
	bool test_RangeValidityAndMembership(UniformNode<T> *rangeFirstNode,
		UniformNode<T> *rangeLastNode, const bool iterateForward = true) const
	{

		if (iterateForward)
		{

			//validate the range
			while (rangeFirstNode != rangeLastNode)
			{

				rangeFirstNode = rangeFirstNode->nextNode;

				if (!rangeFirstNode)
					return false;

			}

			//ensure the range is contained within this list
			rangeLastNode = rangeLastNode->nextNode;

			while (rangeLastNode)
			{

				rangeFirstNode = rangeLastNode;

				rangeLastNode = rangeLastNode->nextNode;

			}

			if (rangeFirstNode == lastNode)
				return true;

			else
				return false;

		}

		else
		{

			//validate the range
			while (rangeLastNode != rangeFirstNode)
			{

				rangeLastNode = rangeLastNode->priorNode;

				if (!rangeLastNode)
					return false;

			}

			//ensure the range is contained within this list
			rangeFirstNode = rangeFirstNode->priorNode;

			while (rangeFirstNode)
			{

				rangeLastNode = rangeFirstNode;

				rangeFirstNode = rangeFirstNode->priorNode;

			}

			if (rangeLastNode == firstNode)
				return true;

			else
				return false;

		}

	}

	//test whether the specified origin node, length, and direction define
	//a valid range and check that origin node is a member of this list
	//assumes rangeOriginNode != NULL; nodeCount > 0; returns true if the
	//counted range is valid and rangeOriginNode is a member of this list;
	//false if list end or start is reached before count is completed or
	//if rangeOriginNode is not a member of this list
	bool test_RangeValidityAndMembership(UniformNode<T> *rangeOriginNode,
		unsigned long long nodeCount, const bool countForward) const
	{

		UniformNode<T> *leadingNode;
		nodeCount--;

		if (countForward)
		{

			//validate the range
			while (nodeCount)
			{

				rangeOriginNode = rangeOriginNode->nextNode;
				nodeCount--;

				if (!rangeOriginNode)
					return false;

			}

			//ensure the range is contained within this list
			leadingNode = rangeOriginNode->nextNode;

			while (leadingNode)
			{

				rangeOriginNode = leadingNode;

				leadingNode = leadingNode->nextNode;

			}

			if (rangeOriginNode == lastNode)
				return true;

			else
				return false;

		}

		else
		{

			//validate the range
			while (nodeCount)
			{

				rangeOriginNode = rangeOriginNode->priorNode;
				nodeCount--;

				if (!rangeOriginNode)
					return false;

			}

			//ensure the range is contained within this list
			leadingNode = rangeOriginNode->priorNode;

			while (leadingNode)
			{

				rangeOriginNode = leadingNode;

				leadingNode = leadingNode->priorNode;

			}

			if (rangeOriginNode == firstNode)
				return true;

			else
				return false;

		}

	}

};

template<typename T> class ExtensibleArray
{

private:

	//total number of elements in the content array
	unsigned long long arrayLength;

	//pointer to the array stored on the heap
	T *content;

public:

	//default constructor
	ExtensibleArray() : arrayLength(0), content(NULL) {}

	//modified constructor - sets number of content elements to length
	ExtensibleArray(const unsigned long long length) : arrayLength(length)
	{

		if (length)
			content = new T[length];

		else
			content = NULL;

	}

	/************************ COPY CONSTRUCTOR DOCUMENTATION ***************************
	Function:	ExtensibleArray()
	Purpose:	copy construction; "deep" copies otherArray
	Inputs:		const ExtensibleArray<T>& otherArray - the array to be copied into this
	Assumes:	general class assumptions
	Returns:	...
	Effects:	creates a copy of otherArrays content in this ExtensibleArray
	***********************************************************************************/
	ExtensibleArray(const ExtensibleArray<T>& otherArray)
	{

		//copy in the new content
		arrayLength = otherArray.arrayLength;
		content = new T[arrayLength];

		for (unsigned long long count = 0; count < arrayLength; count++)
			content[count] = otherArray.content[count];

	}

	ExtensibleArray(UniformList<T> const& otherList)
	{

		//copy in the new content
		arrayLength = otherList.get_ListLength();

		content = new T[arrayLength];

		UniformNode<T> *listNode(otherList.get_FirstNode());
		unsigned long long count(0);

		while (listNode)
		{

			content[count] = listNode->content;

			listNode = listNode->get_NextNode();
			count++;

		}
			
	}

	/************************ MOVE CONSTRUCTOR DOCUMENTATION ***************************
	Function:	ExtensibleArray()
	Purpose:	move construction - moves otherArray content to this
	Inputs:		const ExtensibleArray<T>& otherArray - the array to be moved to this
	Assumes:	general class assumptions
	Returns:	...
	Effects:	moves otherArrays content into this ExtensibleArray
	***********************************************************************************/
	ExtensibleArray(const ExtensibleArray<T>&& otherArray)
	{

		//move otherArray's content to this
		arrayLength = otherArray.arrayLength;
		content = otherArray.content;

		//set otherArray to null state
		otherArray.arrayLength = 0;
		otherArray.content = NULL;

	}

	/****************** COPY ASSIGNMENT CONSTRUCTOR DOCUMENTATION **********************
	Function:	ExtensibleArray<T>& operator=()
	Purpose:	operator overload to ensure array copies are "deep" not "shallow"
	Inputs:		ExtensibleArray<T> const - the array to be copied into this one
	Assumes:	general class assumptions
	Returns:	*this
	Effects:	deletes this array content, reallocates new memory based on otherArray's
				arrayLength, and copies content from otherArray to this
	***********************************************************************************/
	ExtensibleArray<T>& operator=(ExtensibleArray<T> const& otherArray)
	{

		if (&otherArray != this)
		{

			//delete the old content
			delete[] content;

			//copy in the new content
			arrayLength = otherArray.arrayLength;
			content = new T[arrayLength];

			for (unsigned long long count = 0; count < arrayLength; count++)
				content[count] = otherArray.content[count];

		}

		return *this;

	}

	/************* OVERLOADED COPY ASSIGNMENT CONSTRUCTOR DOCUMENTATION ****************
	Function:	ExtensibleArray<T>& operator=()
	Purpose:	operator overload to allow easy copying from UniformLists
	Inputs:		UniformList<T> const - the list to be copied into this array
	Assumes:	general ExtensibleArray and UniformList class assumptions
	Returns:	*this
	Effects:	deletes this array content, reallocates new memory based on otherlist's
				listLength, and copies content from otherList to this
	***********************************************************************************/
	ExtensibleArray<T>& operator=(UniformList<T> const& otherList)
	{

		UniformNode<T> *listNode = otherList.get_LastNode();

		if (listNode)
		{

			unsigned long long listLength = otherList.get_ListLength();

			//delete the old content
			delete[] content;

			//copy in the new content
			arrayLength = listLength;
			content = new T[arrayLength];

			do
			{

				listLength--;

				*(get_ElementAddress(listLength)) = listNode->content;

				listNode = listNode->get_PriorNode();

			} while (listLength > 0);

		}

		else
		{

			//delete the old content
			delete[] content;

			//set to null stae
			arrayLength = 0;
			content = NULL;

		}

		return *this;

	}

	/****************** MOVE ASSIGNMENT CONSTRUCTOR DOCUMENTATION **********************
	Function:	ExtensibleArray<T>& operator=()
	Purpose:	...
	Inputs:		...
	Assumes:	general class assumptions
	Returns:	*this
	Effects:	not sure if this works or is safe
	***********************************************************************************/
	ExtensibleArray<T>& operator=(ExtensibleArray<T> &&otherArray)
	{

		if (&otherArray != this)
		{

			//delete the old content
			delete[] content;

			//move contents to this ExtensibleArray
			arrayLength = otherArray.arrayLength;
			content = otherArray.content;

			//set otherArray to a null state
			otherArray.arrayLength = 0;
			otherArray.content = NULL;

		}

		return *this;

	}

	/*********************** DESTRUCTOR DOCUMENTATION **********************************
	Function:	~ExtensibleArray()
	Purpose:	object destruction; deallocates the dynamic array
	Inputs:		...
	Assumes:	...
	Returns:	...
	Effects:	...
	***********************************************************************************/
	~ExtensibleArray()
	{

		//NULL pointer deletion is safe
		delete[] content;

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	void increment_NoCopy()
	Purpose:	increments the size of the content array but leaves array elements
					uninitialized
	Inputs:		unsigned long long incrementLength - number of additional elements
					in the new array
	Assumes:	...
	Returns:	...
	Effects:	any/all data previously stored in content are lost
	***********************************************************************************/
	void increment_NoCopy(unsigned long long incrementLength)
	{

		if (!incrementLength)
			return;

		incrementLength += arrayLength;

		T *tempContent = new T[incrementLength];

		//NULL pointer deletion is safe
		delete[] content;

		arrayLength = incrementLength;
		content = tempContent;

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	void increment_WithCopy()
	Purpose:	increments the size of the content array, copying old content elements
					into the new array
	Inputs:		unsigned long long incrementLength - number of additional elements in
					the new array
				const bool extendAtEnd - controls whether the additional elements are
					added at the end of the array (true) or the beginning (false)
	Assumes:	...
	Returns:	...
	Effects:	...
	***********************************************************************************/
	void increment_WithCopy(const unsigned long long incrementLength,
		const bool extendAtEnd = true)
	{

		if (!incrementLength)
			return;

		incrementLength += arrayLength;

		T *tempContent = new T[incrementLength];
		unsigned long long count = 0;

		//copy elements forward from the start
		if (extendAtEnd)
		{

				while (count < arrayLength)
				{

					tempContent[count] = content[count];
					count++;

				}

		}

		//copy elements backward from the end
		else
		{

			unsigned long long difference = incrementLength - arrayLength;
			count = arrayLength - 1;

			while (count)
			{

				tempContent[count + difference] = content[count];
				count--;

			}

			tempContent[difference] = content[count];

		}

		//NULL pointer deletion is safe
		delete[] content;

		arrayLength = incrementLength;
		content = tempContent;

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	void resize_NoCopy()
	Purpose:	resizes the content but leaves array elements uninitialized
	Inputs:		const unsigned long long newLength - number of elements in the new array
	Assumes:	...
	Returns:	...
	Effects:	any/all data previously stored in content are lost
	***********************************************************************************/
	void resize_NoCopy(const unsigned long long newLength)
	{

		if (!newLength)
			return;

		T *tempContent = new T[newLength];

		//NULL pointer deletion is safe
		delete[] content;

		arrayLength = newLength;
		content = tempContent;

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	void resize_WithCopy()
	Purpose:	resizes the content, copying old content elements into the new array
	Inputs:		const unsigned long long newLength - number of elements in the new array
				const bool copyFromStart - controls whether the old content element(s)
					are copied into the new array fist to last (true) or last to (first)
					as space allows in the new array
	Assumes:	...
	Returns:	...
	Effects:	...
	***********************************************************************************/
	void resize_WithCopy(const unsigned long long newLength,
		const bool copyFromStart = true)
	{

		if (!newLength)
			return;

		T *tempContent = new T[newLength];
		unsigned long long count = 0;

		//copy elements forward from the start
		if (copyFromStart)
		{

			//new array is longer than orignal
			if (newLength > arrayLength)
			{

				while (count < arrayLength)
				{

					tempContent[count] = content[count];
					count++;

				}

			}

			//original array is longer or equal to new
			else
			{

				while (count < newLength)
				{

					tempContent[count] = content[count];
					count++;

				}

			}

		}

		//copy elements backward from the end
		else
		{

			long long difference = newLength - arrayLength;
			count = arrayLength - 1;

			//new array is longer than orignal
			if (difference > 0)
			{

				while (count)
				{

					tempContent[count + difference] = content[count];
					count--;

				}

				tempContent[difference] = content[count];

			}

			//original array is longer or equal to new
			else
			{

				difference = difference * (-1);

				while (count > difference)
				{

					tempContent[count] = content[count + difference];
					count--;

				}

				//final iteration
				tempContent[count] = content[count + difference];

			}

		}
	
		//NULL pointer deletion is safe
		delete[] content;

		arrayLength = newLength;
		content = tempContent;

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	unsigned long long get_ArrayLength() const
	Purpose:	provides public access to arrayLength
	Inputs:		...
	Assumes:	...
	Returns:	...
	Effects:	...
	***********************************************************************************/
	unsigned long long get_ArrayLength() const
	{

		return arrayLength;

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	T get_Element() const
	Purpose:	provides public access to content array element address
	Inputs:		...
	Assumes:	elementIndex < arrayLength
	Returns:	dereferenced element at provided elementIndex
	Effects:	...
	************************************************************************************/
	T get_Element(unsigned long long elementIndex) const
	{

		return content[elementIndex];

	}

	/************************ FUNCTION DOCUMENTATION ***********************************
	Function:	T *get_ElementAddress()
	Purpose:	provides public access to content array element address
	Inputs:		...
	Assumes:	elementIndex < arrayLength
	Returns:	address of element at provided elementIndex
	Effects:	...
	************************************************************************************/
	T *get_ElementAddress(unsigned long long elementIndex) const
	{

		return &content[elementIndex];

	}

	//testing only//
	T *get_ElementAddress_Safe(unsigned long long elementIndex) const
	{

		if (elementIndex < arrayLength)
			return &content[elementIndex];

		else
			return NULL;

	}

};

template<typename T> class BranchingNode
{

private:

	//pointer to the parent node, NULL if this node has no parent
	//(if at the first level within a list)
	BranchingNode<T> *parentNode;

	//pointer to prior sibling (NULL if no prior sibling)
	BranchingNode<T> *priorSibling;

	//pointer to next sibling (NULL if no next sibling)
	BranchingNode<T> *nextSibling;

	//pointer to this node's first child, (NULL if no children)
	BranchingNode<T> *firstChild;

	//pointer to this node's last child, (NULL if no children)
	BranchingNode<T> *lastChild;

	//count of the number of children assigned to this node
	unsigned long long totalChildren;

	//modified constructor; assumes all parameters validated in BranchingList context
	BranchingNode(BranchingNode<T> *aParentNode, BranchingNode<T> *aPriorSibling,
	BranchingNode<T> *aNextSibling) : parentNode(aParentNode),
	priorSibling(aPriorSibling), nextSibling(aNextSibling),
	firstChild(NULL), lastChild(NULL), totalChildren(0) {}

	//modified constructor; for copying in specified content, assumes
	//all parameters validated in BranchingList context
	BranchingNode(const T &contentToCopy, BranchingNode<T> *aParentNode, BranchingNode<T> *aPriorSibling,
		BranchingNode<T> *aNextSibling) : content(contentToCopy), parentNode(aParentNode),
		priorSibling(aPriorSibling), nextSibling(aNextSibling),
		firstChild(NULL), lastChild(NULL), totalChildren(0) {}

public:

	T content;

	//default constructor
	BranchingNode() : parentNode(NULL), priorSibling(NULL), nextSibling(NULL),
		firstChild(NULL), lastChild(NULL), totalChildren(0) {}

	//default destructor
	~BranchingNode() {}

	//getter - returns the parentNode of this node or NULL if
	//thisNode has no parent (unassigned or a list elder)
	BranchingNode<T> *get_Parent() const
	{

		return parentNode;

	}

	//getter - returns the priorSibling of this node,
	//(NULL if this node has no prior sibling)
	BranchingNode<T> *get_PriorSibling() const
	{
		
		return priorSibling;

	}

	//getter - returns the nextSibling of this node,
	//(NULL if this node has no next sibling)
	BranchingNode<T> *get_NextSibling() const
	{

		return nextSibling;

	}

	//getter - returns the first child of this node or
	//NULL if thisNode has no children
	BranchingNode<T> *get_FirstChild() const
	{

		return firstChild;

	}

	//getter - returns the last child of this node or
	//NULL if thisNode has no children
	BranchingNode<T> *get_LastChild() const
	{

		return lastChild;

	}

	//getter -returns the number of children of this node
	unsigned long long get_ChildCount() const
	{

		return totalChildren;

	}

	//returns this node's 'next' relative as exists in order of preference:
	//first child, or next sibling, or parent's next sibling (loops up through grandparent,
	//great grandparent, etc., while no parental next sibling exists), or NULL if no
	//'next' relative exists;
	BranchingNode<T> *get_NextRelative() const
	{

		if (firstChild)
			return firstChild;

		//no children
		else
		{

			if (nextSibling)
				return nextSibling;

			//no next siblings
			else
			{

				BranchingNode<JSON_Node> *tempNode(parentNode);

				while (tempNode)
				{

					if (tempNode->nextSibling)
						return tempNode->nextSibling;

					//no next siblings of the parent
					else
						tempNode = tempNode->parentNode;

				}

				//no 'next relative' exists
				return NULL;

			}

		}

	}

	//gives BranchingList access to private members
	template<typename S> friend class BranchingList;

};

//design concept: parent, sibling and child node pointers within
//each member BranchingNode convey the branching structure;
//a risk of this design is that circular loops can be created whereby
//access to a group of nodes could be lost (if a node is moved to
//become a child of one of its decendants)

template<typename T> class BranchingList
{

private:

	//pointers to the first and last elder nodes (nodes without any parentNode),
	//both are NULL if the list is empty, both point to the same node if only
	//one elder exists
	BranchingNode<T> *firstElder;
	BranchingNode<T> *lastElder;

	//the tracked number of elder nodes
	unsigned long long elderNodeCount;

	//the tracked number of total nodes
	unsigned long long totalNodeCount;

public:

	//default constructor
	BranchingList() : firstElder(NULL), lastElder(NULL),
		elderNodeCount(0), totalNodeCount(0) {}

	//default destructor
	~BranchingList()
	{

		BranchingNode<T> *currentChildNode = firstElder;
		
		//the list is empty
		if (!currentChildNode)
			return;
		
		BranchingNode<T> *currentParentNode = NULL;
		BranchingNode<T> *tempDeletionNode;

		do
		{

			//this node has children, dive down one level
			if (currentChildNode->firstChild)
			{

				currentParentNode = currentChildNode;
				currentChildNode = currentChildNode->firstChild;

			}

			//this node has no children
			else
			{

				tempDeletionNode = currentChildNode;
				currentChildNode = currentChildNode->nextSibling;

				//more siblings remain at this level
				if (currentChildNode)
				{

					//***TESTING***
					cout << "\n\nnode at address " << tempDeletionNode << " deleted successfully";
					//***TESTING***

					delete tempDeletionNode;

				}

				//no siblings remain at this level
				else
				{

					//this was not at the elder level, move up to the parent
					if (currentParentNode)
					{

						currentParentNode->firstChild = NULL;

						currentChildNode = currentParentNode;
						currentParentNode = currentParentNode->parentNode;

						//***TESTING***
						cout << "\n\nnode at address " << tempDeletionNode << " deleted successfully";
						//***TESTING***

						delete tempDeletionNode;

					}

					//this was at the elder level - final deletion
					else
					{

						//***TESTING***
						cout << "\n\nnode at address " << tempDeletionNode << " deleted successfully";
						//***TESTING***

						delete tempDeletionNode;

						return;

					}

				}
				
			}

		} while (true);

	}

	//getter - returns the total number of elder nodes
	unsigned long long get_ElderNodeCount() const
	{

		return elderNodeCount;

	}

	//getter - returns the total number of nodes in all branches
	unsigned long long get_TotalNodeCount() const
	{

		return totalNodeCount;

	}

	//getter - returns the first child of this list or
	//NULL if this list is empty
	BranchingNode<T> *get_FirstElder()
	{

		return firstElder;

	}

	//getter - returns the first child of this list or
	//NULL if this list is empty
	BranchingNode<T> *get_LastElder()
	{

		return lastElder;

	}

	//insert a new node in any position within the branching list
	//assumes parentNode is a node within this array (or NULL in which
	//case the inserted node will be an elder; assumes adjacentSiblingNode
	//is a child of parentNode or NULL; inserts the new node after or before
	//adjacentSiblingNode depending on whether insertAfter is true or false
	//respectively. Inserts at parentNode's firstChild or lastChild position
	//(or the lists first or last elder position if parentNode is NULL)
	//if adjacentSiblingNode is NULL and insterAfter is false or true respectively;
	//returns the address of the newly added node
	BranchingNode<T> *add_NewNode(BranchingNode<T> * parentNode = NULL,
		BranchingNode<T> *adjacentSiblingNode = NULL, const bool insertAfter = true)
	{

		BranchingNode<T> *newNode;

		if (parentNode)
		{

			if (adjacentSiblingNode)
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(parentNode, adjacentSiblingNode,
						adjacentSiblingNode->nextSibling);

					parentNode->totalChildren++;

					if (parentNode->lastChild == adjacentSiblingNode)
						parentNode->lastChild = newNode;

					else
						adjacentSiblingNode->nextSibling->priorSibling = newNode;

					adjacentSiblingNode->nextSibling = newNode;
					
				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(parentNode,
						adjacentSiblingNode->priorSibling, adjacentSiblingNode);

					parentNode->totalChildren++;

					if (parentNode->firstChild == adjacentSiblingNode)
						parentNode->firstChild = newNode;

					else
						adjacentSiblingNode->priorSibling->nextSibling = newNode;

					adjacentSiblingNode->priorSibling = newNode;

				}

			}

			//adjacentSiblingNode is NULL
			else
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(parentNode,
						parentNode->lastChild, NULL);

					parentNode->totalChildren++;

					if (parentNode->lastChild)
						parentNode->lastChild->nextSibling = newNode;

					else
						parentNode->firstChild = newNode;

					parentNode->lastChild = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(parentNode,
						NULL, parentNode->firstChild);

					parentNode->totalChildren++;

					if (parentNode->firstChild)
						parentNode->firstChild->priorSibling = newNode;

					else
						parentNode->lastChild = newNode;
					
					parentNode->firstChild = newNode;

				}

			}

		}

		//parentNode is NULL
		else
		{

			if (adjacentSiblingNode)
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(NULL, adjacentSiblingNode,
						adjacentSiblingNode->nextSibling);

					elderNodeCount++;

					if (lastElder == adjacentSiblingNode)
						lastElder = newNode;

					else
						adjacentSiblingNode->nextSibling->priorSibling = newNode;

					adjacentSiblingNode->nextSibling = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(NULL,
						adjacentSiblingNode->priorSibling, adjacentSiblingNode);

					elderNodeCount++;

					if (firstElder == adjacentSiblingNode)
						firstElder = newNode;

					else
						adjacentSiblingNode->priorSibling->nextSibling = newNode;

					adjacentSiblingNode->priorSibling = newNode;

				}

			}

			//adjacentSiblingNode is NULL
			else
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(NULL, lastElder, NULL);

					elderNodeCount++;

					if (lastElder)
						lastElder->nextSibling = newNode;

					else
						firstElder = newNode;

					lastElder = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(NULL, NULL, firstElder);

					elderNodeCount++;

					if (firstElder)
						firstElder->priorSibling = newNode;

					else
						lastElder = newNode;

					firstElder = newNode;

				}

			}

		}

		totalNodeCount++;
		return newNode;

	}

	//insert a new node in any position within the branching list and copies in new content
	//assumes parentNode is a node within this array (or NULL in which
	//case the inserted node will be an elder; assumes adjacentSiblingNode
	//is a child of parentNode or NULL; inserts the new node after or before
	//adjacentSiblingNode depending on whether insertAfter is true or false
	//respectively. Inserts at parentNode's firstChild or lastChild position
	//(or the lists first or last elder position if parentNode is NULL)
	//if adjacentSiblingNode is NULL and insterAfter is false or true respectively;
	//returns the address of the newly added node
	BranchingNode<T> *add_NewNode(const T &contentToCopy, BranchingNode<T> *parentNode = NULL,
		BranchingNode<T> *adjacentSiblingNode = NULL, const bool insertAfter = true)
	{

		BranchingNode<T> *newNode;

		if (parentNode)
		{

			if (adjacentSiblingNode)
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(contentToCopy, parentNode,
						adjacentSiblingNode, adjacentSiblingNode->nextSibling);

					parentNode->totalChildren++;

					if (parentNode->lastChild == adjacentSiblingNode)
						parentNode->lastChild = newNode;

					else
						adjacentSiblingNode->nextSibling->priorSibling = newNode;

					adjacentSiblingNode->nextSibling = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(contentToCopy, parentNode,
						adjacentSiblingNode->priorSibling, adjacentSiblingNode);

					parentNode->totalChildren++;

					if (parentNode->firstChild == adjacentSiblingNode)
						parentNode->firstChild = newNode;

					else
						adjacentSiblingNode->priorSibling->nextSibling = newNode;

					adjacentSiblingNode->priorSibling = newNode;

				}

			}

			//adjacentSiblingNode is NULL
			else
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(contentToCopy, parentNode,
						parentNode->lastChild, NULL);

					parentNode->totalChildren++;

					if (parentNode->lastChild)
						parentNode->lastChild->nextSibling = newNode;

					else
						parentNode->firstChild = newNode;

					parentNode->lastChild = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(contentToCopy, parentNode,
						NULL, parentNode->firstChild);

					parentNode->totalChildren++;

					if (parentNode->firstChild)
						parentNode->firstChild->priorSibling = newNode;

					else
						parentNode->lastChild = newNode;

					parentNode->firstChild = newNode;

				}

			}

		}

		//parentNode is NULL
		else
		{

			if (adjacentSiblingNode)
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(contentToCopy, NULL,
						adjacentSiblingNode, adjacentSiblingNode->nextSibling);

					elderNodeCount++;

					if (lastElder == adjacentSiblingNode)
						lastElder = newNode;

					else
						adjacentSiblingNode->nextSibling->priorSibling = newNode;

					adjacentSiblingNode->nextSibling = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(contentToCopy, NULL,
						adjacentSiblingNode->priorSibling, adjacentSiblingNode);

					elderNodeCount++;

					if (firstElder == adjacentSiblingNode)
						firstElder = newNode;

					else
						adjacentSiblingNode->priorSibling->nextSibling = newNode;

					adjacentSiblingNode->priorSibling = newNode;

				}

			}

			//adjacentSiblingNode is NULL
			else
			{

				if (insertAfter)
				{

					newNode = new BranchingNode<T>(contentToCopy, NULL,
						lastElder, NULL);

					elderNodeCount++;

					if (lastElder)
						lastElder->nextSibling = newNode;

					else
						firstElder = newNode;

					lastElder = newNode;

				}

				//insertAfter is false
				else
				{

					newNode = new BranchingNode<T>(contentToCopy, NULL,
						NULL, firstElder);

					elderNodeCount++;

					if (firstElder)
						firstElder->priorSibling = newNode;

					else
						lastElder = newNode;

					firstElder = newNode;

				}

			}

		}

		totalNodeCount++;
		return newNode;

	}

	//deletes nodeToDelete and all its descendants, returns nodeToDelete's parentNode
	//assumes nodeToDelete is not NULL and is a member of this list
	BranchingNode<T> *delete_Branch(BranchingNode<T> *nodeToDelete)
	{

		BranchingNode<T> *currentParentNode = nodeToDelete->parentNode;

		//nodeToDelete is not an elder
		if (currentParentNode)
		{

			//stitch out this node in its siblings and parent
			if (nodeToDelete->priorSibling)
				nodeToDelete->priorSibling->nextSibling = nodeToDelete->nextSibling;

			else
				currentParentNode->firstChild = nodeToDelete->nextSibling;

			if (nodeToDelete->nextSibling)
				nodeToDelete->nextSibling->priorSibling = nodeToDelete->priorSibling;

			else
				currentParentNode->lastChild = nodeToDelete->priorSibling;

			currentParentNode->totalChildren--;

		}

		//nodeToDelete is an elder
		else
		{

			//stitch out this node in its siblings and from the list's elders
			if (nodeToDelete->priorSibling)
				nodeToDelete->priorSibling->nextSibling = nodeToDelete->nextSibling;

			else
				firstElder = nodeToDelete->nextSibling;

			if (nodeToDelete->nextSibling)
				nodeToDelete->nextSibling->priorSibling = nodeToDelete->priorSibling;

			else
				lastElder = nodeToDelete->priorSibling;

			elderNodeCount--;

		}

		BranchingNode<T> *currentChildNode = nodeToDelete->firstChild;

		//nodeToDelete has no children
		if (!currentChildNode)
		{

			//***TESTING***
			cout << "\n\nnode at address " << nodeToDelete << " deleted successfully";
			//***TESTING***

			delete nodeToDelete;
			totalNodeCount--;

			return currentParentNode;

		}
		
		currentParentNode = nodeToDelete;
		BranchingNode<T> *tempDeletionNode;

		do
		{

			//this node has children, dive down one level
			if (currentChildNode->firstChild)
			{

				currentParentNode = currentChildNode;
				currentChildNode = currentChildNode->firstChild;

			}

			//this node has no children
			else
			{

				tempDeletionNode = currentChildNode;
				currentChildNode = currentChildNode->nextSibling;

				//more siblings remain at this level
				if (currentChildNode)
				{

					//***TESTING***
					cout << "\n\nnode at address " << tempDeletionNode << " deleted successfully";
					//***TESTING***

					delete tempDeletionNode;
					totalNodeCount--;

				}

				//no siblings remain at this level
				else
				{

					//this was not the last child of nodeToDelete, move up to the parent
					if (currentParentNode != nodeToDelete)
					{

						currentParentNode->firstChild = NULL;

						currentChildNode = currentParentNode;
						currentParentNode = currentParentNode->parentNode;

						//***TESTING***
						cout << "\n\nnode at address " << tempDeletionNode << " deleted successfully";
						//***TESTING***

						delete tempDeletionNode;
						totalNodeCount--;

					}

					//this was the last child of nodeToDelete - final deletions
					else
					{

						//***TESTING***
						cout << "\n\nnode at address " << tempDeletionNode << " deleted successfully";
						//***TESTING***

						delete tempDeletionNode;

						currentParentNode = nodeToDelete->parentNode;

						//***TESTING***
						cout << "\n\nnode at address " << nodeToDelete << " deleted successfully";
						//***TESTING***

						delete nodeToDelete;
						totalNodeCount -= 2;

						return currentParentNode;

					}

				}

			}

		} while (true);

	}

	//***I should also create a safety check function to be paired with this one in some cases
	//along the lines of the test range validity function in UniformList to ensure that
	//newParentNode is not a descendant of nodeToMove***

	//move an existing node branch to a new position within this branching list;
	//assumes branchOriginNode is a member of this list and not NULL; newParentNode
	//is NULL or both a member of this list and not a descendant of branchOriginNode;
	//newAdjacentSiblingNode is NULL or a child of newParentNode; inserts the new node
	//after or before newAdjacentSiblingNode depending on whether insertAfter is true or
	//false respectively, or inserts at newParentNode's firstChild or lastChild position
	//(or this lists first or last elder position if newParentNode is NULL) if newAdjacentSiblingNode
	//is NULL and insterAfter is false or true respectively; returns the address of the adjacent
	//sibling node opposite newAdjacentSiblingNode or the pentultimate node (at list beggining or end)
	//if newAdjacentSiblingNode is NULL ***note: relocating nodeToMove under one of it's
	//decendants will result in memory leak***
	BranchingNode<T> *move_Branch(BranchingNode<T> *branchOriginNode,
		BranchingNode<T> *newParentNode = NULL, BranchingNode<T> *newAdjacentSiblingNode = NULL,
		const bool insertAfter = true)
	{

		BranchingNode<T> *tempParent(branchOriginNode->parentNode);
		BranchingNode<T> *tempPriorSibling(branchOriginNode->priorSibling);
		BranchingNode<T> *tempNextSibling(branchOriginNode->nextSibling);

		//stitch branchOriginNode out of it's current parent and siblings
		if (tempParent)
		{

			tempParent->totalChildren--;

			if (tempPriorSibling)
				tempPriorSibling->nextSibling = tempNextSibling;

			else
				tempParent->firstChild = tempNextSibling;
			
			if (tempNextSibling)
				tempNextSibling->priorSibling = tempPriorSibling;

			else
				tempParent->lastChild = tempPriorSibling;

		}

		else
		{

			elderNodeCount--;

			if (tempPriorSibling)
				tempPriorSibling->nextSibling = tempNextSibling;

			else
				firstElder = tempNextSibling;

			if (tempNextSibling)
				tempNextSibling->priorSibling = tempPriorSibling;

			else
				lastElder = tempPriorSibling;

		}

		//stitch to branchOriginNode into new parent and siblings
		if (!newParentNode)
		{

			elderNodeCount++;

			if (!newAdjacentSiblingNode)
			{

				if (insertAfter)
				{

					if (lastElder)
						lastElder->nextSibling = branchOriginNode;

					else
						firstElder = branchOriginNode;

					branchOriginNode->priorSibling = lastElder;
					branchOriginNode->nextSibling = NULL;
					
					branchOriginNode->parentNode = NULL;

					lastElder = branchOriginNode;

					return branchOriginNode->priorSibling;

				}

				else
				{

					if (firstElder)
						firstElder->priorSibling = branchOriginNode;

					else
						lastElder = branchOriginNode;

					branchOriginNode->priorSibling = NULL;
					branchOriginNode->nextSibling = firstElder;
					
					branchOriginNode->parentNode = NULL;

					firstElder = branchOriginNode;

					return branchOriginNode->nextSibling;

				}

			}

			else
			{

				if (insertAfter)
				{

					tempNextSibling = newAdjacentSiblingNode->nextSibling;

					branchOriginNode->priorSibling = newAdjacentSiblingNode;
					branchOriginNode->nextSibling = tempNextSibling;

					branchOriginNode->parentNode = NULL;

					if (tempNextSibling)
						tempNextSibling->priorSibling = branchOriginNode;

					else
						lastElder = branchOriginNode;

					newAdjacentSiblingNode->nextSibling = branchOriginNode;

					return tempNextSibling;

				}

				else
				{

					tempPriorSibling = newAdjacentSiblingNode->priorSibling;

					branchOriginNode->priorSibling = tempPriorSibling;
					branchOriginNode->nextSibling = newAdjacentSiblingNode;

					branchOriginNode->parentNode = NULL;

					if (tempPriorSibling)
						tempPriorSibling->nextSibling = branchOriginNode;

					else
						firstElder = branchOriginNode;

					newAdjacentSiblingNode->priorSibling = branchOriginNode;

					return tempPriorSibling;

				}

			}

		}

		else
		{

			newParentNode->totalChildren++;

			if (!newAdjacentSiblingNode)
			{

				if (insertAfter)
				{

					//tempNextSibling is reused here as newParentNode's lastChild
					tempNextSibling = newParentNode->lastChild;

					if (tempNextSibling)
						tempNextSibling->nextSibling = branchOriginNode;

					else
						newParentNode->firstChild = branchOriginNode;

					branchOriginNode->priorSibling = tempNextSibling;
					branchOriginNode->nextSibling = NULL;

					branchOriginNode->parentNode = newParentNode;

					newParentNode->lastChild = branchOriginNode;

					return tempNextSibling;

				}

				else
				{

					//tempPriorSibling is reused here as newParentNode's firstChild
					tempPriorSibling = newParentNode->firstChild;

					if (tempPriorSibling)
						tempPriorSibling->priorSibling = branchOriginNode;

					else
						newParentNode->lastChild = branchOriginNode;

					branchOriginNode->priorSibling = NULL;
					branchOriginNode->nextSibling = tempPriorSibling;

					branchOriginNode->parentNode = newParentNode;

					newParentNode->firstChild = branchOriginNode;

					return tempPriorSibling;

				}

			}

			else
			{

				if (insertAfter)
				{

					tempNextSibling = newAdjacentSiblingNode->nextSibling;

					branchOriginNode->priorSibling = newAdjacentSiblingNode;
					branchOriginNode->nextSibling = tempNextSibling;

					branchOriginNode->parentNode = newParentNode;

					if (tempNextSibling)
						tempNextSibling->priorSibling = branchOriginNode;

					else
						newParentNode->lastChild = branchOriginNode;

					newAdjacentSiblingNode->nextSibling = branchOriginNode;

					return tempNextSibling;

				}

				else
				{

					tempPriorSibling = newAdjacentSiblingNode->priorSibling;

					branchOriginNode->priorSibling = tempPriorSibling;
					branchOriginNode->nextSibling = newAdjacentSiblingNode;

					branchOriginNode->parentNode = newParentNode;

					if (tempPriorSibling)
						tempPriorSibling->nextSibling = branchOriginNode;

					else
						newParentNode->firstChild = branchOriginNode;

					newAdjacentSiblingNode->priorSibling = branchOriginNode;

					return tempPriorSibling;

				}

			}

		}

	}

	/*

	//testing
	void outputStructure_ToConsole()
	{

		BranchingNode<T> *currentNode = firstElder;
		BranchingNode<T> *currentParentNode = NULL;
		unsigned short levelCount = 0;
		unsigned short count;

		cout << "\n\nBranchingList structure report:";
		cout << "firstElder = " << firstElder;
		cout << ", lastElder = " << lastElder;
		cout << ", elderNodeCount = " << elderNodeCount;
		cout << ", totalNodeCount = " << totalNodeCount;

		do
		{

			//there are no more siblings at this level
			if (!currentNode)
			{

				//the proceedure is not at the top level, move up one level
				//and laterally by one node
				if (currentParentNode)
				{

					levelCount--;

					currentNode = currentParentNode->nextSibling;
					currentParentNode = currentParentNode->parentNode;

				}

				//the proceedure is concluded
				else
					return;

			}

			//this sibling exists and has children, dive down one level
			else if (currentNode->firstChild)
			{

				//output information about currentNode
				cout << "\n\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "node (this = " << currentNode;
				cout << ", parent = " << currentNode->parentNode;
				cout << ", priorSibling = " << currentNode->priorSibling;
				cout << ", nextSibling = " << currentNode->nextSibling << ",\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "totalChildren = " << currentNode->totalChildren;
				cout << ", firstChild = " << currentNode->firstChild;
				cout << ", lastChild = " << currentNode->lastChild << ")";

				levelCount++;

				currentParentNode = currentNode;
				currentNode = currentNode->firstChild;

			}

			//this sibling exists but has no children,
			//move laterally by one node
			else
			{

				//output information about currentNode
				cout << "\n\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "node (this = " << currentNode;
				cout << ", parent = " << currentNode->parentNode;
				cout << ", priorSibling = " << currentNode->priorSibling;
				cout << ", nextSibling = " << currentNode->nextSibling << ",\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "totalChildren = " << currentNode->totalChildren;
				cout << ", firstChild = " << currentNode->firstChild;
				cout << ", lastChild = " << currentNode->lastChild << ")";

				currentNode = currentNode->nextSibling;

			}

		} while (true);

	}

	*/

	//functions to find a node based on it's location in list order, etc

};

//next create a branching list indexed class in which all nodes contain two additional pointers, one to their levelPriorNode and one to their levelNextNode
//as well as an additional level index unsigned long long (level index is necessary to know which level list to add node(s) to when creating/moving)
//the pointers will compose linked lists of the nodes at any given level of the branching structure, crossing over boundaries of inheritance
//the branching list object itself will contain 2 additional lists: levelFirstNodes and levelLastNodes pointers

#endif // MEMORYTOOLS_H_INCLUDED