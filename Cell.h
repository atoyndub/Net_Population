#ifndef CELL_H_INCLUDED
#define CELL_H_INCLUDED

#include "Link.h"

class Cell
{

private:

	//stored copies of net numerical properties
	unsigned short thisCellIndex; //index of this cell in net context

	//fixed mutation control properties
	double linksCoeffCenter; //starting value of all this cell's postLinks linkCoeffs (> 0.0)
	double internalSpread; //controls rate of internalCoeff spread by mutation (> 0.0, value of 1.0 means an unamplified mutation changes internalCoeff by up to +-0.5)
	double broadcastSpread; //controls rate of broadcastCoeff spread by mutation (> 0.0, value of 1.0 means an unamplified mutation changes broadcastCoeff by up to +-0.5)
	double linksCoeffSpread; //controls rate of links coefficients spreads (> 0.0, value of 1.0 means an unamplified mutation changes a linksCoeff by up to +-0.5)
	double decayRateSpread; //controls rate of decayRate spread (> 0.0, value of 1.0 means an unamplified mutation changes a decayRate by up to +-0.05)
	unsigned char refractoryPeriodSpread; //sets the maximum refractoryPeriod change per mutation (>= 1) 
	double broadcastSwitchSignFreq; //controls frequency at which broadcastCoeff switches signs (between 0 *never switches* and 1 *always switches*)
	double linksSwitchSignFreq; //controls frequency at which linkCoeffs switch signs (between 0 *never switches* and 1 *always switches*)

	//mutateble stimulus control properties
	double internalCoeff; //modifies the total stimulus required to reach action potential (>= 0.0)
	double broadcastCoeff; //modifies amplification level and sign of this cell's post-action potential signal to other cells (any value)
	double decayRate; //modifies speed of stimulus loss per stimulate() level if the cell does not reach action potential (>= 0.0 *total loss*, <= 1 *no loss*)
	unsigned char refractoryPeriod; //the number of stimulate() levels following activation when stimulus is reset to zero and this cell can be reactivated (> 0)	
	UniformList<Link> postLinks; //list of links (cells and linkCoeffs) corresponding this cell's output connections (assumed to always be populated)

	//updateable stimulus calculation properties
	unsigned short priorLinksCount; //the tracked number of cells which send this cell stimulus
	double inputDiffusalCoeff; //diffuses stimulus received by the cell based on number of prior links

	//updateable stimulus tracking properties
	double stimulus; //stores the unmodified stimulus this cell receives (positive, negative, or 0.0)
	unsigned short lastRoundStimulusChanged; //stores the count (starting from 1) of the stimulus round in which this cell last was stimulated or stimulus was decayed (0 if never stimulated)
	unsigned short roundRefractionComplete; //stores the count (starting from 1) of the stimulus round in which this cell will have passed it's refractory period after activation (0 if never activated)
	unsigned short activationCount; //tracks number of activations per stimulate() call

	//***private sub-mutation methods***

