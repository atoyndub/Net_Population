#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#include "Cell.h"

class Net
{

private:
	
	//main cell storage array
	ExtensibleArray<Cell> cells;

	//performance/fitness tracking property
	double fitnessRating; //lower value means better performance

public:

	//default constructor
	Net() :cells(3)
	{

		*(cells.get_ElementAddress(0)) = Cell(0, 1, 3);
		*(cells.get_ElementAddress(1)) = Cell(1, 1, 3);
		*(cells.get_ElementAddress(2)) = Cell(2, 1, 3);

	}

	//modified constructor; only safe in context of Population initialization
	Net(UniformList<Cell> &cellsList, ExtensibleArray<unsigned short> &priorLinksCountsList,
		const unsigned short aInputCellCount, const unsigned short aCellsPerNet)
		: cells(aCellsPerNet)
	{

		UniformNode<Cell> *cellNode(cellsList.get_FirstNode());
		unsigned short count = 0;

		//copy the contents of populatedCellsList into cells
		//and set each cell's priorLinksCount
		do
		{

			*(cells.get_ElementAddress(count)) = cellNode->content;

			if (count < aInputCellCount)
				cells.get_ElementAddress(count)->set_PriorLinksCount(
					priorLinksCountsList.get_Element(count) + 1);

			else
				cells.get_ElementAddress(count)->set_PriorLinksCount(
					priorLinksCountsList.get_Element(count));

			cellNode = cellNode->get_NextNode();
			count++;

		} while (count < aCellsPerNet);

	}

	//default destructor
	~Net() {}

	//assumes rand() has been seeded, assumes mutationCount > 0; mutationAmplitude > 0.0
	void mutate(unsigned short mutationCount, const double mutationAmplitude)
	{

		//initial setup
		const unsigned short totalCellCount(cells.get_ArrayLength());
		unsigned short mutationCellIndex;
		Cell *mutationCell;
		unsigned short mutationType;

		//main loop to effect multiple mutations
		do 
		{

			//randomly select a mutation cell and type
			mutationCellIndex = rand() % totalCellCount;
			mutationCell = cells.get_ElementAddress(mutationCellIndex);
			mutationType = rand() % 8;

			//mutate structure of postLinks by adding, deleting, or revising a link
			if (mutationType < 3)
				mutationCell->mutate_PostLinks(totalCellCount, cells, mutationType);

			//mutate a linkCoeff value
			else if (mutationType == 3)
				mutationCell->mutate_RandomLinkCoeff(mutationAmplitude);

			//mutate a internalCoeff value
			else if (mutationType == 4)
				mutationCell->mutate_InternalCoeff(mutationAmplitude);

			//mutate a broadcastCoeff value
			else if (mutationType == 5)
				mutationCell->mutate_BroadcastCoeff(mutationAmplitude);

			//mutate a decayRate value
			else if (mutationType == 6)
				mutationCell->mutate_DecayRate(mutationAmplitude);

			//mutate a refractoryPeriod value
			else //mutationType == 7
				mutationCell->mutate_RefractoryPeriod(mutationAmplitude);

			mutationCount--;

		} while (mutationCount);

	}

	//splices the contents of motherCells and fatherCells into this net
	//overwriting the existing cells; assumes motherCells and fatherCells are from
	//nets in the same population and are not NULL; assumes motherCells
	//and fatherCells are not this net's cells
	void meiosis(ExtensibleArray<Cell> &motherCells, ExtensibleArray<Cell> &fatherCells,
		const unsigned short maxSpliceLength)
	{

		unsigned short currentSpliceLength;
		const unsigned short totalCellCount(cells.get_ArrayLength());
		unsigned short cellsCopied = 0;
		unsigned short count = cellsCopied;

		do
		{
			
			//set currentSpliceLength
			currentSpliceLength = (rand() % maxSpliceLength) + 1;

			if (currentSpliceLength > (totalCellCount - cellsCopied))
				currentSpliceLength = (totalCellCount - cellsCopied);

			//copy currentSpliceLength cells in from currentParent
			cellsCopied += currentSpliceLength;

			//choose the parent and splice in cells
			if (rand() % 2)
			{

				do
				{

					*(cells.get_ElementAddress(count)) = *(motherCells.get_ElementAddress(count));
					count++;

				} while (count < cellsCopied);

			}

			else
			{

				do
				{

					*(cells.get_ElementAddress(count)) = *(fatherCells.get_ElementAddress(count));
					count++;

				} while (count < cellsCopied);

			}

		} while (cellsCopied < totalCellCount);

	}

	//getter
	double get_FitnessRating() const
	{

		return fitnessRating;

	}

	//adds fitnessRatingModifier to fitnessRating
	void modify_FitnessRating(const double fitnessRatingModifier)
	{

		fitnessRating += fitnessRatingModifier;

	}

	//setter
	void reset_FitnessRating()
	{

		fitnessRating = 0.0;

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		unsigned short count;
		const unsigned short totalCellCount(cells.get_ArrayLength());

		outputFile << "\n\t\t\t<groupLength>1</groupLength>";

		//cells
		outputFile << "\n\t\t\t<cells>";
		for (count = 0; count < totalCellCount; count++)
		{

			outputFile << "\n\t\t\t\t<c_" << count << ">";
			cells.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t\t\t</c_" << count << ">";

		}
		outputFile << "\n\t\t\t</cells>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		unsigned short count;
		const unsigned short length(cells.get_ArrayLength() - 1);

		//cells
		outputFile << "\"cells\":[";
		for (count = 0; count < length; count++)
		{

			outputFile << "{";
			cells.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

		}
		outputFile << "{";
		cells.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
		outputFile << "}]";

	}

	//returns reference to cells for use by Population
	ExtensibleArray<Cell> &get_Cells()
	{

		return cells;

	}

};

#endif // NET_H_INCLUDED