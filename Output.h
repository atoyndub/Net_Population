#ifndef OUTPUT_H_INCLUDED
#define OUTPUT_H_INCLUDED

#include "MemoryTools.h"
#include "StringTools.h"

class Output
{

private:

	//functional value
	double value;

	//identifier
	Word name;

	//the number of activations against which the corresponding output cell's
	//activation count will be calculated when determining the output value
	//for each double in this array (must be >= 1)
	unsigned short maxActivations;

public:

	//default constructor; generally safe null state
	Output() : value(0), name("unidentified_output"), maxActivations(1) {}

	//modified constructor, assumes aName is populated, and aMaxActivations >= 1
	Output(Word &aName, const unsigned short aMaxActivations)
		: value(0.0), name(aName), maxActivations(aMaxActivations) {}

	//default destructor
	~Output() {}

	//setter
	void set_Value(const double newValue)
	{

		value = newValue;

	}

	//getter
	double get_Value() const
	{

		return value;

	}

	//getter, used in population initialization
	double *get_ValueAddress()
	{

		return &value;

	}

	//getter, used in population initialization,
	//assumes name wont be modified
	Word &get_Name()
	{

		return name;

	}

	//getter, used in stimulate() function
	unsigned short get_MaxActivations() const
	{

		return maxActivations;

	}

	//assumes that outputFile is already open
	void outputName_ToOpenFile(ofstream &outputFile)
	{

		name.output_ToOpenFile(outputFile);

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		outputFile << "\n\t\t\t<name>";
		name.output_ToOpenFile(outputFile);
		outputFile << "</name>";
		
		outputFile << "\n\t\t\t<maxActivations>" << maxActivations << "</maxActivations>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		outputFile << "\"name\":\"";
		name.output_ToOpenFile(outputFile);
		outputFile << "\",";

		outputFile << "\"maxActivations\":" << maxActivations;

	}

};

#endif // OUTPUT_H_INCLUDED