	//finds the nearest available postCell value, either before or after
	//startingAndReturnLinkNode, which is not currently in this cell's
	//postLinks list; assumes postLinks length is < totalCellCount - 1
	//sets value of startingAndReturnIndex to the index of the nearest
	//available post cell index and sets startingAndReturnLinkNode to the
	//post cell node just prior to where new post link will be inserted
	//(NULL if at list start)
	inline void sub_MutatePostLinks_FindNearestAvailablePostCellIndex(const unsigned short totalCellCount,
		unsigned short &startingAndReturnIndex, UniformNode<Link> *&startingAndReturnLinkNode)
	{

		//initial setup
		unsigned short forwardIterationIndex(startingAndReturnIndex);
		UniformNode<Link> *forwardLinkNode(startingAndReturnLinkNode);
		unsigned short backwardIterationIndex(startingAndReturnIndex);
		UniformNode<Link> *backwardLinkNode(startingAndReturnLinkNode);

		//main loop
		do
		{

			//single forward iteration
			forwardLinkNode = forwardLinkNode->get_NextNode();
			{

				if (!forwardLinkNode)
				{

					//terminal states with no available postCell indices
					if (forwardIterationIndex == (totalCellCount - 1)
						|| (forwardIterationIndex == (totalCellCount - 2) && thisCellIndex == (totalCellCount - 1)))
					{

						//iterate backwards to completion
						do
						{

							backwardLinkNode = backwardLinkNode->get_PriorNode();
							backwardIterationIndex--;

							if (!backwardLinkNode)
							{

								if (backwardIterationIndex == thisCellIndex)
									backwardIterationIndex--;

								startingAndReturnLinkNode = NULL;
								startingAndReturnIndex = backwardIterationIndex;

								return;

							}

							else
							{

								if (backwardIterationIndex != backwardLinkNode->content.postCellIndex)
								{

									if (backwardIterationIndex == thisCellIndex)
									{

										backwardIterationIndex--;

										if (backwardIterationIndex != backwardLinkNode->content.postCellIndex)
										{

											startingAndReturnLinkNode = backwardLinkNode;
											startingAndReturnIndex = backwardIterationIndex;

											return;

										}

									}

									else
									{

										startingAndReturnLinkNode = backwardLinkNode;
										startingAndReturnIndex = backwardIterationIndex;

										return;

									}

								}

							}

						} while (true);

					}

					else
					{

						forwardIterationIndex++;
						
						if (forwardIterationIndex == thisCellIndex)
							forwardIterationIndex++;

						startingAndReturnLinkNode = postLinks.get_LastNode();
						startingAndReturnIndex = forwardIterationIndex;

						return;

					}

				}

				else
				{

					forwardIterationIndex++;

					if (forwardIterationIndex != forwardLinkNode->content.postCellIndex)
					{

						if (forwardIterationIndex == thisCellIndex)
						{

							forwardIterationIndex++;

							if (forwardIterationIndex != forwardLinkNode->content.postCellIndex)
							{

								startingAndReturnLinkNode = forwardLinkNode->get_PriorNode();
								startingAndReturnIndex = forwardIterationIndex;

								return;

							}

						}

						else
						{

							startingAndReturnLinkNode = forwardLinkNode->get_PriorNode();
							startingAndReturnIndex = forwardIterationIndex;

							return;

						}

					}

				}

			}

			//single backward iteration
			backwardLinkNode = backwardLinkNode->get_PriorNode();
			{

				if (!backwardLinkNode)
				{

					//terminal states with no available postCell indices
					if (backwardIterationIndex == 0
						|| backwardIterationIndex == 1 && thisCellIndex == 0)
					{

						//iterate forward to completion
						do
						{

							forwardLinkNode = forwardLinkNode->get_NextNode();
							forwardIterationIndex++;

							if (!forwardLinkNode)
							{

								if (forwardIterationIndex == thisCellIndex)
									forwardIterationIndex++;

								startingAndReturnLinkNode = postLinks.get_LastNode();
								startingAndReturnIndex = forwardIterationIndex;

								return;

							}

							else
							{

								if (forwardIterationIndex != forwardLinkNode->content.postCellIndex)
								{

									if (forwardIterationIndex == thisCellIndex)
									{

										forwardIterationIndex++;

										if (forwardIterationIndex != forwardLinkNode->content.postCellIndex)
										{

											startingAndReturnLinkNode = forwardLinkNode->get_PriorNode();
											startingAndReturnIndex = forwardIterationIndex;

											return;

										}

									}

									else
									{

										startingAndReturnLinkNode = forwardLinkNode->get_PriorNode();
										startingAndReturnIndex = forwardIterationIndex;

										return;

									}

								}

							}

						} while (true);

					}

					else
					{

						backwardIterationIndex--;

						if (backwardIterationIndex == thisCellIndex)
							backwardIterationIndex--;

						startingAndReturnLinkNode = NULL;
						startingAndReturnIndex = backwardIterationIndex;

						return;

					}

				}

				else
				{

					backwardIterationIndex--;

					if (backwardIterationIndex != backwardLinkNode->content.postCellIndex)
					{

						if (backwardIterationIndex == thisCellIndex)
						{

							backwardIterationIndex--;

							if (backwardIterationIndex != backwardLinkNode->content.postCellIndex)
							{

								startingAndReturnLinkNode = backwardLinkNode;
								startingAndReturnIndex = backwardIterationIndex;

								return;

							}

						}

						else
						{

							startingAndReturnLinkNode = backwardLinkNode;
							startingAndReturnIndex = backwardIterationIndex;

							return;

						}

					}

				}

			}

		} while (true);

	}

