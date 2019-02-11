#ifndef LINK_H_INCLUDED
#define LINK_H_INCLUDED

#include "MemoryTools.h"

class Link
{

private:

	//index of the cell receiving input through this link
	unsigned short postCellIndex;

	//stimulus modifying coefficient specific to this link (positive or negative)
	double linkCoeff;

public:

	//default constructor - safe in this class conext only
	Link() : postCellIndex(0), linkCoeff(1.0) {}

	//modified constructor - assumes aPostCell != the containing
	//cell's index
	Link(unsigned short aPostCell, double aLinkCoeff)
	{

		postCellIndex = aPostCell;
		linkCoeff = aLinkCoeff;

	}

	//default destructor
	~Link() {}

	//getter
	unsigned short get_PostCellIndex() const
	{

		return postCellIndex;

	}

	//getter
	double get_LinkCoeff() const
	{

		return linkCoeff;

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		outputFile << "\n\t\t\t\t\t\t\t<postCellIndex>" << postCellIndex << "</postCellIndex>";

		outputFile << "\n\t\t\t\t\t\t\t<linkCoeff>";
		outputDouble_ToOpenFile(linkCoeff, outputFile, 15);
		outputFile << "</linkCoeff>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		outputFile << "\"postCellIndex\":" << postCellIndex << ",";

		outputFile << "\"linkCoeff\":";
		outputDouble_ToOpenFile(linkCoeff, outputFile, 15);

	}

	friend class Cell;

};

#endif // LINK_H_INCLUDED
