#ifndef EVOLUTIONCONTROL_H_INCLUDED
#define EVOLUTIONCONTROL_H_INCLUDED

#include "MemoryTools.h"

class EvolutionControl
{

private:

	unsigned short maxPopulationSize; //the maximum number of nets which the population can support in this round of evolution (>= 2)
	double survivalThreshold; //the maximum fittnessRating a net may possess in order to survive and reproduce in this round of evolution
	unsigned short totalRepetitions; //the number of times the population will repeat use of this evolution control before iterating onto the next (>= 1)

	ExtensibleArray<unsigned long long> dataSetRowIndices; //indicies of the data set row(s) which will be read before nets reproduce in this round of evolution

public:

	//default constructor - these values are very arbitrary
	EvolutionControl() : maxPopulationSize(2), survivalThreshold(1000000),
		totalRepetitions(1), dataSetRowIndices(1)
	{
	
		*(dataSetRowIndices.get_ElementAddress(0)) = 0;
	
	}

	//modified constructor - assumes all parameters have been validated in population context
	EvolutionControl(const unsigned short aMaxPopulationSize, const double aSurvivalThreshold,
		const unsigned short aTotalRepetitions, UniformList<unsigned long long> & aDataSetRowIndicies)
	{

		maxPopulationSize = aMaxPopulationSize;
		survivalThreshold = aSurvivalThreshold;
		totalRepetitions = aTotalRepetitions;
		dataSetRowIndices = aDataSetRowIndicies;

	}

	//destructor
	~EvolutionControl() {}

	//getter
	unsigned short get_MaxPopulationSize() const
	{

		return maxPopulationSize;

	}

	//getter
	double get_SurvivalThreshold() const
	{

		return survivalThreshold;

	}

	//getter
	unsigned short get_TotalRepetitions() const
	{

		return totalRepetitions;

	}

	//getter
	unsigned long long get_DataSetRowIndicesLength() const
	{

		return dataSetRowIndices.get_ArrayLength();

	}

	//getter
	unsigned long long get_DataSetRowIndicesElement(
		const unsigned long long dataSetRowIndex) const
	{

		return dataSetRowIndices.get_Element(dataSetRowIndex);

	}

};

#endif // EVOLUTIONCONTROL_H_INCLUDED