	//assumes postLinks listLength > 0 and < totalCellCount - 1, and rand() is seeded
	inline void sub_MutatePostLinks_AddRandomPostLink(const unsigned short totalCellCount,
		ExtensibleArray<Cell> &allNetCells)
	{

		//initial setup
		unsigned short randomCellIndex;
		UniformNode<Link> *linkNode(postLinks.get_FirstNode());

		//select a random cell index to add
		do
		{

			randomCellIndex = rand() % totalCellCount;

		} while (randomCellIndex == thisCellIndex);

		//if randomCellIndex is not already present in postLinks,
		//add it in at the appropriate location
		do
		{

			if (randomCellIndex < linkNode->content.postCellIndex)
			{

				postLinks.add_NewNode(Link(randomCellIndex, linksCoeffCenter), linkNode, false);

				allNetCells.get_ElementAddress(randomCellIndex)->priorLinksCount++;
				allNetCells.get_ElementAddress(randomCellIndex)->update_InputDiffusalCoeff();

				return;

			}

			if (randomCellIndex == linkNode->content.postCellIndex)
				break;

			else //randomCellIndex > linkNode->content.postCell
			{

				linkNode = linkNode->get_NextNode();

				if (!linkNode)
				{

					postLinks.add_NewNode(Link(randomCellIndex, linksCoeffCenter));

					allNetCells.get_ElementAddress(randomCellIndex)->priorLinksCount++;
					allNetCells.get_ElementAddress(randomCellIndex)->update_InputDiffusalCoeff();

					return;

				}

			}

		} while (true);

		//postLinks already contains a linkCorresponding to randomCellIndex
		//so set randomCellIndex to the nearest postCell index value
		//not currently in postLinks and set linkNode to the node before the
		//desired insertion location (NULL if at the start of postLinks)
		sub_MutatePostLinks_FindNearestAvailablePostCellIndex(totalCellCount,
			randomCellIndex, linkNode);

		if (linkNode)
			postLinks.add_NewNode(Link(randomCellIndex, linksCoeffCenter), linkNode);

		else
			postLinks.add_NewNode(Link(randomCellIndex, linksCoeffCenter), NULL, false);

		allNetCells.get_ElementAddress(randomCellIndex)->priorLinksCount++;
		allNetCells.get_ElementAddress(randomCellIndex)->update_InputDiffusalCoeff();

	}

	//removes a random postLinks member;
	//assumes postLinksLength > 1, assumes rand() is seeded
	inline void sub_MutatePostLinks_RemoveRandomPostLink(
		ExtensibleArray<Cell> &allNetCells, const unsigned short postLinksLength)
	{

		UniformNode<Link> *randomLinkNode = postLinks.get_NodeAddress(rand() % postLinksLength);

		allNetCells.get_ElementAddress(randomLinkNode->content.postCellIndex)->priorLinksCount--;
		allNetCells.get_ElementAddress(randomLinkNode->content.postCellIndex)->update_InputDiffusalCoeff();
		postLinks.delete_Node(randomLinkNode);

	}

	//replaces the postCell value of an existing link with a new,
	//available value and relocates the link in list order
	//assumes postLinksLength > 0 and < totalCellCount -1; rand() is seeded
	inline void sub_MutatePostLinks_ReplaceRandomPostLink(const unsigned short totalCellCount,
		ExtensibleArray<Cell> &allNetCells, const unsigned short postLinksLength)
	{

		//initial setup
		unsigned short randomIndex;
		UniformNode<Link> *linkNode(postLinks.get_FirstNode());
		UniformNode<Link> *nodeToReplace(postLinks.get_NodeAddress(
			rand() % postLinksLength));
		unsigned short currentIndex(linkNode->content.postCellIndex);

		//select a random cell index to add
		do
		{

			randomIndex = rand() % totalCellCount;

		} while (randomIndex == thisCellIndex);

		//main loop
		do
		{

			//randomCellIndex is not already represented in postLinks;
			//change nodeToReplace postCellIndex to randomIndex and
			//relocate it to correct position in list order (if not already there)
			if (randomIndex < currentIndex)
			{

				allNetCells.get_ElementAddress(nodeToReplace->content.postCellIndex)->priorLinksCount--;
				allNetCells.get_ElementAddress(nodeToReplace->content.postCellIndex)->update_InputDiffusalCoeff();

				nodeToReplace->content.postCellIndex = randomIndex;

				allNetCells.get_ElementAddress(randomIndex)->priorLinksCount++;
				allNetCells.get_ElementAddress(randomIndex)->update_InputDiffusalCoeff();

				//move nodeToReplace into correct position if not already there
				if (nodeToReplace != linkNode && nodeToReplace != linkNode->get_PriorNode())
					postLinks.move_Node(nodeToReplace, linkNode, false);

				return;

			}

			//postLinks already contains a link corresponding to randomIndex
			else if (randomIndex == currentIndex)
				break;

			//randomIndex > linkNode->content.postCell, iterate to next link
			else
			{

				linkNode = linkNode->get_NextNode();

				//randomIndex is not already represented in postLinks;
				//change nodeToReplace postCellIndex to randomIndex and
				//relocate it to last position in list order (if not already there)
				if (!linkNode)
				{

					allNetCells.get_ElementAddress(nodeToReplace->content.postCellIndex)->priorLinksCount--;
					allNetCells.get_ElementAddress(nodeToReplace->content.postCellIndex)->update_InputDiffusalCoeff();

					nodeToReplace->content.postCellIndex = randomIndex;

					allNetCells.get_ElementAddress(randomIndex)->priorLinksCount++;
					allNetCells.get_ElementAddress(randomIndex)->update_InputDiffusalCoeff();

					//move nodeToReplace into correct position if not already there
					if (nodeToReplace != postLinks.get_LastNode())
						postLinks.move_Node(nodeToReplace);

					return;

				}

				else
					currentIndex = linkNode->content.postCellIndex;

			}

		} while (true);

		//set randomCellIndex to the nearest postCell index value
		//not currently in postLinks and set linkNode to the node before the
		//desired insertion location (NULL if at the start of postLinks)
		sub_MutatePostLinks_FindNearestAvailablePostCellIndex(totalCellCount, randomIndex, linkNode);

		allNetCells.get_ElementAddress(nodeToReplace->content.postCellIndex)->priorLinksCount--;
		allNetCells.get_ElementAddress(nodeToReplace->content.postCellIndex)->update_InputDiffusalCoeff();

		nodeToReplace->content.postCellIndex = randomIndex;

		allNetCells.get_ElementAddress(randomIndex)->priorLinksCount++;
		allNetCells.get_ElementAddress(randomIndex)->update_InputDiffusalCoeff();

		if (linkNode)
		{

			if (nodeToReplace != linkNode && nodeToReplace != linkNode->get_NextNode())
				postLinks.move_Node(nodeToReplace, linkNode);

		}

		else
		{

			if (nodeToReplace != postLinks.get_FirstNode())
				postLinks.move_Node(nodeToReplace, NULL, false);

		}

	}

	//used to update inputDiffusalCoeff when priorLinksCount changes
	//this calculation is based on a concept that at the junction between two cells
	//(axon terminal), the amount of the post cell's surfance area the prior cell is able to monopolize
	//modifies the relative influence of its stimulus; this also includes an assumption that
	//stimulus is passed uninhibited with up to 3 simultaneous axonal connections, but beyond that
	//stimulus is inhibited proportional to the relative loss of surface area from the 3 connection-level
	inline void update_InputDiffusalCoeff()
	{

		if (priorLinksCount > 3)
			inputDiffusalCoeff = 3.0 / static_cast<double>(priorLinksCount);

		else
			inputDiffusalCoeff = 1.0;

	}

	//called when this cell has been activated
	//sets stimulus to zero, increments activationCount, and sets
	//roundRefractionComplete to currentStimulusRound + refractoryPeriod
	inline void activate(const unsigned short currentStimulusRound)
	{

		stimulus = 0.0;
		activationCount++;
		roundRefractionComplete = currentStimulusRound + refractoryPeriod;

	}

	//modifies the value of this cell's stimulus and lastRoundStimulusChanged
	//accounting for any stimulus decay based on decayRate and the number of stimulus
	//rounds passed since lastRoundStimulusChanged (if any); assumes
	//currentStimulusRound >= lastRoundStimulusChanged
	inline void decay_Stimulus(const unsigned short currentStimulusRound)
	{

		while (lastRoundStimulusChanged < currentStimulusRound)
		{

			stimulus *= decayRate;
			lastRoundStimulusChanged++;

		}

	}

public:

	//default constructor - unsafe should not be used in any context
	Cell() {}

	//modified constructor - used in net default construction
	//constructs each cell in a net to all have a single postLink
	//with next next cell in array order except for the cell in last
	//position which links to the first cell in array position;
	//also has the effect of gives the input cells an artificially larger priorLinksCount
	//to represent the fact they have a connection with external input and therefore will
	//have less room to grow cell body size to receive connections from other cells before
	//diffusion will diminish all signal received
	//assumes thisCellIndex is this Cell's array index in net context
	//totalCellCount >= 3, inputCellCount > 0, outputCellCount > 0, and
	//totalCellCount >= inputCellCount + outputCellCount (to ensure member function safety)
	Cell(const unsigned short aThisCellIndex, const unsigned short inputCellCount,
		const unsigned short totalCellCount) : thisCellIndex(aThisCellIndex),
		linksCoeffCenter(1.0), internalSpread(1.0), broadcastSpread(1.0),
		linksCoeffSpread(1.0), decayRateSpread(1.0), refractoryPeriodSpread(2),
		broadcastSwitchSignFreq(0.01), linksSwitchSignFreq(0.01), internalCoeff(1.0),
		broadcastCoeff(1.0), decayRate(0.5), refractoryPeriod(2), inputDiffusalCoeff(1.0)
	{
		
		if (thisCellIndex < (totalCellCount - 1))
			postLinks.add_NewNode(Link((thisCellIndex + 1), 1.0));

		else
			postLinks.add_NewNode(Link(0, 1.0));

		if (thisCellIndex < inputCellCount)
			priorLinksCount = 2;

		else
			priorLinksCount = 1;
	
	}

	//modified constructor;
	//constructs a cell assuming all parameters have been validated in Population
	//initialization; assumes that this cell's priorLinksCount and inputDiffusalCoeff
	//will be set at a later point in the initialization via set_PriorLinksCount();
	//assumes aCellControlIndex < the Cell Control array length in net context
	//assumes all parameters are in valide ranges
	//has the effect of moving links stored in postLinksList into this cell;
	Cell(const unsigned short aThisCellIndex, const double aLinksCoeffCenter,
		const double aInternalSpread, const double aBroadcastSpread, const double aLinksCoeffSpread,
		const double aDecayRateSpread, const double aRefractoryPeriodSpread, const double aBroadcastSwitchSignFreq,
		const double aLinksSwitchSignFreq, double aInternalCoeff, const double aBroadcastCoeff,
		const double aDecayRate, const unsigned char aRefractoryPeriod, UniformList<Link> &tempPostLinksList)
		: thisCellIndex(aThisCellIndex), linksCoeffCenter(aLinksCoeffCenter), internalSpread(aInternalSpread),
		broadcastSpread(aBroadcastSpread), linksCoeffSpread(aLinksCoeffSpread), decayRateSpread(aDecayRateSpread),
		refractoryPeriodSpread(aRefractoryPeriodSpread), broadcastSwitchSignFreq(aBroadcastSwitchSignFreq),
		linksSwitchSignFreq(aLinksSwitchSignFreq), internalCoeff(aInternalCoeff), broadcastCoeff(aBroadcastCoeff),
		decayRate(aDecayRate), refractoryPeriod(aRefractoryPeriod)
	{

		postLinks.add_CutWholeList(tempPostLinksList);

	}

	//default destructor
	~Cell() {}

	//mutation functions

	//modifies internalCoeff (ensures it will remain >= 0.0)
	void mutate_InternalCoeff(const double mutationAmplitude)
	{

		double tempDouble = ((static_cast<double>(rand() % 10000) / 10000.0) - 0.5)
			* internalSpread * mutationAmplitude;

		if (internalCoeff + tempDouble >= 0.0)
			internalCoeff += tempDouble;

		else
			internalCoeff -= tempDouble;

	}

	//modifies broadcastCoeff (resulting value is any double)
	//assumes rand() is seeded
	void mutate_BroadcastCoeff(const double mutationAmplitude)
	{

		double tempDouble = (static_cast<double>(rand() % 10000) / 10000.0);

		if (tempDouble < broadcastSwitchSignFreq)
			broadcastCoeff = (-1) * (broadcastCoeff + ((tempDouble - 0.5)
				* broadcastSpread * mutationAmplitude));

		else
			broadcastCoeff += ((tempDouble - 0.5) * broadcastSpread
				* mutationAmplitude);

	}

	//randomly either adds, deletes, or replaces a postLink member;
	//ensures that postLinks remains populated with at least 1 member and
	//never contains more than totalCellCount - 1 members
	//assumes rand() has been seeded; totalCellCount >= 3
	//assumes thisCellIndex is this Cell's array index in net context
	//assumes mutationType is 0 (add), 1 (replace), or 2 (remove)
	void mutate_PostLinks(const unsigned short totalCellCount, ExtensibleArray<Cell> &allNetCells,
		const unsigned short mutationType)
	{

		unsigned short postLinksLength = postLinks.get_ListLength();

		//postLinks is at min length, add or replace link
		if (postLinksLength == 1)
		{

			if(rand() % 2)
				sub_MutatePostLinks_AddRandomPostLink(totalCellCount,
					allNetCells);

			else
				sub_MutatePostLinks_ReplaceRandomPostLink(totalCellCount,
					allNetCells, postLinksLength);

		}

		//postLinks is at max length, remove link
		else if (postLinksLength == totalCellCount - 1)
			sub_MutatePostLinks_RemoveRandomPostLink(allNetCells,
				postLinksLength);

		//postLinks is neither at min or max length,
		//add, replace, or remove link
		else
		{

			if (!mutationType)
				sub_MutatePostLinks_AddRandomPostLink(totalCellCount, allNetCells);

			else if (mutationType == 1)
				sub_MutatePostLinks_ReplaceRandomPostLink(totalCellCount, allNetCells, postLinksLength);

			else //mutationType == 2
				sub_MutatePostLinks_RemoveRandomPostLink(allNetCells, postLinksLength);

		}

	}

	//modifies a random linkCoeff (resulting value is any double)
	//assumes that postLinks is populated; assumes rand() has been seeded;
	void mutate_RandomLinkCoeff(const double mutationAmplitude)
	{

		UniformNode<Link> *randomLinkNode = postLinks.get_NodeAddress(rand()
			% postLinks.get_ListLength());

		double tempDouble = (static_cast<double>(rand() % 10000) / 10000.0);

		if (tempDouble < linksSwitchSignFreq)
			randomLinkNode->content.linkCoeff = (-1) * (randomLinkNode->content.linkCoeff + ((tempDouble - 0.5)
				* linksCoeffSpread * mutationAmplitude));

		else
			randomLinkNode->content.linkCoeff += ((tempDouble - 0.5) * broadcastSpread
				* mutationAmplitude);

	}

	//modifies a decayRate ensuring it remains >= 0.0 and <= 1.0
	// assumes rand() has been seeded
	void mutate_DecayRate(const double mutationAmplitude)
	{

		decayRate += ((static_cast<double>(rand() % 10000) / 10000.0) - 0.5)
			* 0.1 * decayRateSpread * mutationAmplitude;

		if (decayRate >= 0.0)
		{

			if (decayRate <= 1.0)
				return;

			else
				decayRate = 1.0;

		}

		else
			decayRate = 0.0;

	}

	//modifies a refractoryPeriod ensuring it is >= 1; assumes rand() has been seeded;
	void mutate_RefractoryPeriod(const double mutationAmplitude)
	{

		if (rand() % 2)
			refractoryPeriod += static_cast<unsigned char>((rand() % refractoryPeriodSpread)
			* static_cast<unsigned char>(mutationAmplitude) + 1);

		else
			refractoryPeriod -= static_cast<unsigned char>((rand() % refractoryPeriodSpread)
			* static_cast<unsigned char>(mutationAmplitude) + 1);

		if (refractoryPeriod > 0)
			return;

		else
			refractoryPeriod = 1;

	}

	//***getter, setter, and output functions***

	//getter
	double get_InternalCoeff() const
	{

		return internalCoeff;

	}

	//getter
	double get_BroadcastCoeff() const
	{

		return broadcastCoeff;

	}

	//getter
	unsigned short get_PostLinksLength() const
	{

		return postLinks.get_ListLength();

	}

	//getter
	double get_DecayRate() const
	{

		return decayRate;

	}

	//getter
	unsigned char get_RefractoryPeriod() const
	{

		return refractoryPeriod;

	}

	//double get_InputDiffusalCoeff() const
	//{

		//return inputDiffusalCoeff;

	//}

	//setter, only for use in context of Population controlled construction
	void set_PriorLinksCount(unsigned short aPriorLinksCount)
	{

		priorLinksCount = aPriorLinksCount;
		update_InputDiffusalCoeff();

	}

	//***stimulus control methods***

	//resets all tracking variables to null state
	void reset()
	{

		stimulus = 0.0;
		lastRoundStimulusChanged = 0;
		roundRefractionComplete = 0;
		activationCount = 0;

	}

	//getter
	unsigned short get_RoundRefractionComplete() const
	{

		return roundRefractionComplete;

	}

	//getter
	unsigned short get_ActivationCount() const
	{

		return activationCount;

	}

	//calculates the value of this tracker's stimulus (without any actual modification)
	//accounting for any stimulus decay based on decayRate and the number of stimulus
	//rounds passed since lastRoundStimulusChanged (if any); assumes decayRate
	//is that of the cell in index position corresponding to the index position
	//of this tracker and currentStimulusRound >= lastRoundStimulusChanged
	double calculate_DecayedStimulus(const double decayRate,
		const unsigned short currentStimulusRound) const
	{

		unsigned short roundCount(lastRoundStimulusChanged);
		double returnValue(stimulus);

		while (roundCount < currentStimulusRound)
		{

			returnValue *= decayRate;
			roundCount++;

		}

		return returnValue;

	}

	//***sub population::stimulate() functions***

	//called for net input cells only in first stimulus round
	void addStimulus_FirstRound(const double externalStimulus)
	{

		stimulus = externalStimulus * inputDiffusalCoeff;
		lastRoundStimulusChanged = 1;

	}

	//called for each input cell in the first stimulus round; returns true
	//and updates activation tracking variables the cell has received
	//sufficient stimulus from external input to reach action potential;
	//returns false otherwise; assumes cellToTest != NULL,
	bool testActivation_FirstRound()
	{

		if (stimulus * internalCoeff >= 1.0)
		{

			activate(1);
			return true;

		}

		else
			return false;

	}

	//passes modified stimulus from this activated preCell to all it's postCells;
	//assumes this cell is activated, being contained in the most recent activation
	//round list
	void addStimulus_LaterRounds(ExtensibleArray<Cell> &allNetCells, const unsigned short currentStimulusRound)
	{

		//initial setup
		UniformNode<Link> *postLinkNode(postLinks.get_FirstNode());
		unsigned short postCellIndex;
		Cell *postCell;

		do
		{

			postCellIndex = postLinkNode->content.get_PostCellIndex();
			postCell = allNetCells.get_ElementAddress(postCellIndex);

			//the post cell has (a) never been activated, (b) is currently
			//in its refractory period, or (c) has passed a refractory period
			//and never been stimulated since
			if (!postCell->stimulus)
			{

				//the post cell is not in it's refractory period
				if (postCell->roundRefractionComplete <= currentStimulusRound)
				{

					//set the current stimulus in the postCell to the
					//stimulus from this preCell modified by it's broadcastCoeff
					//the inputDiffusalCoeff of the post cell and
					//the linkCoeff of the associated postlink node
					postCell->stimulus += broadcastCoeff * postCell->inputDiffusalCoeff
						* postLinkNode->content.get_LinkCoeff();

					//set the tracker's lastRoundStimulusChanged to the current round
					postCell->lastRoundStimulusChanged = currentStimulusRound;

				}

				//else, the post cell is in it's refractory period

			}

			//the post cell is not in a refractory period and has stored stimulus
			else
			{

				//the stored stimulus needs to be decayed before other updating
				if (postCell->lastRoundStimulusChanged < currentStimulusRound)
				{

					//decay postCell's stimulus by multiplying stimulus by
					//the postCell's decayRate in successive iterations equal to
					//the number of rounds since last stimulus change while also
					//updating lastRoundStimulusChanged to currentStimulusRound
					postCell->decay_Stimulus(currentStimulusRound);

					//modify current stimulus in the postCell by adding
					//in the stimulus from this preCell modified by it's broadcastCoeff
					//the inputDiffusalCoeff of the post cell and
					//the linkCoeff of the associated postlink node
					postCell->stimulus += broadcastCoeff * postCell->inputDiffusalCoeff
						* postLinkNode->content.get_LinkCoeff();

				}

				//the stored stimulus is up to date
				else
				{

					//modify current stimulus in the postCell by adding
					//in the stimulus from this preCell modified by it's broadcastCoeff
					//the inputDiffusalCoeff of the post cell and
					//the linkCoeff of the associated postlink node
					postCell->stimulus += broadcastCoeff * postCell->inputDiffusalCoeff
						* postLinkNode->content.get_LinkCoeff();

					//set postCell's lastRoundStimulusChanged to the current round
					postCell->lastRoundStimulusChanged = currentStimulusRound;

				}

			}

			postLinkNode = postLinkNode->get_NextNode();

		} while (postLinkNode);

	}

	//called for each postCell of each activated cell in a given stimulus round;
	//returns true and updates activation tracking variables if this cell
	//is not in its refractory period and has received sufficient stimulus to
	//reach aciton potential; returns false if this cell is in its refractory
	//period (including if it was just activated by this function earlier
	//this stimulus round as a member of another preCell's postCells list), or
	//if it has not received sufficient stimulus to become activated;
	bool testActivation_LaterRounds(const unsigned short currentStimulusRound)
	{

		if (stimulus * internalCoeff >= 1.0)
		{

			activate(currentStimulusRound);
			return true;

		}

		else
			return false;

	}

	//used in population::stimulate() to add any of this cell's postCells
	//which reached action potential, to nextRoundActivationList if not already there;
	//assumes this cell has been activated in the current
	//round of population::stimulate(); assumes activationListPositionIndex correspondes to
	//the element index of nextRoundActivationList which has not been
	//(re)initialized as of yet in the current stimulus round
	void populateNextActivationList_FromPostCells(
		ExtensibleArray<Cell> &allNetCells, const unsigned short currentStimulusRound,
		ExtensibleArray<int> *nextRoundActivationList, unsigned short &activationListPositionIndex)
	{

		//initial setup
		UniformNode<Link> *postLinkNode(postLinks.get_FirstNode());
		unsigned short postCellIndex;

		//loop to iterate through each post cell and test activation
		do
		{

			postCellIndex = postLinkNode->content.get_PostCellIndex();

			if (allNetCells.get_ElementAddress(postCellIndex)->testActivation_LaterRounds(currentStimulusRound))
			{

				//add the postCell index to nextRoundActivationList and iterate nextRoundAvailableIndex
				//(this function need not test if the postCell index is already in nextRound
				//because a postCell shared by multiple preCells in a given stimulus round
				//cannot be activated twice due to the requirement that refractoryPeriod > 0
				*(nextRoundActivationList->get_ElementAddress(activationListPositionIndex)) = postCellIndex;
				activationListPositionIndex++;

			}

			postLinkNode = postLinkNode->get_NextNode();

		} while (postLinkNode);

	}

	//***reporting methods***

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		unsigned short count(0);
		UniformNode<Link> *linkNode(postLinks.get_FirstNode());

		//tracking variables

		outputFile << "\n\t\t\t\t\t<internalCoeff>";
		outputDouble_ToOpenFile(internalCoeff, outputFile, 15);
		outputFile << "</internalCoeff>";

		outputFile << "\n\t\t\t\t\t<broadcastCoeff>";
		outputDouble_ToOpenFile(broadcastCoeff, outputFile, 15);
		outputFile << "</broadcastCoeff>";

		outputFile << "\n\t\t\t\t\t<decayRate>";
		outputDouble_ToOpenFile(decayRate, outputFile, 15);
		outputFile << "</decayRate>";

		outputFile << "\n\t\t\t\t\t<refractoryPeriod>" << static_cast<unsigned short>(refractoryPeriod) << "</refractoryPeriod>";

		//postLinks
		outputFile << "\n\t\t\t\t\t<postLinks>";
		do
		{

			outputFile << "\n\t\t\t\t\t\t<pl_" << count << ">";
			linkNode->content.output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t\t\t\t\t</pl_" << count << ">";

			count++;
			linkNode = linkNode->get_NextNode();

		} while (linkNode);
		outputFile << "\n\t\t\t\t\t</postLinks>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		unsigned short count(0);
		UniformNode<Link> *linkNode(postLinks.get_FirstNode());
		UniformNode<Link> *lastNode(postLinks.get_LastNode());

		//tracking variables

		outputFile << "\"internalCoeff\":";
		outputDouble_ToOpenFile(internalCoeff, outputFile, 15);
		outputFile << ",";

		outputFile << "\"broadcastCoeff\":";
		outputDouble_ToOpenFile(broadcastCoeff, outputFile, 15);
		outputFile << ",";

		outputFile << "\"decayRate\":";
		outputDouble_ToOpenFile(decayRate, outputFile, 15);
		outputFile << ",";

		outputFile << "\"refractoryPeriod\":" << static_cast<unsigned short>(refractoryPeriod) << ",";

		//postLinks
		outputFile << "\"postLinks\":[";
		while (linkNode != lastNode)
		{

			outputFile << "{";
			linkNode->content.output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

			count++;
			linkNode = linkNode->get_NextNode();

		}
		outputFile << "{";
		linkNode->content.output_ToOpenFile_JSON(outputFile);
		outputFile << "}]";

	}

};

#endif // CELL_H_INCLUDED