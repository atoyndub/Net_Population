#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "MemoryTools.h"
#include "CellControl.h"
#include "Input.h"
#include "Output.h"
#include "MathTools.h"
#include "Net.h"
#include "EvolutionControl.h"
#include "StringTools.h"
#include "StimulusTracker.h"
#include "RecordingModule.h"

class FixedInputReference
{

private:

	Variable_Double *dataPointVariable;
	unsigned short dataPointFrameSubIndex;
	unsigned short dataPointColumnIndex;

public:

	//default constructor
	FixedInputReference() : dataPointVariable(NULL),
		dataPointFrameSubIndex(0), dataPointColumnIndex(0) {}

	//modified constructor
	FixedInputReference(Variable_Double *aDataPointVariable,
		const unsigned short aDataPointFrameSubIndex, const unsigned short aDataPointColumnIndex)
		: dataPointVariable(aDataPointVariable), dataPointFrameSubIndex(aDataPointFrameSubIndex),
		dataPointColumnIndex(aDataPointColumnIndex) {}

	//default destructor
	~FixedInputReference() {}

	//getter
	Variable_Double *get_DataPointVariable()
	{

		return dataPointVariable;

	}

	//getter
	unsigned short get_DataPointFrameSubIndex() const
	{

		return dataPointFrameSubIndex;

	}

	//getter
	unsigned short get_DataPointColumnIndex() const
	{

		return dataPointColumnIndex;

	}

};

/*

	***copied from input.h for reference here***

	the following are objects which store and provide access to fixed data
	used to stimulate nets; this data does not change at runtime

	design terminology:

	DataPoint (conceptual only) : a single double value within more complex
	fixed data objects

	DataSet : a one-dimensional array of data points which will be read by
	a discrete group of net input cells; data points within a DataSet
	are read either all simultaneously (the number of assigned net
	input cells == the DataSet array size), or in succession
	with a smaller viewing frame which traverses over the length of the
	DataSet array over time (the number of assigned net input cells ==
	the viewing frame size)

	DataSetRow : an array of DataSet objects which will be read simultaneously
	by a number of discreet groups of net input cells; net performance is
	evaluated after each DataSetRow is read (not each data frame) but
	those in performance evaluations are added cumulatively oiver the course
	of an evolutionary cycle as each new DataSetRow is read;
	contains a name identifier which should conceptually represent all the
	contained DataSets; for example, if DataSets contained within a row
	represent temperature, humidity, and windspeed data, DataSetRows might
	represent those data on different months, or in different cities and
	should be named accordingly

	DataSetColumn : an object representing a grouping of DataSets which are all
	in the same array index position of all DataSetRows; these DataSets
	all are read by the same discrete group of net input cells but in series,
	row by row over the course of a single evolutionary cycle; since these
	DataSets are actually stored across each DataSetRow
	object, the DataSetColumn simply stores a name (e.g. temperature,
	humidity, windspeed, etc.) corresponding to the type of data represented
	in all DataSets 'contained' within the column, as well as a frameLength
	and a shiftLength corresponding to the viewing frame controls associated
	with each 'contained' DataSet

	DataSetCollection : the highest level object containing and array of DataSetRows
	and an array of DataSetColumns. The number of DataSets in each DataSetRow
	== the number of contained DataSetColumns; likewise, all DataSets 'contained'
	in each column will contain the same number of DataPoints which is consistent
	with the DataSetColumn's frameLength, shiftLength, and the population's totalDataFrameCount

	rowIndex - a local variable term used to track the DataSetRow index being referred to
	columnIndex - a local variable term used to track the DataSetColumn index being referred to

*/

//the default constructor will construct a functional Population, but
//one which serves no actual purpose; the initialize() function should be
//used to validate/load a functional and working Population after construction

class Population
{

private:

	//general numerical properties
	unsigned short totalNetCount; //total number of nets per population, assumed to be >= 2
	unsigned short inputCellCount; //total number of cells which receive input from either a data set or a calculated input, assumed to be >= 1
	unsigned short outputCellCount; //number of decision cells per net, assumed to be >= 1
	//unsigned short totalCellCount; //total number of cells per net, assumed to be >= (inputCellCount + outputCellCount) and >= 3
	unsigned short maxRoundsPerStimulate; //sets a cap on number of rounds in stimulate(), ( >= 1 )
	
	//data set input numerical properties 
	unsigned short dataSetColumnCount; //number of Input_DataSet_Series objects in fixedInputs (horozontal axis), assumed >= 1
	unsigned long long dataSetRowCount; //number of data sets within each Input_DataSet_Series object in fixedInputs (vertical axis), assumed >= 1
	unsigned long long totalDataFrameCount; //the number of data frames shared by all data sets in fixedInputs, assumed >= 1

	//calculated input numerical property
	unsigned short calculatedInputsCount; //number of Input_Calculated members in calculatedInputs

	//input/output properties
	DataSetCollection fixedInputs; //used to store/access loaded input data
	ExtensibleArray<Input_Calculated> calculatedInputs; //array of of length calculatedInputsCount, used to store/access calculated input values
	ExtensibleArray<Output> outputs; //array of output objects to manage cell output

	//update expression properties
	UniformList<Variable_Double> variablesList; //stores Variable_Double objects refering to doubles stored in fixedInputs, calculatedInputs, and outputs (for use in expressions)
	UniformList<FixedInputReference> updatingFixedInputsVariables; //refers to only Variable_Double objects referring to fixedInputs data points which are used in expressions
	ExtensibleArray<Expression_Double> calculatedInputUpdateExpressions; //array of expressions of size calculatedInputsCount used to update each respective calculatedInput after each data frame
	ExtensibleArray<Expression_Double> evaluationCriteriaExpressions; //array of expressions of size dataSetRowCount to add to net fitnessRating after data set row iteration in evolve()

	//reproduction/evolution control properties
	unsigned char reproductionType; //0 means asexual, 1 sexual, 2 sexual selected
	unsigned char mutationsPerNet; //number of mutations each net will accumlate after each cycle of reproduction in evolve()
	double mutationAmplitude; //amplitude of each net mutation
	ExtensibleArray<EvolutionControl> evolutionControls; //array of controls to guide each cycle of reproduction in evolve()

	//structural/functional properties
	//ExtensibleArray<CellControl> cellControls; //array of cellControls (size == totalCellCount)					   
	//ExtensibleArray<StimulusTracker> trackers; //cell-associated stimulus trackers (of size totalCellCount)
	UniformList<Net> nets; //list of size totalNetCount

	//recording properties
	RecordedDataSetRowSequence recordingData;

	//***private methods***//




	//***WORK FROM HERE TO CONTINUE IMPLEMENTING CHANGES, MAYBE BREAK UP SOME PORTIONS OF THE SUB_STIMULATE FUNCTIONS
	//AND REDISTRIBUTE THEM TO NET CLASS, ALSO GO BACK TO CELL AND REMOVE ANY NOW UNUSED GETTER/SETTER FUNCTIONS***




	//netCells parameter is all the cells contained in a single net,
	//assumes dataSetParellelIndex is < dataSetCount
	//assumes netCells contains more than inputCellCount + outputCellCount
	//assumes dataFrameIndex < totalDataFrameCount
	inline void stimulate_SingleFrame(const unsigned long long dataSetRowIndex,
		const unsigned long long dataFrameIndex, ExtensibleArray<Cell> &netCells,
		ExtensibleArray<int> &activationListRoundA, ExtensibleArray<int> &activationListRoundB)
	{

		//reset all cell tracking properties
		unsigned short cellIndex;
		const unsigned short totalCellCount(netCells.get_ArrayLength());

		for (cellIndex = 0; cellIndex < totalCellCount; cellIndex++)
			netCells.get_ElementAddress(cellIndex)->reset();

		//other initial setup
		unsigned short currentRoundActivationListPositionIndex(0);

		unsigned short dataSetColumnIndex(0);
		unsigned short dataFrameSubIndex;
		unsigned short dataSetFrameLength;

		Input_Calculated *calculatedInput;
		Output *output;
		Cell *cell;

		cellIndex = 0;

		//read fixed input data and activate corresponding input cells if warranted
		do
		{

			dataSetFrameLength = fixedInputs.get_ColumnFrameLength(dataSetColumnIndex);
			dataFrameSubIndex = 0;
			
			do
			{

				cell = netCells.get_ElementAddress(cellIndex);

				//pass external input to the input cell
				cell->addStimulus_FirstRound(fixedInputs.get_FrameAdjustedDataPoint(
					dataSetRowIndex, dataSetColumnIndex, dataFrameIndex, dataFrameSubIndex));

				//activate the input cell and add to currentRound if warranted
				if (cell->testActivation_FirstRound())
				{

					*(activationListRoundA.get_ElementAddress(currentRoundActivationListPositionIndex)) = cellIndex;
					currentRoundActivationListPositionIndex++;

				}

				cellIndex++;
				dataFrameSubIndex++;

			} while (dataFrameSubIndex < dataSetFrameLength);

			dataSetColumnIndex++;

		} while (dataSetColumnIndex < dataSetColumnCount);

		//read any calculated inputs and activate corresponding input cells if warranted
		//(dataSetColumnIndex reused here but means calculatedInputIndex)
		dataSetColumnIndex = 0;

		while (dataSetColumnIndex < calculatedInputsCount)
		{

			calculatedInput = calculatedInputs.get_ElementAddress(dataSetColumnIndex);

			cell = netCells.get_ElementAddress(cellIndex);
			tracker = trackers.get_ElementAddress(cellIndex);

			//pass external input to the input cell
			sub_Stimulate_AddStimulus_FirstRound(cell, tracker,
				calculatedInput->get_CurrentValue());

			//activate the input cell and add to currentRound if warranted
			if (sub_Stimulate_TestActivation_FirstRound(cell, tracker))
			{

				*(activationListRoundA.get_ElementAddress(currentRoundActivationListPositionIndex)) = cellIndex;
				currentRoundActivationListPositionIndex++;

			}

			cellIndex++;
			dataSetColumnIndex++;

		}
		
		//set activationListRoundA end marker value
		*(activationListRoundA.get_ElementAddress(currentRoundActivationListPositionIndex)) = (-1);

		//additional setup
		ExtensibleArray<int> *currentRound;
		ExtensibleArray<int> *nextRound;
		unsigned short nextRoundActivationListPositionIndex;

		unsigned short roundCount = 2;
		int preCellIndex;

		//main signal cascade loop
		do
		{

			//set the activation list tracking variables
			currentRound = &activationListRoundA;
			nextRound = &activationListRoundB;
			currentRoundActivationListPositionIndex = 0;

			preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

			if (preCellIndex != (-1))
			{

				//pass stimulus on to all postCells represented in currentRound
				do
				{

					sub_Stimulate_AddStimulus_LaterRounds(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//activate postCells and add to nextRound if warranted	
				currentRoundActivationListPositionIndex = 0;
				preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);
				nextRoundActivationListPositionIndex = 0;

				do
				{

					sub_Stimulate_PopulateNextActivationList_FromPostCells(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount, nextRound, nextRoundActivationListPositionIndex);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//set nextRound marker value
				*(nextRound->get_ElementAddress(nextRoundActivationListPositionIndex)) = (-1);

				roundCount++;

			}

			//no cells have been activated after the most recent iteration
			else
				break;

			//flip the activation list tracking variables
			currentRound = &activationListRoundB;
			nextRound = &activationListRoundA;
			currentRoundActivationListPositionIndex = 0;

			preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

			if (preCellIndex != (-1))
			{

				//pass stimulus on to all postCells represented in currentRound
				do
				{

					sub_Stimulate_AddStimulus_LaterRounds(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//activate postCells and add to nextRound if warranted	
				currentRoundActivationListPositionIndex = 0;
				preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);
				nextRoundActivationListPositionIndex = 0;

				do
				{

					sub_Stimulate_PopulateNextActivationList_FromPostCells(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount, nextRound, nextRoundActivationListPositionIndex);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//set nextRound marker value
				*(nextRound->get_ElementAddress(nextRoundActivationListPositionIndex)) = (-1);

				roundCount++;

			}

			//no cells have been activated after the most recent iteration
			else
				break;

		} while (roundCount < maxRoundsPerStimulate);

		//update stored values in outputs (output cells are those immediately
		//following inputCells) and reset all stimulus tracking variables
		//(dataSetColumnIndex reused here but means outputIndex)
		dataSetColumnIndex = 0;
		double testValue;

		do
		{

			output = outputs.get_ElementAddress(dataSetColumnIndex);

			//update the output element's stored value to the
			//ratio of activationCount to the corresponding output's maxActivations
			//(or to 1.0 if activationCount > maxActivations 
			testValue = (static_cast<double>(trackers.get_ElementAddress(cellIndex)->get_ActivationCount())
				/ static_cast<double>(output->get_MaxActivations()));

			if (testValue <= 1.0)
				output->set_Value(testValue);

			else
				output->set_Value(1.0);

			cellIndex++;
			dataSetColumnIndex++;

		} while (dataSetColumnIndex < outputCellCount);

	}

	//netCells parameter is all the cells contained in a single net,
	//assumes dataSetParellelIndex is < dataSetCount
	//assumes netCells contains more than inputCellCount + outputCellCount
	//assumes dataFrameIndex < totalDataFrameCount
	//assumes outputFile is already open
	//assumes currentRow is not NULL and corresponds to the node in recordingData
	//which is currently being populated 
	inline void stimulate_SingleFrame_Recorded(const unsigned long long dataSetRowIndex,
		const unsigned long long dataFrameIndex, ExtensibleArray<Cell> &netCells,
		ExtensibleArray<int> &activationListRoundA, ExtensibleArray<int> &activationListRoundB,
		UniformNode<RecordedDataSetRow> *currentRow)
	{

		//reset trackers
		unsigned short cellIndex;

		for (cellIndex = 0; cellIndex < totalCellCount; cellIndex++)
			trackers.get_ElementAddress(cellIndex)->reset();

		//other initial setup
		unsigned short currentRoundActivationListPositionIndex(0);

		unsigned short dataSetColumnIndex(0);
		unsigned short dataFrameSubIndex;
		unsigned short dataSetFrameLength;

		Input_Calculated *calculatedInput;
		Output *output;
		Cell *cell;
		StimulusTracker *tracker;

		cellIndex = 0;

		//read fixed input data and activate corresponding input cells if warranted
		do
		{

			dataSetFrameLength = fixedInputs.get_ColumnFrameLength(dataSetColumnIndex);
			dataFrameSubIndex = 0;

			do
			{

				cell = netCells.get_ElementAddress(cellIndex);
				tracker = trackers.get_ElementAddress(cellIndex);

				//pass external input to the input cell
				sub_Stimulate_AddStimulus_FirstRound(cell, tracker, fixedInputs.get_FrameAdjustedDataPoint(
					dataSetRowIndex, dataSetColumnIndex, dataFrameIndex, dataFrameSubIndex));

				//activate the input cell and add to currentRound if warranted
				if (sub_Stimulate_TestActivation_FirstRound(cell, tracker))
				{

					*(activationListRoundA.get_ElementAddress(currentRoundActivationListPositionIndex)) = cellIndex;
					currentRoundActivationListPositionIndex++;

				}

				cellIndex++;
				dataFrameSubIndex++;

			} while (dataFrameSubIndex < dataSetFrameLength);

			dataSetColumnIndex++;

		} while (dataSetColumnIndex < dataSetColumnCount);

		//read any calculated inputs and activate corresponding input cells if warranted
		//(dataSetColumnIndex reused here but means calculatedInputIndex)
		dataSetColumnIndex = 0;

		while (dataSetColumnIndex < calculatedInputsCount)
		{

			calculatedInput = calculatedInputs.get_ElementAddress(dataSetColumnIndex);

			cell = netCells.get_ElementAddress(cellIndex);
			tracker = trackers.get_ElementAddress(cellIndex);

			//pass external input to the input cell
			sub_Stimulate_AddStimulus_FirstRound(cell, tracker,
				calculatedInput->get_CurrentValue());

			//activate the input cell and add to currentRound if warranted
			if (sub_Stimulate_TestActivation_FirstRound(cell, tracker))
			{

				*(activationListRoundA.get_ElementAddress(currentRoundActivationListPositionIndex)) = cellIndex;
				currentRoundActivationListPositionIndex++;

			}

			cellIndex++;
			dataSetColumnIndex++;

		}

		//set activationListRoundA end marker value
		*(activationListRoundA.get_ElementAddress(currentRoundActivationListPositionIndex)) = (-1);

		//additional setup
		ExtensibleArray<int> *currentRound;
		ExtensibleArray<int> *nextRound;
		unsigned short nextRoundActivationListPositionIndex;

		unsigned short roundCount = 2;
		int preCellIndex;

		//main signal cascade loop
		do
		{

			//set the activation list tracking variables
			currentRound = &activationListRoundA;
			nextRound = &activationListRoundB;
			currentRoundActivationListPositionIndex = 0;

			preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

			if (preCellIndex != (-1))
			{

				//***RECORDING***
				recordingData.add_StimulusRound(currentRow, trackers, dataFrameIndex);
				//***RECORDING***

				//pass stimulus on to all postCells represented in currentRound
				do
				{

					sub_Stimulate_AddStimulus_LaterRounds(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//activate postCells and add to nextRound if warranted	
				currentRoundActivationListPositionIndex = 0;
				preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);
				nextRoundActivationListPositionIndex = 0;

				do
				{

					sub_Stimulate_PopulateNextActivationList_FromPostCells(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount, nextRound, nextRoundActivationListPositionIndex);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//set nextRound marker value
				*(nextRound->get_ElementAddress(nextRoundActivationListPositionIndex)) = (-1);

				roundCount++;

			}

			//no cells have been activated after the most recent iteration
			else
				break;

			//flip the activation list tracking variables
			currentRound = &activationListRoundB;
			nextRound = &activationListRoundA;
			currentRoundActivationListPositionIndex = 0;

			preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

			if (preCellIndex != (-1))
			{

				//***RECORDING***
				recordingData.add_StimulusRound(currentRow, trackers, dataFrameIndex);
				//***RECORDING***

				//pass stimulus on to all postCells represented in currentRound
				do
				{

					sub_Stimulate_AddStimulus_LaterRounds(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//activate postCells and add to nextRound if warranted	
				currentRoundActivationListPositionIndex = 0;
				preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);
				nextRoundActivationListPositionIndex = 0;

				do
				{

					sub_Stimulate_PopulateNextActivationList_FromPostCells(netCells.get_ElementAddress(
						preCellIndex), netCells, roundCount, nextRound, nextRoundActivationListPositionIndex);

					currentRoundActivationListPositionIndex++;
					preCellIndex = currentRound->get_Element(currentRoundActivationListPositionIndex);

				} while (preCellIndex != (-1));

				//set nextRound marker value
				*(nextRound->get_ElementAddress(nextRoundActivationListPositionIndex)) = (-1);

				roundCount++;

			}

			//no cells have been activated after the most recent iteration
			else
				break;

		} while (roundCount < maxRoundsPerStimulate);

		//update stored values in outputs (output cells are those immediately
		//following inputCells) and reset all stimulus tracking variables
		//(dataSetColumnIndex reused here but means outputIndex)
		dataSetColumnIndex = 0;
		double testValue;

		do
		{

			output = outputs.get_ElementAddress(dataSetColumnIndex);

			//update the output element's stored value to the
			//ratio of activationCount to the corresponding output's maxActivations
			//(or to 1.0 if activationCount > maxActivations 
			testValue = (static_cast<double>(trackers.get_ElementAddress(cellIndex)->get_ActivationCount())
				/ static_cast<double>(output->get_MaxActivations()));

			if (testValue <= 1.0)
				output->set_Value(testValue);

			else
				output->set_Value(1.0);

			cellIndex++;
			dataSetColumnIndex++;

		} while (dataSetColumnIndex < outputCellCount);

	}

	//iterates through all data frames in all data set rows referred to by currentEvolutionControl
	//one time; updates nets' performance ratings in the process, but does not sort or reproduce them
	//assumes currentEvolutionControl != NULL
	inline void stimulate_AllFrames_SingleEvolutionCycle(ExtensibleArray<int> &activationListRoundA,
		ExtensibleArray<int> &activationListRoundB, ExtensibleArray<double> &calculatedInputsPrepValues)
		//EvolutionControl *currentEvolutionControl)
	{

		//initial setup
		UniformNode<Net> *netNode(nets.get_FirstNode());
		unsigned long long dataSetRowIndex;
		//unsigned long long evolutionControlInternalIndex;
		//unsigned long long evolutionControlInternalLength(currentEvolutionControl->get_DataSetRowIndicesLength());
		unsigned long long dataFrameIndex;
		unsigned short count;
		UniformNode<FixedInputReference> *fixedInputsVariableNode;

		//net loop
		do
		{

			//reset the net's fitness rating
			netNode->content.reset_FitnessRating();

			dataSetRowIndex = 0;
			//evolutionControlInternalIndex = 0;

			//data set row loop 
			do
			{

				//set the correct data set row
				//dataSetRowIndex = currentEvolutionControl->get_DataSetRowIndicesElement(
					//evolutionControlInternalIndex);

				//reset calculated inputs to default values
				for (count = 0; count < calculatedInputsCount; count++)
					calculatedInputs.get_ElementAddress(count)->reset();

				dataFrameIndex = 0;

				//data frame loop
				do
				{

					//stimulate the net
					stimulate_SingleFrame(dataSetRowIndex, dataFrameIndex, netNode->content.get_Cells(),
						activationListRoundA, activationListRoundB);

					//iterate the data frame
					dataFrameIndex++;

					//reassign any fixedInputs dataPoint variables used in
					//calculatedInputUpdateExpressions to prep for the next dataFrame
					fixedInputsVariableNode = updatingFixedInputsVariables.get_FirstNode();

					while (fixedInputsVariableNode)
					{

						fixedInputsVariableNode->content.get_DataPointVariable()->reassign_Value(
							fixedInputs.get_FrameAdjustedDataPointAddress(dataSetRowIndex,
								fixedInputsVariableNode->content.get_DataPointColumnIndex(),
								dataFrameIndex, fixedInputsVariableNode->content.get_DataPointFrameSubIndex()));

						fixedInputsVariableNode = fixedInputsVariableNode->get_NextNode();

					}

					//update calculatedInputs values via calculatedInputUpdateExpressions
					//(done here in two steps so that calculatedInputs members can effectively
					//be updated in parallel, not in series to ensure that changes occuring within
					//each member round do not impact calculations of members in later array position
					//in context of the current dataFrame iteration)
					for (count = 0; count < calculatedInputsCount; count++)
						*(calculatedInputsPrepValues.get_ElementAddress(count)) =
						calculatedInputUpdateExpressions.get_ElementAddress(count)->evaluate();

					for (count = 0; count < calculatedInputsCount; count++)
						calculatedInputs.get_ElementAddress(count)->set_CurrentValue(
							calculatedInputsPrepValues.get_Element(count));

				} while (dataFrameIndex < totalDataFrameCount);

				//update the net's fitness rating
				netNode->content.modify_FitnessRating(evaluationCriteriaExpressions.get_ElementAddress(
					dataSetRowIndex)->evaluate());

				dataSetRowIndex++;
				//evolutionControlInternalIndex++;

			} while (dataSetRowIndex < dataSetRowCount);
			//} while (evolutionControlInternalIndex < evolutionControlInternalLength);

			netNode = netNode->get_NextNode();

		} while (netNode);

	}



	//*** WORK FROM HERE TO CHANGE THE LOGIC OF THE FUNCTIONS BELOW TO MATCH THAT ABOVE
	//ALSO RECONSIDER/REFACTOR THE RECORDING FUNCTIONS TO BE MORE MANAGEABLE AND SO THEY
	//WILL FOLLOW THE NEW PARADIGM FOR RECORDING (USING SUB FUNCTIONS TO MAKE COMPOUNDING CHANGES MORE MANAGEABLE)***

/*

	//uses all data sets/frames to stimulate only the top performing net; updates
	//the net's performance rating in the process, but does not sort; outputs in CSV format
	//assumes outputFile is already open
	inline void stimulate_AllFramesAllParallels_CSVMinimalRecord(ExtensibleArray<int> &activationListRoundA,
		ExtensibleArray<int> &activationListRoundB, ExtensibleArray<double> &calculatedInputsPrepValues,
		ofstream &outputFile)
	{

		//initial setup
		UniformNode<Net> *netNode(nets.get_FirstNode());
		unsigned long long dataSetRowIndex;
		unsigned long long dataFrameIndex;
		unsigned short count;
		UniformNode<FixedInputReference> *fixedInputsVariableNode;

		//***<RECORDING>***
		unsigned short recordingCalculatedInputIndex;
		unsigned short recordingOutputIndex;
		ExtensibleArray<unsigned short> dataSetColumnFrameIndices(dataSetColumnCount);
		unsigned short maxShiftLengthMinusOne(1);
		unsigned short maxFrameLength(1);
		unsigned short recordingSubFrameIndex;
		
		for (count = 0; count < dataSetColumnCount; count++)
		{

			if (fixedInputs.get_ColumnShiftLength(count) > maxShiftLengthMinusOne)
				maxShiftLengthMinusOne = fixedInputs.get_ColumnShiftLength(count);

			if (fixedInputs.get_ColumnFrameLength(count) > maxFrameLength)
				maxFrameLength = fixedInputs.get_ColumnFrameLength(count);

		}
		
		maxShiftLengthMinusOne--;
		//***</RECORDING>***

		//reset the net's fitness rating
		netNode->content.reset_FitnessRating();

		dataSetRowIndex = 0;

		//data set row loop
		do
		{

			//reset calculated inputs to default values
			for (count = 0; count < calculatedInputsCount; count++)
				calculatedInputs.get_ElementAddress(count)->reset();

			dataFrameIndex = 0;

			//***<RECORDING>***
			outputFile << "\n\n";
			fixedInputs.get_RowName(dataSetRowIndex).output_ToOpenFile(outputFile);

			for (count = 0; count < dataSetColumnCount; count++)
			{

				outputFile << ",";
				fixedInputs.get_ColumnName(count).output_ToOpenFile(outputFile);

			}

			for (count = 0; count < calculatedInputsCount; count++)
			{

				outputFile << ",";
				calculatedInputs.get_ElementAddress(count)->get_Name().output_ToOpenFile(outputFile);

			}

			for (count = 0; count < outputCellCount; count++)
			{

				outputFile << ",";
				outputs.get_ElementAddress(count)->get_Name().output_ToOpenFile(outputFile);

			}

			//leading fixed set data
			for (count = 0; count < dataSetColumnCount; count++)
				*(dataSetColumnFrameIndices.get_ElementAddress(count)) = 0;

			outputFile << "\n";

			for (recordingSubFrameIndex = 0; recordingSubFrameIndex < maxFrameLength - (maxShiftLengthMinusOne + 1); recordingSubFrameIndex++)
			{

				for (count = 0; count < dataSetColumnCount; count++)
				{

					outputFile << ",";

					if (dataSetColumnFrameIndices.get_Element(count) < fixedInputs.get_ColumnFrameLength(count))
					{

						outputFile << fixedInputs.get_FrameAdjustedDataPoint(dataSetRowIndex,
							count, dataFrameIndex, dataSetColumnFrameIndices.get_Element(count));

						*(dataSetColumnFrameIndices.get_ElementAddress(count)) += 1;

					}

				}

				for (recordingCalculatedInputIndex = 0; recordingCalculatedInputIndex < calculatedInputsCount; recordingCalculatedInputIndex++)
					outputFile << ",";

				for (recordingOutputIndex = 0; recordingOutputIndex < outputCellCount; recordingOutputIndex++)
					outputFile << ",";

				outputFile << "\n";

			}
			//***</RECORDING>***

			//data frame loop
			do
			{

				//***<RECORDING>***
				for (count = 0; count < dataSetColumnCount; count++)
					*(dataSetColumnFrameIndices.get_ElementAddress(count)) = fixedInputs.get_ColumnFrameLength(count) - (maxShiftLengthMinusOne + 1);

				//all but final iteration
				for (recordingSubFrameIndex = 0; recordingSubFrameIndex < maxShiftLengthMinusOne; recordingSubFrameIndex++)
				{

					for (count = 0; count < dataSetColumnCount; count++)
					{

						outputFile << ",";

						if (dataSetColumnFrameIndices.get_Element(count) < fixedInputs.get_ColumnFrameLength(count))
						{

							outputFile << fixedInputs.get_FrameAdjustedDataPoint(dataSetRowIndex,
								count, dataFrameIndex, dataSetColumnFrameIndices.get_Element(count));

							*(dataSetColumnFrameIndices.get_ElementAddress(count)) += 1;

						}

					}

					for (recordingCalculatedInputIndex = 0; recordingCalculatedInputIndex < calculatedInputsCount; recordingCalculatedInputIndex++)
						outputFile << ",";

					for (recordingOutputIndex = 0; recordingOutputIndex < outputCellCount; recordingOutputIndex++)
						outputFile << ",";

					outputFile << "\n";

				}

				if (!maxShiftLengthMinusOne && dataFrameIndex)
					outputFile << "\n";

				//final iteration
				outputFile << "dataFrame_" << dataFrameIndex;

				for (count = 0; count < dataSetColumnCount; count++)
				{

					outputFile << ",";

					if (dataSetColumnFrameIndices.get_Element(count) < fixedInputs.get_ColumnFrameLength(count))
					{

						outputFile << fixedInputs.get_FrameAdjustedDataPoint(dataSetRowIndex,
							count, dataFrameIndex, dataSetColumnFrameIndices.get_Element(count));

						*(dataSetColumnFrameIndices.get_ElementAddress(count)) += 1;

					}

				}

				for (count = 0; count < calculatedInputsCount; count++)
				{

					outputFile << "," << calculatedInputs.get_ElementAddress(
						count)->get_CurrentValue();

				}
				//***</RECORDING>***

				//stimulate the net
				stimulate_SingleFrame(dataSetRowIndex, dataFrameIndex, netNode->content.get_Cells(),
					activationListRoundA, activationListRoundB);

				//***<RECORDING>***
				for (count = 0; count < outputCellCount; count++)
				{

					outputFile << "," << outputs.get_ElementAddress(
						count)->get_Value();

				}
				//***</RECORDING>***

				//iterate the data frame
				dataFrameIndex++;

				//reassign any fixedInputs dataPoint variables used in
				//calculatedInputUpdateExpressions to prep for the next dataFrame
				fixedInputsVariableNode = updatingFixedInputsVariables.get_FirstNode();

				while (fixedInputsVariableNode)
				{

					fixedInputsVariableNode->content.get_DataPointVariable()->reassign_Value(
						fixedInputs.get_FrameAdjustedDataPointAddress(dataSetRowIndex,
							fixedInputsVariableNode->content.get_DataPointColumnIndex(),
							dataFrameIndex, fixedInputsVariableNode->content.get_DataPointFrameSubIndex()));

					fixedInputsVariableNode = fixedInputsVariableNode->get_NextNode();

				}

				//update calculatedInputs values via calculatedInputUpdateExpressions
				//(done here in two steps so that calculatedInputs members can effectively
				//be updated in parallel, not in series to ensure that changes occuring within
				//each member round do not impact calculations of members in later array position
				//in context of the current dataFrame iteration)
				for (count = 0; count < calculatedInputsCount; count++)
					*(calculatedInputsPrepValues.get_ElementAddress(count)) =
					calculatedInputUpdateExpressions.get_ElementAddress(count)->evaluate();

				for (count = 0; count < calculatedInputsCount; count++)
					calculatedInputs.get_ElementAddress(count)->set_CurrentValue(
						calculatedInputsPrepValues.get_Element(count));

			} while (dataFrameIndex < totalDataFrameCount);


			//update the net's fitness rating
			netNode->content.modify_FitnessRating(evaluationCriteriaExpressions.get_ElementAddress(
				dataSetRowIndex)->evaluate());

			dataSetRowIndex++;

		} while (dataSetRowIndex < dataSetRowCount);

	}

	*/

	//uses all data sets/frames to stimulate the net in first position (later can revise
	//to potentially record a list of nets; updates the net(s)'s performance rating(s)
	//in the process, but does not sort; assumes outputFile is already open,
	//iterates through all data frames in all data set rows referred to by currentEvolutionControl
	//one time; updates nets' performance ratings in the process, but does not sort or reproduce them
	//assumes currentEvolutionControl != NULL, assumes
	inline void stimulate_AllFrames_SingleEvolutionCycle_Recorded(ExtensibleArray<int> &activationListRoundA,
		ExtensibleArray<int> &activationListRoundB, ExtensibleArray<double> &calculatedInputsPrepValues)
		//EvolutionControl *currentEvolutionControl)
	{

		//initial setup
		UniformNode<Net> *netNode(nets.get_FirstNode());
		unsigned long long dataSetRowIndex;
		//unsigned long long evolutionControlInternalIndex;
		//unsigned long long evolutionControlInternalLength(currentEvolutionControl->get_DataSetRowIndicesLength());
		unsigned long long dataFrameIndex;
		unsigned short count;
		UniformNode<FixedInputReference> *fixedInputsVariableNode;

		//***RECORDING***
		recordingData.reset();
		UniformNode<RecordedDataSetRow> *currentRow;
		//***RECORDING***

		//net loop
		//do
		//{

			//reset the net's fitness rating
			netNode->content.reset_FitnessRating();

			dataSetRowIndex = 0;
			//evolutionControlInternalIndex = 0;

			//data set row loop 
			do
			{

				//***RECORDING***
				currentRow = recordingData.add_NewDataSetRow(totalDataFrameCount);
				//***RECORDING***

				//set the correct data set row
				//dataSetRowIndex = currentEvolutionControl->get_DataSetRowIndicesElement(
				//evolutionControlInternalIndex);

				//reset calculated inputs to default values
				for (count = 0; count < calculatedInputsCount; count++)
					calculatedInputs.get_ElementAddress(count)->reset();

				dataFrameIndex = 0;

				//data frame loop
				do
				{

					//stimulate the net
					stimulate_SingleFrame_Recorded(dataSetRowIndex, dataFrameIndex, netNode->content.get_Cells(),
						activationListRoundA, activationListRoundB, currentRow);

					//iterate the data frame
					dataFrameIndex++;

					//reassign any fixedInputs dataPoint variables used in
					//calculatedInputUpdateExpressions to prep for the next dataFrame
					fixedInputsVariableNode = updatingFixedInputsVariables.get_FirstNode();

					while (fixedInputsVariableNode)
					{

						fixedInputsVariableNode->content.get_DataPointVariable()->reassign_Value(
							fixedInputs.get_FrameAdjustedDataPointAddress(dataSetRowIndex,
								fixedInputsVariableNode->content.get_DataPointColumnIndex(),
								dataFrameIndex, fixedInputsVariableNode->content.get_DataPointFrameSubIndex()));

						fixedInputsVariableNode = fixedInputsVariableNode->get_NextNode();

					}

					//update calculatedInputs values via calculatedInputUpdateExpressions
					//(done here in two steps so that calculatedInputs members can effectively
					//be updated in parallel, not in series to ensure that changes occuring within
					//each member round do not impact calculations of members in later array position
					//in context of the current dataFrame iteration)
					for (count = 0; count < calculatedInputsCount; count++)
						*(calculatedInputsPrepValues.get_ElementAddress(count)) =
						calculatedInputUpdateExpressions.get_ElementAddress(count)->evaluate();

					for (count = 0; count < calculatedInputsCount; count++)
						calculatedInputs.get_ElementAddress(count)->set_CurrentValue(
							calculatedInputsPrepValues.get_Element(count));

				} while (dataFrameIndex < totalDataFrameCount);

				//update the net's fitness rating
				netNode->content.modify_FitnessRating(evaluationCriteriaExpressions.get_ElementAddress(
					dataSetRowIndex)->evaluate());

				dataSetRowIndex++;
				//evolutionControlInternalIndex++;

			} while (dataSetRowIndex < dataSetRowCount);
			//} while (evolutionControlInternalIndex < evolutionControlInternalLength);

			//netNode = netNode->get_NextNode();

		//} while (netNode);

	}

	/*

	//uses all data sets/frames to stimulate the net in first position (later can revise
	//to potentially record a list of nets; updates the net(s)'s performance rating(s)
	//in the process, but does not sort; assumes outputFile is already open,
	inline void stimulate_AllFramesAllParallels_XMLMinimalRecord(ExtensibleArray<int> &activationListRoundA,
		ExtensibleArray<int> &activationListRoundB, ExtensibleArray<double> &calculatedInputsPrepValues,
		ofstream &outputFile)
	{

		//initial setup
		UniformNode<Net> *netNode(nets.get_FirstNode());
		unsigned long long dataSetRowIndex;
		unsigned long long dataFrameIndex;
		unsigned short count;
		UniformNode<FixedInputReference> *fixedInputsVariableNode;

		//***RECORDING***
		unsigned short netIndex(0);
		unsigned short recordingDataFrameSubIndex;
		unsigned short recordingDataSetFrameLength;
		//***RECORDING***

		//net loop
		do
		{

			//***RECORDING***
			outputFile << "\n<net_" << netIndex << ">";
			//***RECORDING***

			//reset the net's fitness rating
			netNode->content.reset_FitnessRating();

			dataSetRowIndex = 0;

			//data set row loop
			do
			{

				//***RECORDING***
				outputFile << "\n\t<dataSetRow_" << dataSetRowIndex << ">";
				outputFile << "\n\t\t<name>";
				fixedInputs.get_RowName(dataSetRowIndex).output_ToOpenFile(outputFile);
				outputFile << "</name>";
				//***RECORDING***

				//reset calculated inputs to default values
				for (count = 0; count < calculatedInputsCount; count++)
					calculatedInputs.get_ElementAddress(count)->reset();

				dataFrameIndex = 0;

				//data frame loop
				do
				{

					//***RECORDING***
					count = 0;

					outputFile << "\n\t\t<dataFrame_" << dataFrameIndex << ">";
					outputFile << "\n\t\t\t<inputs>";
					outputFile << "\n\t\t\t\t<fixedInputs>";

					//count as dataSetColumnIndex
					do
					{

						recordingDataFrameSubIndex = 0;

						recordingDataSetFrameLength = fixedInputs.get_ColumnFrameLength(count);

						outputFile << "\n\t\t\t\t\t<dataSetColumn_" << count << ">";
						outputFile << "\n\t\t\t\t\t\t<name>";
						fixedInputs.get_ColumnName(count).output_ToOpenFile(outputFile);
						outputFile << "</name>";

						do
						{

							outputFile << "\n\t\t\t\t\t\t<value_" << recordingDataFrameSubIndex << ">";
							outputFile << fixedInputs.get_FrameAdjustedDataPoint(dataSetRowIndex, count,
								dataFrameIndex, recordingDataFrameSubIndex);
							outputFile << "</value_" << recordingDataFrameSubIndex << ">";

							recordingDataFrameSubIndex++;

						} while (recordingDataFrameSubIndex < recordingDataSetFrameLength);

						outputFile << "\n\t\t\t\t\t</dataSetColumn_" << count << ">";

						count++;

					} while (count < dataSetColumnCount);
					outputFile << "\n\t\t\t\t</fixedInputs>";

					outputFile << "\n\t\t\t\t<calculatedInputs>";
					for (count = 0; count < calculatedInputsCount; count++)
					{

						outputFile << "\n\t\t\t\t\t<ci_" << count << ">";
						outputFile << "\n\t\t\t\t\t\t<name>";
						calculatedInputs.get_ElementAddress(count)->outputName_ToOpenFile(outputFile);
						outputFile << "</name>";
						outputFile << "\n\t\t\t\t\t\t<value>" << calculatedInputs.get_ElementAddress(count)->get_CurrentValue() << "</value>";
						outputFile << "\n\t\t\t\t\t</ci_" << count << ">";

					}
					outputFile << "\n\t\t\t\t</calculatedInputs>";
					outputFile << "\n\t\t\t</inputs>";
					//***RECORDING***

					//stimulate the net
					stimulate_SingleFrame(dataSetRowIndex, dataFrameIndex,
						netNode->content.get_Cells(), activationListRoundA,
						activationListRoundB);

					//***RECORDING***
					outputFile << "\n\t\t\t<outputs>";
					for (count = 0; count < outputCellCount; count++)
					{

						outputFile << "\n\t\t\t\t<o_" << count << ">";
						outputFile << "\n\t\t\t\t\t<name>";
						outputs.get_ElementAddress(count)->outputName_ToOpenFile(outputFile);
						outputFile << "</name>";
						outputFile << "\n\t\t\t\t\t<value>" << outputs.get_ElementAddress(count)->get_Value() << "</value>";
						outputFile << "\n\t\t\t\t</o_" << count << ">";

					}
					outputFile << "\n\t\t\t</outputs>";
					outputFile << "\n\t\t</dataFrame_" << dataFrameIndex << ">";
					//***RECORDING***

					//iterate the data frame
					dataFrameIndex++;

					//reassign any fixedInputs dataPoint variables used in
					//calculatedInputUpdateExpressions to prep for the next dataFrame
					fixedInputsVariableNode = updatingFixedInputsVariables.get_FirstNode();

					while (fixedInputsVariableNode)
					{

						fixedInputsVariableNode->content.get_DataPointVariable()->reassign_Value(
							fixedInputs.get_FrameAdjustedDataPointAddress(dataSetRowIndex,
								fixedInputsVariableNode->content.get_DataPointColumnIndex(),
								dataFrameIndex, fixedInputsVariableNode->content.get_DataPointFrameSubIndex()));

						fixedInputsVariableNode = fixedInputsVariableNode->get_NextNode();

					}

					//update calculatedInputs values via calculatedInputUpdateExpressions
					//(done here in two steps so that calculatedInputs members can effectively
					//be updated in parallel, not in series to ensure that changes occuring within
					//each member round do not impact calculations of members in later array position
					//in context of the current dataFrame iteration)
					for (count = 0; count < calculatedInputsCount; count++)
						*(calculatedInputsPrepValues.get_ElementAddress(count)) =
						calculatedInputUpdateExpressions.get_ElementAddress(count)->evaluate();

					for (count = 0; count < calculatedInputsCount; count++)
						calculatedInputs.get_ElementAddress(count)->set_CurrentValue(
							calculatedInputsPrepValues.get_Element(count));

				} while (dataFrameIndex < totalDataFrameCount);


				//update the net's fitness rating
				netNode->content.modify_FitnessRating(evaluationCriteriaExpressions.get_ElementAddress(
					dataSetRowIndex)->evaluate());

				//***RECORDING***
				outputFile << "\n\t</dataSetRow_" << dataSetRowIndex << ">";
				//***RECORDING***

				dataSetRowIndex++;

			} while (dataSetRowIndex < dataSetRowCount);

			//***RECORDING***
			outputFile << "\n\t<fitnessRating>" << netNode->content.get_FitnessRating() << "</fitnessRating>";
			outputFile << "\n</net_" << netIndex << ">";
			//***RECORDING***

			netIndex++;
			netNode = netNode->get_NextNode();

		} while (netIndex < netsIncluded);

	}

	//uses all data sets/frames to stimulate netsIncluded number of nets
	//starting from net in first position; updates nets' performance ratings
	//in the process, but does not sort them; assumes outputFile is already open,
	//assumes netsIncluded >= 1
	inline void stimulate_AllFramesAllParallels_XMLFullRecord(ExtensibleArray<int> &activationListRoundA,
		ExtensibleArray<int> &activationListRoundB, ExtensibleArray<double> &calculatedInputsPrepValues,
		ofstream &outputFile, const unsigned short netsIncluded)
	{

		//initial setup
		UniformNode<Net> *netNode(nets.get_FirstNode());
		unsigned long long dataSetRowIndex;
		unsigned long long dataFrameIndex;
		unsigned short count;
		UniformNode<FixedInputReference> *fixedInputsVariableNode;

		//***RECORDING***
		unsigned short netIndex(0);
		unsigned short recordingDataFrameSubIndex;
		unsigned short recordingDataSetFrameLength;
		//***RECORDING***

		//net loop
		do
		{

			//***RECORDING***
			outputFile << "\n<net_" << netIndex << ">";
			//***RECORDING***

			//reset the net's fitness rating
			netNode->content.reset_FitnessRating();

			dataSetRowIndex = 0;

			//data set row loop
			do
			{

				//***RECORDING***
				outputFile << "\n\t<dataSetRow_" << dataSetRowIndex << ">";
				outputFile << "\n\t\t<name>";
				fixedInputs.get_RowName(dataSetRowIndex).output_ToOpenFile(outputFile);
				outputFile << "</name>";
				//***RECORDING***

				//reset calculated inputs to default values
				for (count = 0; count < calculatedInputsCount; count++)
					calculatedInputs.get_ElementAddress(count)->reset();

				dataFrameIndex = 0;

				//data frame loop
				do
				{

					//***RECORDING***
					count = 0;

					outputFile << "\n\t\t<dataFrame_" << dataFrameIndex << ">";
					outputFile << "\n\t\t\t<inputs>";
					outputFile << "\n\t\t\t\t<fixedInputs>";

					//count as dataSetColumnIndex
					do
					{

						recordingDataFrameSubIndex = 0;

						recordingDataSetFrameLength = fixedInputs.get_ColumnFrameLength(count);

						outputFile << "\n\t\t\t\t\t<dataSetColumn_" << count << ">";
						outputFile << "\n\t\t\t\t\t\t<name>";
						fixedInputs.get_ColumnName(count).output_ToOpenFile(outputFile);
						outputFile << "</name>";

						do
						{

							outputFile << "\n\t\t\t\t\t\t<value_" << recordingDataFrameSubIndex << ">";
							outputFile << fixedInputs.get_FrameAdjustedDataPoint(dataSetRowIndex, count,
								dataFrameIndex, recordingDataFrameSubIndex);
							outputFile << "</value_" << recordingDataFrameSubIndex << ">";

							recordingDataFrameSubIndex++;

						} while (recordingDataFrameSubIndex < recordingDataSetFrameLength);

						outputFile << "\n\t\t\t\t\t</dataSetColumn_" << count << ">";

						count++;

					} while (count < dataSetColumnCount);
					outputFile << "\n\t\t\t\t</fixedInputs>";

					outputFile << "\n\t\t\t\t<calculatedInputs>";
					for (count = 0; count < calculatedInputsCount; count++)
					{

						outputFile << "\n\t\t\t\t\t<ci_" << count << ">";
						outputFile << "\n\t\t\t\t\t\t<name>";
						calculatedInputs.get_ElementAddress(count)->outputName_ToOpenFile(outputFile);
						outputFile << "</name>";
						outputFile << "\n\t\t\t\t\t\t<value>" << calculatedInputs.get_ElementAddress(count)->get_CurrentValue() << "</value>";
						outputFile << "\n\t\t\t\t\t</ci_" << count << ">";

					}
					outputFile << "\n\t\t\t\t</calculatedInputs>";
					outputFile << "\n\t\t\t</inputs>";
					//***RECORDING***

					//stimulate the net
					stimulate_SingleFrame_Recorded(dataSetRowIndex, dataFrameIndex,
						netNode->content.get_Cells(), activationListRoundA,
						activationListRoundB, outputFile, true);

					//***RECORDING***
					outputFile << "\n\t\t\t<outputs>";
					for (count = 0; count < outputCellCount; count++)
					{

						outputFile << "\n\t\t\t\t<o_" << count << ">";
						outputFile << "\n\t\t\t\t\t<name>";
						outputs.get_ElementAddress(count)->outputName_ToOpenFile(outputFile);
						outputFile << "</name>";
						outputFile << "\n\t\t\t\t\t<value>" << outputs.get_ElementAddress(count)->get_Value() << "</value>";
						outputFile << "\n\t\t\t\t</o_" << count << ">";

					}
					outputFile << "\n\t\t\t</outputs>";
					outputFile << "\n\t\t</dataFrame_" << dataFrameIndex << ">";
					//***RECORDING***

					//iterate the data frame
					dataFrameIndex++;

					//reassign any fixedInputs dataPoint variables used in
					//calculatedInputUpdateExpressions to prep for the next dataFrame
					fixedInputsVariableNode = updatingFixedInputsVariables.get_FirstNode();

					while (fixedInputsVariableNode)
					{

						fixedInputsVariableNode->content.get_DataPointVariable()->reassign_Value(
							fixedInputs.get_FrameAdjustedDataPointAddress(dataSetRowIndex,
								fixedInputsVariableNode->content.get_DataPointColumnIndex(),
								dataFrameIndex, fixedInputsVariableNode->content.get_DataPointFrameSubIndex()));

						fixedInputsVariableNode = fixedInputsVariableNode->get_NextNode();

					}

					//update calculatedInputs values via calculatedInputUpdateExpressions
					//(done here in two steps so that calculatedInputs members can effectively
					//be updated in parallel, not in series to ensure that changes occuring within
					//each member round do not impact calculations of members in later array position
					//in context of the current dataFrame iteration)
					for (count = 0; count < calculatedInputsCount; count++)
						*(calculatedInputsPrepValues.get_ElementAddress(count)) =
						calculatedInputUpdateExpressions.get_ElementAddress(count)->evaluate();

					for (count = 0; count < calculatedInputsCount; count++)
						calculatedInputs.get_ElementAddress(count)->set_CurrentValue(
							calculatedInputsPrepValues.get_Element(count));

				} while (dataFrameIndex < totalDataFrameCount);


				//update the net's fitness rating
				netNode->content.modify_FitnessRating(evaluationCriteriaExpressions.get_ElementAddress(
					dataSetRowIndex)->evaluate());

				//***RECORDING***
				outputFile << "\n\t</dataSetRow_" << dataSetRowIndex << ">";
				//***RECORDING***

				dataSetRowIndex++;

			} while (dataSetRowIndex < dataSetRowCount);

			//***RECORDING***
			outputFile << "\n\t<fitnessRating>" << netNode->content.get_FitnessRating() << "</fitnessRating>";
			outputFile << "\n</net_" << netIndex << ">";
			//***RECORDING***

			netIndex++;
			netNode = netNode->get_NextNode();

		} while (netIndex < netsIncluded);

	}

	//uses all data sets/frames to stimulate netsIncluded number of nets
	//starting from net in first position; updates nets' performance ratings
	//in the process, but does not sort them; assumes outputFile is already open,
	//assumes netsIncluded >= 1
	inline void stimulate_AllFramesAllParallels_JSONFullRecord(ExtensibleArray<int> &activationListRoundA,
		ExtensibleArray<int> &activationListRoundB, ExtensibleArray<double> &calculatedInputsPrepValues,
		ofstream &outputFile, const unsigned short netsIncluded)
	{

		//initial setup
		UniformNode<Net> *netNode(nets.get_FirstNode());
		unsigned long long dataSetRowIndex;
		unsigned long long dataFrameIndex;
		unsigned short count;
		UniformNode<FixedInputReference> *fixedInputsVariableNode;

		//***RECORDING***
		unsigned short netIndex(0);
		unsigned short recordingDataFrameSubIndex;
		unsigned short recordingDataSetFrameLength;
		//***RECORDING***

		//***RECORDING***
		outputFile << "\"nets\":{";
		//***RECORDING***

		//net loop
		do
		{

			//***RECORDING***
			outputFile << "{";
			//***RECORDING***

			//reset the net's fitness rating
			netNode->content.reset_FitnessRating();

			dataSetRowIndex = 0;

			//***RECORDING***
			outputFile << "\"dataSetRows\":{";
			//***RECORDING***

			//data set row loop
			do
			{

				//***RECORDING***
				outputFile << "{";
				outputFile << "\"name\":";
				fixedInputs.get_RowName(dataSetRowIndex).output_ToOpenFile(outputFile);
				outputFile << ",";
				//***RECORDING***

				//reset calculated inputs to default values
				for (count = 0; count < calculatedInputsCount; count++)
					calculatedInputs.get_ElementAddress(count)->reset();

				dataFrameIndex = 0;

				//***RECORDING***
				outputFile << "\"dataFrames\":{";
				//***RECORDING***

				//data frame loop
				do
				{

					//***RECORDING***
					count = 0;

					outputFile << "{";
					outputFile << "\"inputs\":{";
					outputFile << "\"fixedInputs\":{";
					outputFile << "\"dataSets\":{";

					//count as dataSetColumnIndex
					do
					{

						recordingDataFrameSubIndex = 0;

						recordingDataSetFrameLength = fixedInputs.get_ColumnFrameLength(count);

						outputFile << "{";
						outputFile << "\"name\":";
						fixedInputs.get_ColumnName(count).output_ToOpenFile(outputFile);
						outputFile << ",\"values\":{";

						do
						{

							outputFile << fixedInputs.get_FrameAdjustedDataPoint(dataSetRowIndex, count,
								dataFrameIndex, recordingDataFrameSubIndex);

							if (recordingDataFrameSubIndex < recordingDataSetFrameLength - 1)
								outputFile << ",";

							recordingDataFrameSubIndex++;

						} while (recordingDataFrameSubIndex < recordingDataSetFrameLength);

						outputFile << "}";

						if (count < dataSetColumnCount - 1)
							outputFile << ",";

						count++;

					} while (count < dataSetColumnCount);

					outputFile << "}}\"calculatedInputs\":{";
					for (count = 0; count < calculatedInputsCount; count++)
					{

						outputFile << "{";
						outputFile << "\"name\":";
						calculatedInputs.get_ElementAddress(count)->outputName_ToOpenFile(outputFile);
						outputFile << ",\"value\":" << calculatedInputs.get_ElementAddress(count)->get_CurrentValue() << "}";

						if (count < calculatedInputsCount - 1)
							outputFile << ",";

					}
					outputFile << "}},";
					//***RECORDING***

					//stimulate the net
					stimulate_SingleFrame_Recorded(dataSetRowIndex, dataFrameIndex,
						netNode->content.get_Cells(), activationListRoundA,
						activationListRoundB, outputFile, false);

					//***RECORDING***
					outputFile << "\"outputs\":{";
					for (count = 0; count < outputCellCount; count++)
					{

						outputFile << "{";
						outputFile << "\"name\":";
						outputs.get_ElementAddress(count)->outputName_ToOpenFile(outputFile);
						outputFile << ",\"value\":" << outputs.get_ElementAddress(count)->get_Value() << "}";

						if (count < outputCellCount - 1)
							outputFile << ",";

					}
					outputFile << "}}";
					//***RECORDING***

					//iterate the data frame
					dataFrameIndex++;

					//reassign any fixedInputs dataPoint variables used in
					//calculatedInputUpdateExpressions to prep for the next dataFrame
					fixedInputsVariableNode = updatingFixedInputsVariables.get_FirstNode();

					while (fixedInputsVariableNode)
					{

						fixedInputsVariableNode->content.get_DataPointVariable()->reassign_Value(
							fixedInputs.get_FrameAdjustedDataPointAddress(dataSetRowIndex,
								fixedInputsVariableNode->content.get_DataPointColumnIndex(),
								dataFrameIndex, fixedInputsVariableNode->content.get_DataPointFrameSubIndex()));

						fixedInputsVariableNode = fixedInputsVariableNode->get_NextNode();

					}

					//update calculatedInputs values via calculatedInputUpdateExpressions
					//(done here in two steps so that calculatedInputs members can effectively
					//be updated in parallel, not in series to ensure that changes occuring within
					//each member round do not impact calculations of members in later array position
					//in context of the current dataFrame iteration)
					for (count = 0; count < calculatedInputsCount; count++)
						*(calculatedInputsPrepValues.get_ElementAddress(count)) =
						calculatedInputUpdateExpressions.get_ElementAddress(count)->evaluate();

					for (count = 0; count < calculatedInputsCount; count++)
						calculatedInputs.get_ElementAddress(count)->set_CurrentValue(
							calculatedInputsPrepValues.get_Element(count));

				} while (dataFrameIndex < totalDataFrameCount);


				//update the net's fitness rating
				netNode->content.modify_FitnessRating(evaluationCriteriaExpressions.get_ElementAddress(
					dataSetRowIndex)->evaluate());

				//***RECORDING***
				outputFile << "}";

				if (dataSetRowIndex < dataSetRowCount - 1)
					outputFile << ",";
				//***RECORDING***

				dataSetRowIndex++;

			} while (dataSetRowIndex < dataSetRowCount);

			//***RECORDING***
			outputFile << "},\"fitnessRating\":" << netNode->content.get_FitnessRating() << "}";
			//***RECORDING***

			netIndex++;
			netNode = netNode->get_NextNode();

		} while (netIndex < netsIncluded);

	}

	*/

	/*
	checks the leading and numerical properties section of loadedContent to ensure it
	matches the xml format described below (w/out whitespace); initializes totalNetCount,
	inputCellCount, outputCellCount, totalCellCount, maxRoundsPerStimulate,
	dataSetColumnCount, dataSetRowCount, totalDataFrameCount, and calculatedInputsCount;
	returns 0 if the file section is valid, otherwise returns an error code for console reporting;
	"<?xmlversion=\"1.0\"?>"										//required
	<Population>													//required
		<totalNetCount>...</totalNetCount>							//required (unsigned short >= 2)
		<inputCellCount>...</inputCellCount>						//required (unsigned short >= 1)
		<outputCellCount>...</outputCellCount>						//required (unsigned short >= 1)
		<totalCellCount>...</totalCellCount>						//required (unsigned short >= 3 and >= (inputCellCount + outputCellCount))
		<maxRoundsPerStimulate>...</maxRoundsPerStimulate>			//required (unsigned short >= 1)
		<dataSetColumnCount>...</dataSetColumnCount>				//required (unsigned short >= 1)
		<dataSetRowCount>...</dataSetRowCount>						//required (unsigned long long >= 1)
		<totalDataFrameCount>...</totalDataFrameCount>				//required (unsigned long long >= 1)
		<calculatedInputsCount>...</calculatedInputsCount>			//required (unsigned short any value)	
	*/
	inline unsigned char sub_Initialize_Validate_NumericalProperties(
		FileValidation_WordGroup &loadedContent, UniformNode<Word> *&wordIterationNode)
	{

		//variables for leading section and fixedInputs sections validation
		Word headerTag("<?xmlversion=\"1.0\"?>");
		Word populationInTag("<Population>");
		Word totalNetCountInTag("<totalNetCount>");
		Word totalNetCountOutTag("</totalNetCount>");
		Word inputCellCountInTag("<inputCellCount>");
		Word inputCellCountOutTag("</inputCellCount>");
		Word outputCellCountInTag("<outputCellCount>");
		Word outputCellCountOutTag("</outputCellCount>");
		Word totalCellCountInTag("<totalCellCount>");
		Word totalCellCountOutTag("</totalCellCount>");
		Word maxRoundsPerStimulateInTag("<maxRoundsPerStimulate>");
		Word maxRoundsPerStimulateOutTag("</maxRoundsPerStimulate>");
		Word dataSetColumnCountInTag("<dataSetColumnCount>");
		Word dataSetColumnCountOutTag("</dataSetColumnCount>");
		Word dataSetRowCountInTag("<dataSetRowCount>");
		Word dataSetRowCountOutTag("</dataSetRowCount>");
		Word totalDataFrameCountInTag("<totalDataFrameCount>");
		Word totalDataFrameCountOutTag("</totalDataFrameCount>");
		Word calculatedInputsCountInTag("<calculatedInputsCount>");
		Word calculatedInputsCountOutTag("</calculatedInputsCount>");

		//main body of validation logic

		//check for required header
		if (!loadedContent.validateWordNode_MatchesWord(headerTag, wordIterationNode))
			return 1;

		//check for required <Population> tag
		if (!loadedContent.validateWordNode_MatchesWord(populationInTag, wordIterationNode))
			return 2;

		//check for required <totalNetCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(totalNetCountInTag, wordIterationNode))
			return 3;

		//check for required <totalNetCount> value (>= 2)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, totalNetCount, 2))
			return 4;

		//check for required </totalNetCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(totalNetCountOutTag, wordIterationNode))
			return 5;

		//check for required <inputCellCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(inputCellCountInTag, wordIterationNode))
			return 6;

		//check for required <inputCellCount> value (>= 1)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, inputCellCount, 1))
			return 7;

		//check for required </inputCellCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(inputCellCountOutTag, wordIterationNode))
			return 8;

		//check for required <outputCellCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(outputCellCountInTag, wordIterationNode))
			return 9;

		//check for required <outputCellCount> value (>= 1)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, outputCellCount, 1))
			return 10;

		//check for required </outputCellCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(outputCellCountOutTag, wordIterationNode))
			return 11;

		//check for required <totalCellCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(totalCellCountInTag, wordIterationNode))
			return 12;

		//check for required <totalCellCount> value (>= 3 and >= inputCellCount + OutputCellCount)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, totalCellCount, 3)
			|| totalCellCount < (inputCellCount + outputCellCount))
			return 13;

		//check for required </totalCellCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(totalCellCountOutTag, wordIterationNode))
			return 14;

		//check for required <maxRoundsPerStimulate> tag
		if (!loadedContent.validateWordNode_MatchesWord(maxRoundsPerStimulateInTag, wordIterationNode))
			return 15;

		//check for required <maxRoundsPerStimulate> value (>= 1)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, maxRoundsPerStimulate, 1))
			return 16;

		//check for required </maxRoundsPerStimulate> tag
		if (!loadedContent.validateWordNode_MatchesWord(maxRoundsPerStimulateOutTag, wordIterationNode))
			return 17;

		//check for required <dataSetColumnCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(dataSetColumnCountInTag, wordIterationNode))
			return 18;

		//check for required <dataSetColumnCount> value (>= 1)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, dataSetColumnCount, 1))
			return 19;

		//check for required </dataSetColumnCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(dataSetColumnCountOutTag, wordIterationNode))
			return 20;

		//check for required <dataSetRowCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(dataSetRowCountInTag, wordIterationNode))
			return 21;

		//check for required <dataSetRowCount> value (>= 1)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, dataSetRowCount, 1))
			return 22;

		//check for required </dataSetRowCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(dataSetRowCountOutTag, wordIterationNode))
			return 23;

		//check for required <totalDataFrameCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(totalDataFrameCountInTag, wordIterationNode))
			return 24;

		//check for required <totalDataFrameCount> value (>= 1)
		if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, totalDataFrameCount, 1))
			return 25;

		//check for required </totalDataFrameCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(totalDataFrameCountOutTag, wordIterationNode))
			return 26;

		//check for required <calculatedInputsCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(calculatedInputsCountInTag, wordIterationNode))
			return 27;

		//check for required <calculatedInputsCount> value (unsigned short any value)
		if (!loadedContent.validateWordNode_UnsignedInt_Unlimited(wordIterationNode, calculatedInputsCount))
			return 28;

		//check for required </calculatedInputsCount> tag
		if (!loadedContent.validateWordNode_MatchesWord(calculatedInputsCountOutTag, wordIterationNode))
			return 29;

		//this section of the file is valid; totalNetCount, inputCellCount, outputCellCount,
		//totalCellCount, maxRoundsPerStimulate, dataSetColumnCount, dataSetRowCount,
		//totalDataFrameCount, and calculatedInputsCount have been initialized
		return 0;

	}

	inline bool sub_Initialize_InterpretReturn_NumericalPropertiesValidation(
		const unsigned char fileValidationReturnCode)
	{

		cout << "\n";

		if (fileValidationReturnCode == 0)
		{

			cout << "leading and numerical properties file sections are valid";
			return true;

		}
			
		else
		{

			if (fileValidationReturnCode == 1)
				cout << "missing XML header or incomplete file";
			else if (fileValidationReturnCode == 2)
				cout << "missing <Population> tag or incomplete file";
			else if (fileValidationReturnCode == 3)
				cout << "missing <totalNetCount> tag or incomplete file";
			else if (fileValidationReturnCode == 4)
				cout << "invalid <totalNetCount> value or incomplete file";
			else if (fileValidationReturnCode == 5)
				cout << "missing </totalNetCount> tag or incomplete file";
			else if (fileValidationReturnCode == 6)
				cout << "missing <inputCellCount> tag or incomplete file";
			else if (fileValidationReturnCode == 7)
				cout << "invalid <inputCellCount> value or incomplete file";
			else if (fileValidationReturnCode == 8)
				cout << "missing </inputCellCount> tag or incomplete file";
			else if (fileValidationReturnCode == 9)
				cout << "missing <outputCellCount> tag or incomplete file";
			else if (fileValidationReturnCode == 10)
				cout << "invalid <outputCellCount> value or incomplete file";
			else if (fileValidationReturnCode == 11)
				cout << "missing </outputCellCount> tag or incomplete file";
			else if (fileValidationReturnCode == 12)
				cout << "missing <totalCellCount> tag or incomplete file";
			else if (fileValidationReturnCode == 13)
				cout << "invalid <totalCellCount> value or incomplete file";
			else if (fileValidationReturnCode == 14)
				cout << "missing </totalCellCount> tag or incomplete file";
			else if (fileValidationReturnCode == 15)
				cout << "missing <maxRoundsPerStimulate> tag or incomplete file";
			else if (fileValidationReturnCode == 16)
				cout << "invalid <maxRoundsPerStimulate> value or incomplete file";
			else if (fileValidationReturnCode == 17)
				cout << "missing </maxRoundsPerStimulate> tag or incomplete file";
			else if (fileValidationReturnCode == 18)
				cout << "missing <dataSetColumnCount> tag or incomplete file";
			else if (fileValidationReturnCode == 19)
				cout << "invalid <dataSetColumnCount> value or incomplete file";
			else if (fileValidationReturnCode == 20)
				cout << "missing </dataSetColumnCount> tag or incomplete file";
			else if (fileValidationReturnCode == 21)
				cout << "missing <dataSetRowCount> tag or incomplete file";
			else if (fileValidationReturnCode == 22)
				cout << "invalid <dataSetRowCount> value or incomplete file";
			else if (fileValidationReturnCode == 23)
				cout << "missing </dataSetRowCount> tag or incomplete file";
			else if (fileValidationReturnCode == 24)
				cout << "missing <totalDataFrameCount> tag or incomplete file";
			else if (fileValidationReturnCode == 25)
				cout << "invalid <totalDataFrameCount> value or incomplete file";
			else if (fileValidationReturnCode == 26)
				cout << "missing </totalDataFrameCount> tag or incomplete file";
			else if (fileValidationReturnCode == 27)
				cout << "missing <calculatedInputsCount> tag or incomplete file";
			else if (fileValidationReturnCode == 28)
				cout << "invalid <calculatedInputsCount> value or incomplete file";
			else if (fileValidationReturnCode == 29)
				cout << "missing </calculatedInputsCount> tag or incomplete file";

			//reverse any possible partial initialization

			//numerical properties null states
			totalNetCount = 2;
			inputCellCount = 1;
			outputCellCount = 1;
			totalCellCount = 3;
			maxRoundsPerStimulate = 1;
			dataSetColumnCount = 1;
			dataSetRowCount = 1;
			totalDataFrameCount = 1;
			calculatedInputsCount = 0;

			return false;

		}

	}

	/*
	checks the fixedInputs and calculatedInputs sections of loadedContent to
	ensure they match the xml format described below (w/out whitespace);
	initializes fixedInputs, calculatedInputs, and calculatedInputsPrepValues;
	returns 0 if the file section is valid, otherwise returns an error code for console reporting;
	<fixedInputs>												//required
		<name>...</name>										//required (alphanumeric Word)
		<columns>												//required
			<c_0>												//required (tag matches any <c_#>)
				<name>...</name>								//required (alphanumeric Word)
				<frameLength>...</frameLength>					//required (unsigned short >= 1)
				<shiftLength>...</shiftLength>					//required (any unsigned short)
			</c_0>												//required (tag matches any </c_#>)
			<c_1>												//optional
				...
			</c_1>
			...
		</columns>												//required
		<rows>													//required
			<r_0>												//required
				<name>...</name>								//required (alphanumeric Word)
				<dataSets>										//required
					<ds_0>										//required (tag matches any <ds_#>)
						<dataPoints>							//required
							<dp_0>...</dp_0>					//required (any double)
							<dp_1>...</dp_1>					//optional
							...
						</dataPoints>							//required
					</ds_0>										//required (tag matches any </ds_#>)
					<ds_1>										//optional
						...
					</ds_1>
					...
				</dataSets>										//required
			</r_0>
			<r_1>												//optional
				...
			</r_1>
			...
		</rows>
	</fixedInputs>
	<calculatedInputs>											//required
		<ci_0>													//optional (tag matches any <ci_#>)
			<name>...</name>									//required (alphanumeric Word)
			<defaultValue>...</defaultValue>					//required (any double)
		</ci_0>													//required (tag matches any </ci_#>)
		<ci_1>													//optional
			...
		</ci_0>
		...
	</calculatedInputs>											//required
	*/
	inline unsigned char sub_Initialize_Validate_Inputs(
		FileValidation_WordGroup &loadedContent, UniformNode<Word> *&wordIterationNode)
	{

		//variables for fixedInputs section validation
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;

		Word fixedInputsInTag("<fixedInputs>");
		Word fixedInputsOutTag("</fixedInputs>");
		Word nameInTag("<name>");
		Word nameOutTag("</name>");
		Word columnsInTag("<columns>");
		Word columnsOutTag("</columns>");

		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord columnInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord columnOutTag(tempConstructCodes);

		Word frameLengthInTag("<frameLength>");
		Word frameLengthOutTag("</frameLength>");
		Word shiftLengthInTag("<shiftLength>");
		Word shiftLengthOutTag("</shiftLength>");
		Word rowsInTag("<rows>");
		Word rowsOutTag("</rows>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('r'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord rowInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('r'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord rowOutTag(tempConstructCodes);

		Word dataSetsInTag("<dataSets>");
		Word dataSetsOutTag("</dataSets>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('d'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('s'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord dataSetInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('d'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('s'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord dataSetOutTag(tempConstructCodes);

		Word dataPointsInTag("<dataPoints>");
		Word dataPointsOutTag("</dataPoints>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('d'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('p'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord dataPointInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('d'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('p'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord dataPointOutTag(tempConstructCodes);

		//temp storage variables
		Word fixedInputsName;
		Word tempName;
		unsigned short tempFrameLength;
		unsigned short tempShiftLength;
		UniformList<DataSetColumn> tempDataSetColumns;
		UniformNode<DataSetColumn> *tempColumnNode;
		
		UniformList<double> tempDataPointsList;
		UniformNode<double> *tempDataPointNode;
		unsigned long long tempDataPointsListLength;

		UniformList<DataSet> tempDataSets;
		UniformList<DataSetRow> tempDataSetRows;

		unsigned short tempInputCellCount = 0;

		//main body of validation logic

		//check for required <fixedInputs> tag
		if (!loadedContent.validateWordNode_MatchesWord(fixedInputsInTag, wordIterationNode))
			return 1;

		//check for required <name> tag
		if (!loadedContent.validateWordNode_MatchesWord(nameInTag, wordIterationNode))
			return 49;

		//capture required name alphanumeric word
		if (!loadedContent.validateWordNode_CaptureAlphaNumeric(wordIterationNode, fixedInputsName))
			return 50;

		//check for required </name> tag
		if (!loadedContent.validateWordNode_MatchesWord(nameOutTag, wordIterationNode))
			return 51;

		//check for required <columns> tag
		if (!loadedContent.validateWordNode_MatchesWord(columnsInTag, wordIterationNode))
			return 2;

		//columns validation loop
		do
		{

			//check for required <c_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(columnInTag, wordIterationNode))
				return 3;

			//check for required <name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameInTag, wordIterationNode))
				return 4;

			//capture required name alphanumeric word
			if (!loadedContent.validateWordNode_CaptureAlphaNumeric(wordIterationNode, tempName))
				return 5;

			//check for required </name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameOutTag, wordIterationNode))
				return 6;

			//check for required <frameLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(frameLengthInTag, wordIterationNode))
				return 7;

			//capture required frameLength unsigned short (>= 1)
			if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, tempFrameLength, 1))
				return 8;

			//check for required </frameLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(frameLengthOutTag, wordIterationNode))
				return 9;

			//update the value of tempInputCellCount for later validation against inputCellCount
			tempInputCellCount += tempFrameLength;

			//check for required <shiftLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(shiftLengthInTag, wordIterationNode))
				return 10;

			//capture required shiftLength unsigned short
			if (!loadedContent.validateWordNode_UnsignedInt_Unlimited(wordIterationNode, tempShiftLength))
				return 11;

			//check for required </shiftLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(shiftLengthOutTag, wordIterationNode))
				return 12;

			//check for required associated </c_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(columnOutTag, wordIterationNode))
				return 13;

			tempDataSetColumns.add_NewNode(DataSetColumn(tempName,
				tempFrameLength, tempShiftLength));

		} while (columnInTag.testWord_MatchesTerm(wordIterationNode->content));

		//check for required </columns> tag
		if (!loadedContent.validateWordNode_MatchesWord(columnsOutTag, wordIterationNode))
			return 52;

		if (tempDataSetColumns.get_ListLength() != dataSetColumnCount)
			return 14;

		//check for required <rows> tag
		if (!loadedContent.validateWordNode_MatchesWord(rowsInTag, wordIterationNode))
			return 15;

		//row validation loop
		do
		{

			//check for required <r_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(rowInTag, wordIterationNode))
				return 16;

			//check for required <name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameInTag, wordIterationNode))
				return 17;

			//capture required name alphanumeric Word
			if (!loadedContent.validateWordNode_CaptureAlphaNumeric(wordIterationNode, tempName))
				return 18;

			//check for required </name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameOutTag, wordIterationNode))
				return 19;

			//check for required <dataSets> tag
			if (!loadedContent.validateWordNode_MatchesWord(dataSetsInTag, wordIterationNode))
				return 20;

			tempColumnNode = tempDataSetColumns.get_FirstNode();

			//data set validation loop
			do
			{

				//test whether there are more data sets in this row
				//than there should be based on the number of columns
				if (!tempColumnNode)
					return 30;

				//check for required <ds_#> tag
				if (!loadedContent.validateWordNode_MatchesTerm(dataSetInTag, wordIterationNode))
					return 21;

				//check for required <dataPoints> tag
				if (!loadedContent.validateWordNode_MatchesWord(dataPointsInTag, wordIterationNode))
					return 22;

				//tempDataPoints population loop
				do
				{

					//check for required <dp_#> tag
					if (!loadedContent.validateWordNode_MatchesTerm(dataPointInTag, wordIterationNode))
						return 23;

					//capture required dp_# value
					tempDataPointNode = tempDataPointsList.add_NewNode();
					if (!loadedContent.validateWordNode_FloatingPoint_Unlimited(
						wordIterationNode, tempDataPointNode->content))
						return 24;

					//check for required </dp_#> tag
					if (!loadedContent.validateWordNode_MatchesTerm(dataPointOutTag, wordIterationNode))
						return 25;

				} while (dataPointInTag.testWord_MatchesTerm(wordIterationNode->content));

				//check for required </dataPoints> tag
				if (!loadedContent.validateWordNode_MatchesWord(dataPointsOutTag, wordIterationNode))
					return 26;

				//check for required associated </ds_#> tag
				if (!loadedContent.validateWordNode_MatchesTerm(dataSetOutTag, wordIterationNode))
					return 27;

				//verify that the structure of data set is valid in context of it's corresponding row
				tempFrameLength = tempColumnNode->content.get_FrameLength();
				tempShiftLength = tempColumnNode->content.get_ShiftLength();
				tempDataPointsListLength = tempDataPointsList.get_ListLength();

				if (tempDataPointsListLength < (totalDataFrameCount * tempShiftLength) + tempFrameLength)
					return 28;

				else if (tempDataPointsListLength == tempFrameLength && totalDataFrameCount != 1)
					return 29;

				//add the current data set to tempDataSets and reset
				//tempDataPointsList to prep for the next possible data set
				tempDataSets.add_NewNode(DataSet(tempDataPointsList));
				tempDataPointsList.delete_WholeList();

				//iterate to the next column
				tempColumnNode = tempColumnNode->get_NextNode();

			} while (dataSetInTag.testWord_MatchesTerm(wordIterationNode->content));

			//ensure there are not fewer data sets in this row than there are columns
			if (tempDataSets.get_ListLength() != dataSetColumnCount)
				return 31;

			//check for required </dataSets> tag
			if (!loadedContent.validateWordNode_MatchesWord(dataSetsOutTag, wordIterationNode))
				return 32;

			//check for required associated </r_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(rowOutTag, wordIterationNode))
				return 33;

			//add the current row to tempDataSetRows and reset
			//tempDataSets to prep for the next possible row
			tempDataSetRows.add_NewNode(DataSetRow(tempName, tempDataSets));
			tempDataSets.delete_WholeList();

		} while (rowInTag.testWord_MatchesTerm(wordIterationNode->content));

		//check for required </rows> tag
		if (!loadedContent.validateWordNode_MatchesWord(rowsOutTag, wordIterationNode))
			return 34;

		//ensure there are the correct number of rows
		if (tempDataSetRows.get_ListLength() != dataSetRowCount)
			return 35;

		//check for required </fixedInputs> tag
		if (!loadedContent.validateWordNode_MatchesWord(fixedInputsOutTag, wordIterationNode))
			return 36;

		//the fixedInputs section of the file is valid
		fixedInputs = DataSetCollection(fixedInputsName, tempDataSetRows, tempDataSetColumns);

		//calculated input validation section

		//novel variables for calculated input validation
		Word calculatedInputsInTag("<calculatedInputs>");
		Word calculatedInputsOutTag("</calculatedInputs>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('i'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord calculatedInputInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('i'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord calculatedInputOutTag(tempConstructCodes);

		Word defaultValueInTag("<defaultValue>");
		Word defaultValueOutTag("</defaultValue>");

		//novel temp storage variables for calculated input validation
		double tempDefaultValue;
		UniformList<Input_Calculated> tempCalculatedInputsList;

		//check for required <calculatedInputs> tag
		if (!loadedContent.validateWordNode_MatchesWord(calculatedInputsInTag, wordIterationNode))
			return 37;

		//calculated input object construction loop
		while (calculatedInputInTag.testWord_MatchesTerm(wordIterationNode->content))
		{

			//check for required <ci_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(calculatedInputInTag, wordIterationNode))
				return 38;

			//check for required <name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameInTag, wordIterationNode))
				return 39;

			//capture required name alphanumeric Word
			if (!loadedContent.validateWordNode_CaptureAlphaNumeric(wordIterationNode, tempName))
				return 40;

			//check for required </name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameOutTag, wordIterationNode))
				return 41;

			//check for required <defaultValue> tag
			if (!loadedContent.validateWordNode_MatchesWord(defaultValueInTag, wordIterationNode))
				return 42;

			//capture required <defaultValue> value
			if (!loadedContent.validateWordNode_FloatingPoint_Unlimited(wordIterationNode, tempDefaultValue))
				return 43;

			//check for required </defaultValue> tag
			if (!loadedContent.validateWordNode_MatchesWord(defaultValueOutTag, wordIterationNode))
				return 44;

			//check for required </ci_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(calculatedInputOutTag, wordIterationNode))
				return 45;

			tempCalculatedInputsList.add_NewNode(Input_Calculated(tempDefaultValue, tempName));

		}

		//check for required </calculatedInputs> tag
		if (!loadedContent.validateWordNode_MatchesWord(calculatedInputsOutTag, wordIterationNode))
			return 46;

		if (tempCalculatedInputsList.get_ListLength() != calculatedInputsCount)
			return 47;

		tempInputCellCount += calculatedInputsCount;

		if (tempInputCellCount != inputCellCount)
			return 48;

		//the calculated inputs section of the file is valid
		calculatedInputs = tempCalculatedInputsList;

		//fixedInputs and calculatedInputs have been initialized
		return 0;

	}

	inline bool sub_Initialize_InterpretReturn_InputsValidation(
		const unsigned char fileValidationReturnCode)
	{

		cout << "\n";

		if (fileValidationReturnCode == 0)
		{

			cout << "fixedInputs and calculatedInputs file sections are valid";
			return true;

		}
			
		else
		{

			if (fileValidationReturnCode == 1)
				cout << "missing <fixedInputs> tag or incomplete file";
			else if (fileValidationReturnCode == 49)
				cout << "missing fixedInputs <name> tag or incomplete file";
			else if (fileValidationReturnCode == 50)
				cout << "missing fixedInputs <name> value or incomplete file";
			else if (fileValidationReturnCode == 51)
				cout << "missing fixedInputs </name> tag or incomplete file";
			else if (fileValidationReturnCode == 2)
				cout << "missing <columns> tag or incomplete file";
			else if (fileValidationReturnCode == 3)
				cout << "missing <c_#> tag or incomplete file";
			else if (fileValidationReturnCode == 4)
				cout << "missing column <name> tag or incomplete file";
			else if (fileValidationReturnCode == 5)
				cout << "missing column <name> value or incomplete file";
			else if (fileValidationReturnCode == 6)
				cout << "missing column </name> tag or incomplete file";
			else if (fileValidationReturnCode == 7)
				cout << "missing <frameLength> tag or incomplete file";
			else if (fileValidationReturnCode == 8)
				cout << "invalid <frameLength> value or incomplete file";
			else if (fileValidationReturnCode == 9)
				cout << "missing </frameLength> tag or incomplete file";
			else if (fileValidationReturnCode == 10)
				cout << "missing <shiftLength> tag or incomplete file";
			else if (fileValidationReturnCode == 11)
				cout << "invalid <shiftLength> value or incomplete file";
			else if (fileValidationReturnCode == 12)
				cout << "missing </shiftLength> tag or incomplete file";
			else if (fileValidationReturnCode == 13)
				cout << "missing </c_#> tag or incomplete file";
			else if (fileValidationReturnCode == 14)
				cout << "discrepant dataSetColumnCount and counted data set columns";
			else if (fileValidationReturnCode == 52)
				cout << "missing </columns> tag or incomplete file";
			else if (fileValidationReturnCode == 15)
				cout << "missing <rows> tag or incomplete file";
			else if (fileValidationReturnCode == 16)
				cout << "missing <r_#> tag or incomplete file";
			else if (fileValidationReturnCode == 17)
				cout << "missing row <name> tag or incomplete file";
			else if (fileValidationReturnCode == 18)
				cout << "missing row <name> value or incomplete file";
			else if (fileValidationReturnCode == 19)
				cout << "missing row </name> tag or incomplete file";
			else if (fileValidationReturnCode == 20)
				cout << "missing <dataSets> tag or incomplete file";
			else if (fileValidationReturnCode == 21)
				cout << "missing <ds_#> tag or incomplete file";
			else if (fileValidationReturnCode == 22)
				cout << "missing <dataPoints> tag or incomplete file";
			else if (fileValidationReturnCode == 23)
				cout << "missing <dp_#> tag or incomplete file";
			else if (fileValidationReturnCode == 24)
				cout << "invalid <dp_#> value or incomplete file";
			else if (fileValidationReturnCode == 25)
				cout << "missing </dp_#> tag or incomplete file";
			else if (fileValidationReturnCode == 26)
				cout << "missing </dataPoints> tag or incomplete file";
			else if (fileValidationReturnCode == 27)
				cout << "missing </ds_#> tag or incomplete file";
			else if (fileValidationReturnCode == 28)
				cout << "illegal structure: dataPoints listLength < (totalDataFrameCount * shiftLength) + frameLength";
			else if (fileValidationReturnCode == 29)
				cout << "illegal structure: totalDataFrameCount must = 1 when dataPoints listLength == frameLength ";
			else if (fileValidationReturnCode == 30)
				cout << "illegal structure: more DataSet objects contained in a row than there are columns";
			else if (fileValidationReturnCode == 31)
				cout << "illegal structure: fewer DataSet objects contained in a row than there are columns";
			else if (fileValidationReturnCode == 32)
				cout << "missing </dataSets> tag or incomplete file";
			else if (fileValidationReturnCode == 33)
				cout << "missing </r_#> tag or incomplete file";
			else if (fileValidationReturnCode == 34)
				cout << "missing </rows> tag or incomplete file";
			else if (fileValidationReturnCode == 35)
				cout << "discrepant dataSetRowCount and counted rows";
			else if (fileValidationReturnCode == 36)
				cout << "missing </fixedInputs> tag or incomplete file";
			else if (fileValidationReturnCode == 37)
				cout << "missing <calculatedInputs> tag or incomplete file";
			else if (fileValidationReturnCode == 38)
				cout << "missing <ci_#> tag or incomplete file";
			else if (fileValidationReturnCode == 39)
				cout << "missing calculated input <name> tag or incomplete file";
			else if (fileValidationReturnCode == 40)
				cout << "missing calculated input <name> value or incomplete file";
			else if (fileValidationReturnCode == 41)
				cout << "missing calculated input </name> tag or incomplete file";
			else if (fileValidationReturnCode == 42)
				cout << "missing <defaultValue> tag or incomplete file";
			else if (fileValidationReturnCode == 43)
				cout << "invalid <defaultValue> value or incomplete file";
			else if (fileValidationReturnCode == 44)
				cout << "missing </defaultValue> tag or incomplete file";
			else if (fileValidationReturnCode == 45)
				cout << "missing </ci_#> tag or incomplete file";
			else if (fileValidationReturnCode == 46)
				cout << "missing </calculatedInputs> tag or incomplete file";
			else if (fileValidationReturnCode == 47)
				cout << "discrepant calculatedInputsCount and counted calculated inputs";
			else if (fileValidationReturnCode == 48)
				cout << "discrepant inputCellCount and calculated number of required input cells";

			//reverse any possible partial initialization

			//numerical properties null states
			totalNetCount = 2;
			inputCellCount = 1;
			outputCellCount = 1;
			totalCellCount = 3;
			maxRoundsPerStimulate = 1;
			dataSetColumnCount = 1;
			dataSetRowCount = 1;
			totalDataFrameCount = 1;
			calculatedInputsCount = 0;

			//fixedInputs and calculatedInputs null states
			fixedInputs = DataSetCollection();
			calculatedInputs.resize_NoCopy(0);

			return false;

		}

	}

	/*
	checks the outputs section of loadedContent to ensure it matches
	the xml format described below (w/out whitespace);
	initializes outputs and returns 0 if the file section is valid,
	otherwise returns an error code for console reporting;
	<outputs>											//required
		<o_0>											//required (tag matches any <o_#>)
			<name>...</name>							//required (alphanumeric Word)
			<maxActivations>...</maxActivations>		//required (unsigned short >= 1)
		</o_0>											//required (tag matches any </o_#>)
		<o_1>											//optional
			...
		</o_1>
		...
	</outputs>											//required
	*/
	inline unsigned char sub_Initialize_Validate_Outputs(
		FileValidation_WordGroup &loadedContent, UniformNode<Word> *&wordIterationNode)
	{

		//novel variables for output validation
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;
		
		Word outputsInTag("<outputs>");
		Word outputsOutTag("</outputs>");

		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('o'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord outputInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('o'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord outputOutTag(tempConstructCodes);

		Word nameInTag("<name>");
		Word nameOutTag("</name>");
		Word maxActivationsInTag("<maxActivations>");
		Word maxActivationsOutTag("</maxActivations>");

		//storage variables
		Word tempName;
		unsigned short tempMaxActivations;
		UniformList<Output> tempOutputsList;

		//check for required <outputs> tag
		if (!loadedContent.validateWordNode_MatchesWord(outputsInTag, wordIterationNode))
			return 2;

		//output object construction loop
		do
		{

			//check for required <o_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(outputInTag, wordIterationNode))
				return 3;

			//check for required <name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameInTag, wordIterationNode))
				return 4;

			//capture required name alphanumeric Word
			if (!loadedContent.validateWordNode_CaptureAlphaNumeric(wordIterationNode, tempName))
				return 5;

			//check for required </name> tag
			if (!loadedContent.validateWordNode_MatchesWord(nameOutTag, wordIterationNode))
				return 6;

			//check for required <maxActivations> tag
			if (!loadedContent.validateWordNode_MatchesWord(maxActivationsInTag, wordIterationNode))
				return 7;

			//capture required maxActivations unsigned short (must be >= 1)
			if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, tempMaxActivations, 1))
				return 8;

			//check for required </maxActivations> tag
			if (!loadedContent.validateWordNode_MatchesWord(maxActivationsOutTag, wordIterationNode))
				return 9;

			//check for required </o_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(outputOutTag, wordIterationNode))
				return 10;

			//add a node to tempOutputs
			tempOutputsList.add_NewNode(Output(tempName, tempMaxActivations));

		} while (outputInTag.testWord_MatchesTerm(wordIterationNode->content));

		//check for required </outputs> tag
		if (!loadedContent.validateWordNode_MatchesWord(outputsOutTag, wordIterationNode))
			return 11;

		//check for required length
		if (tempOutputsList.get_ListLength() != outputCellCount)
			return 12;

		//this section of the file is valid
		outputs = tempOutputsList;

		//outputs has been initialized
		return 0;

	}

	inline bool sub_Initialize_InterpretReturn_OutputsValidation(
		const unsigned char fileValidationReturnCode)
	{

		cout << "\n";

		if (fileValidationReturnCode == 0)
		{

			cout << "outputs file section is valid";
			return true;

		}

		else
		{

			if (fileValidationReturnCode == 2)
				cout << "missing <outputs> tag or incomplete file";
			else if (fileValidationReturnCode == 3)
				cout << "missing <o_#> tag or incomplete file";
			else if (fileValidationReturnCode == 4)
				cout << "missing <name> tag or incomplete file";
			else if (fileValidationReturnCode == 5)
				cout << "missing <name> value or incomplete file";
			else if (fileValidationReturnCode == 6)
				cout << "missing </name> tag or incomplete file";
			else if (fileValidationReturnCode == 7)
				cout << "missing <maxActivations> tag or incomplete file";
			else if (fileValidationReturnCode == 8)
				cout << "invalid <maxActivations> value or incomplete file";
			else if (fileValidationReturnCode == 9)
				cout << "missing </maxActivations> tag or incomplete file";
			else if (fileValidationReturnCode == 10)
				cout << "missing </o_#> tag or incomplete file";
			else if (fileValidationReturnCode == 11)
				cout << "missing </outputs> tag or incomplete file";
			else if (fileValidationReturnCode == 12)
				cout << "discrepant outputCellCount and counted outputs";

			//reverse any possible partial initialization

			//numerical properties null states
			totalNetCount = 2;
			inputCellCount = 1;
			outputCellCount = 1;
			totalCellCount = 3;
			maxRoundsPerStimulate = 1;
			dataSetColumnCount = 1;
			dataSetRowCount = 1;
			totalDataFrameCount = 1;
			calculatedInputsCount = 0;

			//fixedInputs and calculatedInputs null states
			fixedInputs = DataSetCollection();
			calculatedInputs.resize_NoCopy(0);

			//outputs null state
			outputs.resize_NoCopy(1);

			return false;

		}

	}

	/*
	checks the calculatedInputUpdateExpressions and evaluationCriteriaExpressions
	sections of loadedContent to ensure they match the xml format described below
	(w/out whitespace); first initializes variablesList with Variable_Doubles
	corresponding to each fixed input column (first data frame of the first row initially),
	calculated input, and output; next initializes calculatedInputUpdateBlueprints
	and calculatedInputUpdateExpressions; returns 0 if the file section is valid,
	otherwise returns an error code for console reporting;
	<evaluationCriteriaExpressions>
		<ece_0>...</ece_0>							//required (tags match any <ece_#> and </ece_#>, content is an alphanumeric matching the name of any calculated input or output represented in variablesList
		...
	</evaluationCriteriaExpressions>				//required
	<calculatedInputUpdateExpressions>				//required
		<ciue_0>...</ciue_0>						//optional (tags match any <ciue_#> and </ciue_#>, content is an alphanumeric or alphanumeric indexed matching the name of any fixed input, calculated input, or output represented in variablesList
		...
	</calculatedInputUpdateExpressions>				//required
	*/
	inline unsigned char sub_Initialize_Validate_Expressions(
		FileValidation_WordGroup &loadedContent, UniformNode<Word> *&wordIterationNode)
	{

		//partially initialize variablesList;
		variablesList.delete_WholeList();

		unsigned short count;
		unsigned short frameLength;
		unsigned short frameSubIndex;
		Word columnName;
		Word subFrameIndexWord;
		UniformNode<char> *charNode;
		Output *currentOutput;
		Input_Calculated *currentCalculatedInput;

		for (count = 0; count < calculatedInputsCount; count++)
		{

			currentCalculatedInput = calculatedInputs.get_ElementAddress(count);

			variablesList.add_NewNode(Variable_Double(currentCalculatedInput->get_Name(),
				currentCalculatedInput->get_WorkingValueAddress()));

		}

		for (count = 0; count < outputCellCount; count++)
		{

			currentOutput = outputs.get_ElementAddress(count);

			variablesList.add_NewNode(Variable_Double(currentOutput->get_Name(),
				currentOutput->get_ValueAddress()));

		}

		//novel variables for calculatedInputUpdateExpressions population
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;

		Word evaluationCriteriaExpressionsInTag("<evaluationCriteriaExpressions>");
		Word evaluationCriteriaExpressionsOutTag("</evaluationCriteriaExpressions>");

		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('e'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('e'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord evaluationCriteriaExpressionInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('e'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('e'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord evaluationCriteriaExpressionOutTag(tempConstructCodes);

		Word calculatedInputUpdateExpressionsInTag("<calculatedInputUpdateExpressions>");
		Word calculatedInputUpdateExpressionsOutTag("</calculatedInputUpdateExpressions>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('i'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('u'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('e'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord calculatedInputUpdateExpressionInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('i'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('u'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('e'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord calculatedInputUpdateExpressionOutTag(tempConstructCodes);

		//storage variables
		UniformList<Expression_Double> tempExpressions;
		UniformNode<Expression_Double> *tempExpressionNode;
		UniformNode<Variable_Double> *variableNode;

		//main body of validation logic

		//check for required <evaluationCriteriaExpressions> tag
		if (!loadedContent.validateWordNode_MatchesWord(evaluationCriteriaExpressionsInTag, wordIterationNode))
			return 1;

		//evaluationCriteriaExpressions initialization loop
		while (evaluationCriteriaExpressionInTag.testWord_MatchesTerm(wordIterationNode->content))
		{

			//check for required <ece_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(evaluationCriteriaExpressionInTag, wordIterationNode))
				return 2;

			tempExpressionNode = tempExpressions.add_NewNode();

			//check for valid expression using only variables contained in variablesList
			if (!tempExpressionNode->content.initialize(wordIterationNode->content, variablesList))
				return 3;

			wordIterationNode = wordIterationNode->get_NextNode();

			if (!wordIterationNode)
				return 4;

			//check for required </ece_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(evaluationCriteriaExpressionOutTag, wordIterationNode))
				return 5;

		}

		//check for required </evaluationCriteriaExpressions> tag
		if (!loadedContent.validateWordNode_MatchesWord(evaluationCriteriaExpressionsOutTag, wordIterationNode))
			return 6;

		//check whether the correct number of evaluation criteria expressions are represented
		if (tempExpressions.get_ListLength() != dataSetRowCount)
			return 7;

		//this sections of the file is valid, initialize calculatedInputUpdateExpressions
		evaluationCriteriaExpressions = tempExpressions;

		tempExpressions.delete_WholeList();

		//complete initialization of variables list
		for (count = 0; count < dataSetColumnCount; count++)
		{

			frameLength = fixedInputs.get_ColumnFrameLength(count);
			columnName = fixedInputs.get_ColumnName(count);

			for (frameSubIndex = 0; frameSubIndex < frameLength; frameSubIndex++)
			{

				//set up subFrameIndexWord to correspond to this subframe index
				//in the format "[#]" then add subFrameIndexWord to the end of
				//columnName and create a new Variable_Double corresponding to the combined word
				subFrameIndexWord.add_NewNode('[');
				subFrameIndexWord.transcribe_UnsignedShort(frameSubIndex);
				subFrameIndexWord.add_NewNode(']');

				columnName.add_CutWholeList(subFrameIndexWord);

				variablesList.add_NewNode(Variable_Double(columnName,
					fixedInputs.get_FrameAdjustedDataPointAddress(0, count, 0, frameSubIndex)));

				//***TESTING***
				cout << "\n";
				columnName.output_Console();
				//***TESTING***

				charNode = columnName.get_LastNode();

				do
				{

					charNode = charNode->get_PriorNode();

				} while (charNode->content != '[');

				columnName.delete_Range(charNode, columnName.get_LastNode());

			}

		}

		//check for required <calculatedInputUpdateExpressions> tag
		if (!loadedContent.validateWordNode_MatchesWord(calculatedInputUpdateExpressionsInTag, wordIterationNode))
			return 8;

		//calculatedInputUpdateExpressions initialization loop
		while (calculatedInputUpdateExpressionInTag.testWord_MatchesTerm(wordIterationNode->content))
		{

			//check for required <ciue_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(calculatedInputUpdateExpressionInTag, wordIterationNode))
				return 9;

			tempExpressionNode = tempExpressions.add_NewNode();

			//check for valid expression using only variables contained in variablesList
			if (!tempExpressionNode->content.initialize(wordIterationNode->content, variablesList))
				return 10;

			wordIterationNode = wordIterationNode->get_NextNode();

			if (!wordIterationNode)
				return 11;

			//check for required </ciue_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(calculatedInputUpdateExpressionOutTag, wordIterationNode))
				return 12;

		}

		//check for required </calculatedInputUpdateExpressions> tag
		if (!loadedContent.validateWordNode_MatchesWord(calculatedInputUpdateExpressionsOutTag, wordIterationNode))
			return 13;

		//check whether the correct number of calculated input update expressions are represented
		if (tempExpressions.get_ListLength() != calculatedInputsCount)
			return 14;

		//this sections of the file is valid, initialize calculatedInputUpdateExpressions
		calculatedInputUpdateExpressions = tempExpressions;

		//populate updatingFixedInputsVariables
		updatingFixedInputsVariables.delete_WholeList();
		variableNode = variablesList.get_LastNode();

		for (count = (dataSetColumnCount - 1); count > 0; count--)
		{

			frameLength = fixedInputs.get_ColumnFrameLength(count);

			for (frameSubIndex = (frameLength - 1); frameSubIndex > 0; frameSubIndex--)
			{

				if (variableNode->content.get_Utilized())
					updatingFixedInputsVariables.add_NewNode(FixedInputReference(
						&(variableNode->content), frameSubIndex, count), NULL, false);

				variableNode = variableNode->get_PriorNode();

			}

			if (variableNode->content.get_Utilized())
				updatingFixedInputsVariables.add_NewNode(FixedInputReference(
					&(variableNode->content), frameSubIndex, count), NULL, false);

			variableNode = variableNode->get_PriorNode();

		}

		frameLength = fixedInputs.get_ColumnFrameLength(count);

		for (frameSubIndex = (frameLength - 1); frameSubIndex > 0; frameSubIndex--)
		{

			if (variableNode->content.get_Utilized())
				updatingFixedInputsVariables.add_NewNode(FixedInputReference(
					&(variableNode->content), frameSubIndex, count), NULL, false);

			variableNode = variableNode->get_PriorNode();

		}

		if (variableNode->content.get_Utilized())
			updatingFixedInputsVariables.add_NewNode(FixedInputReference(
				&(variableNode->content), frameSubIndex, count), NULL, false);

		//evaluationCriteriaExpressions, calculatedInputUpdateExpressions, variablesList,
		//and updatingFixedInputsVariables have been initialized
		return 0;

	}

	inline bool sub_Initialize_InterpretReturn_ExpressionsValidation(
		const unsigned char fileValidationReturnCode)
	{

		cout << "\n";

		if (fileValidationReturnCode == 0)
		{

			cout << "calculated input update and evaluation criteria expressions file sections are valid";
			return true;

		}

		else
		{

			if (fileValidationReturnCode == 1)
				cout << "missing <evaluationCriteriaExpressions> tag or incomplete file";
			else if (fileValidationReturnCode == 2)
				cout << "missing <ece_#> tag or incomplete file";
			else if (fileValidationReturnCode == 3)
				cout << "syntax of an evaluation criteria expression is invalid or refers to an illegal variable";
			else if (fileValidationReturnCode == 4)
				cout << "incomplete file";
			else if (fileValidationReturnCode == 5)
				cout << "missing </ece_#> tag or incomplete file";
			else if (fileValidationReturnCode == 6)
				cout << "missing </evaluationCriteriaExpressions> tag or incomplete file";
			else if (fileValidationReturnCode == 7)
				cout << "discrepant dataSetRowCount and counted evaluation criteria expressions";
			else if (fileValidationReturnCode == 8)
				cout << "missing <calculatedInputUpdateExpressions> tag or incomplete file";
			else if (fileValidationReturnCode == 9)
				cout << "missing <ciue_#> tag or incomplete file";
			else if (fileValidationReturnCode == 10)
				cout << "syntax of an calculate input update expression is invalid or refers to an illegal variable";
			else if (fileValidationReturnCode == 11)
				cout << "incomplete file";
			else if (fileValidationReturnCode == 12)
				cout << "missing </ciue_#> tag or incomplete file";
			else if (fileValidationReturnCode == 13)
				cout << "missing </calculatedInputUpdateExpressions> tag or incomplete file";
			else if (fileValidationReturnCode == 14)
				cout << "discrepant calculatedInputsCount and counted calculated input update expressions";

			//reverse any possible partial initialization

			//numerical properties null states
			totalNetCount = 2;
			inputCellCount = 1;
			outputCellCount = 1;
			totalCellCount = 3;
			maxRoundsPerStimulate = 1;
			dataSetColumnCount = 1;
			dataSetRowCount = 1;
			totalDataFrameCount = 1;
			calculatedInputsCount = 0;

			//fixedInputs and calculatedInputs null states
			fixedInputs = DataSetCollection();
			calculatedInputs.resize_NoCopy(0);

			//outputs null state
			outputs.resize_NoCopy(1);

			//variablesList, updatingFixedInputsVariables, evaluationCriteraExpressions
			//and calculatedInputUpdateExpressions null states
			variablesList.delete_WholeList();
			updatingFixedInputsVariables.delete_WholeList();
			evaluationCriteriaExpressions.resize_NoCopy(1);
			calculatedInputUpdateExpressions.resize_NoCopy(0);

			return false;

		}

	}

	/*
	checks the cell controls section of loadedContent to ensure it
	matches the xml format described below (w/out whitespace); initializes
	//cellControls and trackers then returns 0 if the file section is valid,
	otherwise returns an error code for console reporting;
	<cellControlGroups>												//required
		<ccg_0>														//required (tag matches any <ccg_#>)
			<groupLength>...</groupLength>										//required (unsigned short >= 1)
			<linksCoeffCenter>...</linksCoeffCenter>				//required (double > 0.0)
			<internalSpread>...</internalSpread>					//required (double > 0.0)
			<broadcastSpread>...</broadcastSpread>					//required (double > 0.0)
			<linksCoeffSpread>...</linksCoeffSpread>				//required (double > 0.0)
			<decayRateSpread>...</decayRateSpread>					//required (double > 0.0)
			<refractoryPeriodSpread>...</refractoryPeriodSpread>	//required (unsigned char >= 1)
			<broadcastSwitchSignFreq>...</broadcastSwitchSignFreq>	//required (double >= 0.0, <= 1.0)
			<linksSwitchSignFreq>...</linksSwitchSignFreq>			//required (double >= 0.0, <= 1.0)
		</ccg_0>													//required (tag matches any </ccg_#>)
		<ccg_1>														//optional
			...
		</ccg_1>
		...
	</cellControlGroups>
	*/
	inline unsigned char sub_Initialize_ValidateCellControls(
		FileValidation_WordGroup &loadedContent, UniformNode<Word> *&wordIterationNode)
	{

		//variables for validation
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;
		
		Word cellControlGroupsInTag("<cellControlGroups>");
		Word cellControlGroupsOutTag("</cellControlGroups>");

		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("c", 0, false, 2, 2));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('g'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord cellControlGroupInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("c", 0, false, 2, 2));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('g'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord cellControlGroupOutTag(tempConstructCodes);

		Word groupLengthInTag("<groupLength>");
		Word groupLengthOutTag("</groupLength>");
		Word linksCoeffCenterInTag("<linksCoeffCenter>");
		Word linksCoeffCenterOutTag("</linksCoeffCenter>");
		Word internalSpreadInTag("<internalSpread>");
		Word internalSpreadOutTag("</internalSpread>");
		Word broadcastSpreadInTag("<broadcastSpread>");
		Word broadcastSpreadOutTag("</broadcastSpread>");
		Word linksCoeffSpreadInTag("<linksCoeffSpread>");
		Word linksCoeffSpreadOutTag("</linksCoeffSpread>");
		Word decayRateSpreadInTag("<decayRateSpread>");
		Word decayRateSpreadOutTag("</decayRateSpread>");
		Word refractoryPeriodSpreadInTag("<refractoryPeriodSpread>");
		Word refractoryPeriodSpreadOutTag("</refractoryPeriodSpread>");	
		Word broadcastSwitchSignFreqInTag("<broadcastSwitchSignFreq>");
		Word broadcastSwitchSignFreqOutTag("</broadcastSwitchSignFreq>");
		Word linksSwitchSignFreqInTag("<linksSwitchSignFreq>");
		Word linksSwitchSignFreqOutTag("</linksSwitchSignFreq>");

		//storage variables
		unsigned short groupLength;
		double linksCoeffCenter;
		double internalSpread;
		double broadcastSpread;
		double linksCoeffSpread;
		double decayRateSpread;
		unsigned char refractoryPeriodSpread;
		double broadcastSwitchSignFreq;
		double linksSwitchSignFreq;
		UniformList<CellControl> tempCellControlsList;
		UniformNode<CellControl> *cellControlNode;

		unsigned short tempTotalCellCount = 0;

		//main body of validation logic

		//check for required <cellControlGroups> tag
		if (!loadedContent.validateWordNode_MatchesWord(cellControlGroupsInTag, wordIterationNode))
			return 2;

		//output object construction loop
		do
		{

			//check for required <ccg_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(cellControlGroupInTag, wordIterationNode))
				return 3;

			//check for required <groupLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(groupLengthInTag, wordIterationNode))
				return 31;

			//capture required groupLength unsigned short (>= 1)
			if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, groupLength, 1))
				return 32;

			//check for required </groupLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(groupLengthOutTag, wordIterationNode))
				return 33;

			//check for required <linksCoeffCenter> tag
			if (!loadedContent.validateWordNode_MatchesWord(linksCoeffCenterInTag, wordIterationNode))
				return 4;

			//capture required linksCoeffCenter double (any value)
			if (!loadedContent.validateWordNode_FloatingPoint_Unlimited(wordIterationNode, linksCoeffCenter))
				return 5;

			//check for required </linksCoeffCenter> tag
			if (!loadedContent.validateWordNode_MatchesWord(linksCoeffCenterOutTag, wordIterationNode))
				return 6;

			//check for required <internalSpread> tag
			if(!loadedContent.validateWordNode_MatchesWord(internalSpreadInTag, wordIterationNode))
				return 7;

			//capture required internalSpread double (must be > 0.0)
			if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode, internalSpread, true, true))
				return 8;

			//check for required </internalSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(internalSpreadOutTag, wordIterationNode))
				return 9;

			//check for required <broadcastSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(broadcastSpreadInTag, wordIterationNode))
				return 10;

			//capture required broadcastSpread double (must be > 0.0)
			if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode, broadcastSpread, true, true))
				return 1;

			//check for required </broadcastSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(broadcastSpreadOutTag, wordIterationNode))
				return 12;

			//check for required <linksCoeffSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(linksCoeffSpreadInTag, wordIterationNode))
				return 13;

			//capture required linksCoeffSpread double (must be > 0)
			if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode, linksCoeffSpread, true, true))
				return 14;

			//check for required </linksCoeffSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(linksCoeffSpreadOutTag, wordIterationNode))
				return 15;

			//check for required <decayRateSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(decayRateSpreadInTag, wordIterationNode))
				return 16;

			//capture required decayRateSpread double (must be > 0)
			if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode, decayRateSpread, true, true))
				return 17;

			//check for required </decayRateSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(decayRateSpreadOutTag, wordIterationNode))
				return 18;

			//check for required <refractoryPeriodSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(refractoryPeriodSpreadInTag, wordIterationNode))
				return 19;

			//capture required refractoryPeriodSpread unsigned char (>= 1)
			if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, refractoryPeriodSpread, 1))
				return 20;

			//check for required </refractoryPeriodSpread> tag
			if (!loadedContent.validateWordNode_MatchesWord(refractoryPeriodSpreadOutTag, wordIterationNode))
				return 21;

			//check for required <broadcastSwitchSignFreq> tag
			if (!loadedContent.validateWordNode_MatchesWord(broadcastSwitchSignFreqInTag, wordIterationNode))
				return 22;

			//capture required broadcastSwitchSignFreq double (>=0 and <= 1.0)
			if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode,
				broadcastSwitchSignFreq, false, true, 0.0, true, 1.0))
				return 23;

			//check for required </broadcastSwitchSignFreq> tag
			if (!loadedContent.validateWordNode_MatchesWord(broadcastSwitchSignFreqOutTag, wordIterationNode))
				return 24;

			//check for required <linksSwitchSignFreq> tag
			if (!loadedContent.validateWordNode_MatchesWord(linksSwitchSignFreqInTag, wordIterationNode))
				return 25;

			//capture required linksSwitchSignFreq double (>=0 and <= 1.0)
			if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode,
				linksSwitchSignFreq, false, true, 0.0, true, 1.0))
				return 26;

			//check for required </linksSwitchSignFreq> tag
			if (!loadedContent.validateWordNode_MatchesWord(linksSwitchSignFreqOutTag, wordIterationNode))
				return 27;

			//check for required associated </ccg_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(cellControlGroupOutTag, wordIterationNode))
				return 28;

			//update tempTotalCellCount
			tempTotalCellCount += groupLength;

			//add/initialize node(s) to tempCellControlsList
			cellControlNode = tempCellControlsList.add_NewRange(groupLength, NULL, true, false);

			do
			{

				cellControlNode->content = CellControl(linksCoeffCenter, internalSpread,
					broadcastSpread, linksCoeffSpread, decayRateSpread, refractoryPeriodSpread,
					broadcastSwitchSignFreq, linksSwitchSignFreq);

				cellControlNode = cellControlNode->get_NextNode();

			} while (cellControlNode);

		} while (cellControlGroupInTag.testWord_MatchesTerm(wordIterationNode->content));

		//check for required </cellControlGroups> tag
		if (!loadedContent.validateWordNode_MatchesWord(cellControlGroupsOutTag, wordIterationNode))
			return 29;

		//check for required totalCellCount value
		if (tempTotalCellCount != totalCellCount)
			return 30;

		//this section of the file is valid
		cellControls = tempCellControlsList;
		trackers.resize_NoCopy(totalCellCount);

		//cellControls and trackers have been initialized
		return 0;

	}

	inline bool sub_Initialize_InterpretReturn_CellControlsValidation(
		const unsigned char fileValidationReturnCode)
	{

		cout << "\n";

		if (fileValidationReturnCode == 0)
		{

			cout << "cell controls file section is valid";
			return true;

		}

		else
		{

			if (fileValidationReturnCode == 2)
				cout << "missing <cellControlGroups> tag or incomplete file";
			else if (fileValidationReturnCode == 3)
				cout << "missing <ccg_#> tag or incomplete file";
			else if (fileValidationReturnCode == 31)
				cout << "missing <groupLength> tag or incomplete file";
			else if (fileValidationReturnCode == 32)
				cout << "invalid <groupLength> value or incomplete file";
			else if (fileValidationReturnCode == 33)
				cout << "missing </groupLength> tag or incomplete file";
			else if (fileValidationReturnCode == 4)
				cout << "missing <linksCoeffCenter> tag or incomplete file";
			else if (fileValidationReturnCode == 5)
				cout << "invalid <linksCoeffCenter> value or incomplete file";
			else if (fileValidationReturnCode == 6)
				cout << "missing </linksCoeffCenter> tag or incomplete file";
			else if (fileValidationReturnCode == 7)
				cout << "missing <internalSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 8)
				cout << "invalid <internalSpread> value or incomplete file";
			else if (fileValidationReturnCode == 9)
				cout << "missing </internalSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 10)
				cout << "missing <broadcastSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 11)
				cout << "invalid <broadcastSpread> value or incomplete file";
			else if (fileValidationReturnCode == 12)
				cout << "missing </broadcastSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 13)
				cout << "missing <linksCoeffSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 14)
				cout << "invalid <linksCoeffSpread> value or incomplete file";
			else if (fileValidationReturnCode == 15)
				cout << "missing </linksCoeffSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 16)
				cout << "missing <decayRateSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 17)
				cout << "invalid <decayRateSpread> value or incomplete file";
			else if (fileValidationReturnCode == 18)
				cout << "missing </decayRateSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 19)
				cout << "missing <refractoryPeriodSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 20)
				cout << "invalid <refractoryPeriodSpread> value or incomplete file";
			else if (fileValidationReturnCode == 21)
				cout << "missing </refractoryPeriodSpread> tag or incomplete file";
			else if (fileValidationReturnCode == 22)
				cout << "missing <broadcastSwitchSignFreq> tag or incomplete file";
			else if (fileValidationReturnCode == 23)
				cout << "invalid <broadcastSwitchSignFreq> value or incomplete file";
			else if (fileValidationReturnCode == 24)
				cout << "missing </broadcastSwitchSignFreq> tag or incomplete file";
			else if (fileValidationReturnCode == 25)
				cout << "missing <linksSwitchSignFreq> tag or incomplete file";
			else if (fileValidationReturnCode == 26)
				cout << "invalid <linksSwitchSignFreq> value or incomplete file";
			else if (fileValidationReturnCode == 27)
				cout << "missing </linksSwitchSignFreq> tag or incomplete file";
			else if (fileValidationReturnCode == 28)
				cout << "missing </ccg_#> tag or incomplete file";
			else if (fileValidationReturnCode == 29)
				cout << "missing </cellControlGroups> tag";
			else if (fileValidationReturnCode == 30)
				cout << "discrepant totalCellCount and counted cell controls";

			//reverse any possible partial initialization

			//numerical properties null states
			totalNetCount = 2;
			inputCellCount = 1;
			outputCellCount = 1;
			totalCellCount = 3;
			maxRoundsPerStimulate = 1;
			dataSetColumnCount = 1;
			dataSetRowCount = 1;
			totalDataFrameCount = 1;
			calculatedInputsCount = 0;

			//fixedInputs and calculatedInputs null states
			fixedInputs = DataSetCollection();
			calculatedInputs.resize_NoCopy(0);

			//outputs null state
			outputs.resize_NoCopy(1);

			//variablesList, updatingFixedInputsVariables, evaluationCriteraExpressions
			//and calculatedInputUpdateExpressions null states
			variablesList.delete_WholeList();
			updatingFixedInputsVariables.delete_WholeList();
			evaluationCriteriaExpressions.resize_NoCopy(1);
			calculatedInputUpdateExpressions.resize_NoCopy(0);

			//cellControls and trackers null states
			cellControls.resize_NoCopy(3);
			trackers.resize_NoCopy(3);

			return false;

		}

	}

	/*
	checks the nets and terminal sections of loadedContent to ensure they
	matches the xml format described below (w/out whitespace); initializes
	//nets then returns 0 if the file section is valid, otherwise returns
	an error code for console reporting
		<netGroups>															//required
			<ng_0>															//required
				<groupLength>...</groupLength>								//required unsigned short >= 1
				<c_0>														//required
					<internalCoeff>...</internalCoeff>						//required double >= 0.0
					<broadcastCoeff>...</broadcastCoeff>					//required double any value
					<decayRate>...</decayRate>								//required double >= 0.0, <= 1.0
					<refractoryPeriod>...</refractoryPeriod>				//required unsigned char > 0
					<postLinks>												//required
						<pl_0>												//required
							<postCellIndex>...</postCellIndex>				//unsigned short < totalCellCount
							<linkCoeff>...</linkCoeff>						//required double any value			
						</pl_0>												//required
						<pl_1>												//optional
							...
						</pl_1>
						...
					</postLinks>											//required
				</c_0>														//required
				<c_1>														//optional
					...
				</c_1>
				...

			</ng_0>															//required
			<ng_1>															//optional
			...
			</ng_1>
			...
		</netGroups>														//required
	</Population>															//required
	*/
	inline unsigned char sub_Initialize_ValidateNetsAndTerminal(
		FileValidation_WordGroup &loadedContent, UniformNode<Word> *&wordIterationNode)	
	{

		//variables for validation
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;

		Word netGroupsInTag("<netGroups>");
		Word netGroupsOutTag("</netGroups>");

		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('n'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('g'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord netGroupInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('n'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('g'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord netGroupOutTag(tempConstructCodes);

		Word groupLengthInTag("<groupLength>");
		Word groupLengthOutTag("</groupLength>");
		Word cellsInTag("<cells>");
		Word cellsOutTag("</cells>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord cellInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('c'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord cellOutTag(tempConstructCodes);

		Word internalCoeffInTag("<internalCoeff>");
		Word internalCoeffOutTag("</internalCoeff>");
		Word broadcastCoeffInTag("<broadcastCoeff>");
		Word broadcastCoeffOutTag("</broadcastCoeff>");
		Word decayRateInTag("<decayRate>");
		Word decayRateOutTag("</decayRate>");
		Word refractoryPeriodInTag("<refractoryPeriod>");
		Word refractoryPeriodOutTag("</refractoryPeriod>");
		Word postLinksInTag("<postLinks>");
		Word postLinksOutTag("</postLinks>");

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('p'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('l'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord postLinkInTag(tempConstructCodes);

		tempConstructCodes.delete_WholeList();
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('/'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('p'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('l'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('_'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("09", 1, false, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord postLinkOutTag(tempConstructCodes);

		Word postCellIndexInTag("<postCellIndex>");
		Word postCellIndexOutTag("</postCellIndex>");
		Word linkCoeffInTag("<linkCoeff>");
		Word linkCoeffOutTag("</linkCoeff>");
		Word populationOutTag("</Population>");

		//storage variables
		unsigned short groupLength;
		double internalCoeff;
		double broadcastCoeff;
		double decayRate;
		unsigned char refractoryPeriod;
		unsigned short tempPostCellIndex;
		int priorTempPostCellIndex;
		double tempLinkCoeff;
		UniformList<Link> postLinksList;
		ExtensibleArray<bool> cellPriorLinksCounts(totalCellCount);
		ExtensibleArray<unsigned short> netPriorLinksCounts(totalCellCount);
		UniformList<Cell> tempCellsList;
		unsigned short cellCount;
		unsigned short secondCellCount;
		UniformList<Net> tempNetsList;
		UniformNode<Net> *netNode;

		//initialize the netPriorLinksCounts list
		for (cellCount = 0; cellCount < totalCellCount; cellCount++)
			*(netPriorLinksCounts.get_ElementAddress(cellCount)) = 0;			

		//main body of file validation logic

		//check for required <netGroups> tag
		if (!loadedContent.validateWordNode_MatchesWord(netGroupsInTag, wordIterationNode))
			return 2;

		//net object construction loop
		do
		{

			//check for required <ng_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(netGroupInTag, wordIterationNode))
				return 3;

			//check for required <groupLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(groupLengthInTag, wordIterationNode))
				return 38;

			//capture required <groupLength> unsigned int (>= 1)
			if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, groupLength, 1))
				return 39;

			//check for required </groupLength> tag
			if (!loadedContent.validateWordNode_MatchesWord(groupLengthOutTag, wordIterationNode))
				return 40;

			//check for required <cells> tag
			if (!loadedContent.validateWordNode_MatchesWord(cellsInTag, wordIterationNode))
				return 4;

			cellCount = 0;
			
			do
			{

				priorTempPostCellIndex = -1;

				//check for required <c_#> tag
				if (!loadedContent.validateWordNode_MatchesTerm(cellInTag, wordIterationNode))
					return 5;

				//check for required <internalCoeff> tag
				if (!loadedContent.validateWordNode_MatchesWord(internalCoeffInTag, wordIterationNode))
					return 6;

				//capture required <internalCoeff> double (>= 0.0)
				if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode, internalCoeff, false, true, 0.0))
					return 7;

				//check for required </internalCoeff> tag
				if (!loadedContent.validateWordNode_MatchesWord(internalCoeffOutTag, wordIterationNode))
					return 8;

				//check for required <broadcastCoeff> tag
				if (!loadedContent.validateWordNode_MatchesWord(broadcastCoeffInTag, wordIterationNode))
					return 9;

				//capture required <broadcastCoeff> double (any value)
				if (!loadedContent.validateWordNode_FloatingPoint_Unlimited(wordIterationNode, broadcastCoeff))
					return 10;

				//check for required </broadcastCoeff> tag
				if (!loadedContent.validateWordNode_MatchesWord(broadcastCoeffOutTag, wordIterationNode))
					return 11;

				//check for required <decayRate> tag
				if (!loadedContent.validateWordNode_MatchesWord(decayRateInTag, wordIterationNode))
					return 12;

				//capture required <decayRate> double (>= 0.0, <= 1.0)
				if (!loadedContent.validateWordNode_FloatingPoint_Limited(wordIterationNode,
					decayRate, false, true, 0.0, true, 1.0))
					return 13;

				//check for required </decayRate> tag
				if (!loadedContent.validateWordNode_MatchesWord(decayRateOutTag, wordIterationNode))
					return 14;

				//check for required <refractoryPeriod> tag
				if (!loadedContent.validateWordNode_MatchesWord(refractoryPeriodInTag, wordIterationNode))
					return 15;

				//capture required <refractoryPeriod> unsigned char (> 0)
				if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, refractoryPeriod, 1))
					return 16;

				//check for required </refractoryPeriod> tag
				if (!loadedContent.validateWordNode_MatchesWord(refractoryPeriodOutTag, wordIterationNode))
					return 17;

				//check for required <postLinks> tag
				if (!loadedContent.validateWordNode_MatchesWord(postLinksInTag, wordIterationNode))
					return 18;

				//reset the cellPriorLinksCounts list
				for (secondCellCount = 0; secondCellCount < totalCellCount; secondCellCount++)
					*(cellPriorLinksCounts.get_ElementAddress(secondCellCount)) = false;

				do
				{

					//check for required <pl_#> tag
					if (!loadedContent.validateWordNode_MatchesTerm(postLinkInTag, wordIterationNode))
						return 19;

					//check for required <postCellIndex> tag
					if (!loadedContent.validateWordNode_MatchesWord(postCellIndexInTag, wordIterationNode))
						return 20;

					//capture required <postCellIndex> unsigned short (>= 0, < totalCellCount)
					if (!loadedContent.validateWordNode_UnsignedInt_Limited(wordIterationNode, tempPostCellIndex, 0, true, (totalCellCount - 1)))
						return 21;

					//ensure postCellIndex is not this cell's index, is not a duplicate index
					//within this postLinks list, and is in correct ascending list order;
					//increment the index's netPriorCellLinks tracking count
					if (tempPostCellIndex == cellCount)
						return 22;

					else
					{

						if (!cellPriorLinksCounts.get_Element(tempPostCellIndex))
						{

							if (tempPostCellIndex > priorTempPostCellIndex)
							{

								*(cellPriorLinksCounts.get_ElementAddress(tempPostCellIndex)) = true;
								priorTempPostCellIndex = tempPostCellIndex;

								*(netPriorLinksCounts.get_ElementAddress(tempPostCellIndex)) += 1;

							}

							//the index is not in correct ascending list order
							else
								return 23;

						}

						//the index is duplicated in this postLinks list
						else
							return 24;

					}

					//check for required </postCellIndex> tag
					if (!loadedContent.validateWordNode_MatchesWord(postCellIndexOutTag, wordIterationNode))
						return 25;

					//check for required <linkCoeff> tag
					if (!loadedContent.validateWordNode_MatchesWord(linkCoeffInTag, wordIterationNode))
						return 26;

					//capture required <linkCoeff> double (any value)
					if (!loadedContent.validateWordNode_FloatingPoint_Unlimited(wordIterationNode, tempLinkCoeff))
						return 27;

					//check for required </linkCoeff> tag
					if (!loadedContent.validateWordNode_MatchesWord(linkCoeffOutTag, wordIterationNode))
						return 28;

					//check for required </pl_#> tag
					if (!loadedContent.validateWordNode_MatchesTerm(postLinkOutTag, wordIterationNode))
						return 29;

					//add the new link
					postLinksList.add_NewNode(Link(tempPostCellIndex, tempLinkCoeff));

				} while (postLinkInTag.testWord_MatchesTerm(wordIterationNode->content));

				//check for required </postLinks> tag
				if (!loadedContent.validateWordNode_MatchesWord(postLinksOutTag, wordIterationNode))
					return 30;

				//check for required </c_#> tag
				if (!loadedContent.validateWordNode_MatchesTerm(cellOutTag, wordIterationNode))
					return 31;

				//add the new cell to the temp list
				tempCellsList.add_NewNode(Cell(internalCoeff, broadcastCoeff,
					decayRate, refractoryPeriod, postLinksList));
				cellCount++;

			} while (cellInTag.testWord_MatchesTerm(wordIterationNode->content));

			//ensure that the correct number of cells has been created
			if (cellCount != totalCellCount)
				return 32;

			//check for required </cells> tag
			if (!loadedContent.validateWordNode_MatchesWord(cellsOutTag, wordIterationNode))
				return 33;

			//check for required </ng_#> tag
			if (!loadedContent.validateWordNode_MatchesTerm(netGroupOutTag, wordIterationNode))
				return 34;

			//add the new net(s) to tempNetsList
			netNode = tempNetsList.add_NewRange(groupLength, NULL, true, false);

			do
			{

				netNode->content = Net(tempCellsList, netPriorLinksCounts, inputCellCount, totalCellCount);

				netNode = netNode->get_NextNode();

			} while (netNode);

			//reset the temp tracking variables
			tempCellsList.delete_WholeList();

			//reset the netPriorLinksCounts list
			for (cellCount = 0; cellCount < totalCellCount; cellCount++)
				*(netPriorLinksCounts.get_ElementAddress(cellCount)) = 0;

		} while (netGroupInTag.testWord_MatchesTerm(wordIterationNode->content));

		//ensure that the correct number of nets is represented
		if (tempNetsList.get_ListLength() != totalNetCount)
			return 35;

		//check for required </netGroups> tag
		if (!loadedContent.validateWordNode_MatchesWord(netGroupsOutTag, wordIterationNode))
			return 36;

		//check for final required </Population> tag
		if (!loadedContent.validateWordNode_MatchesWord(populationOutTag, wordIterationNode, false, false))
			return 37;

		//this section of the file is valid
		nets = tempNetsList;

		//totalNetCount and nets have been initialized
		return 0;

	}

	inline bool sub_Initialize_InterpretReturn_NetsAndTerminalValidation(
		const unsigned char fileValidationReturnCode)
	{

		cout << "\n";

		if (fileValidationReturnCode == 0)
		{

			cout << "nets file section is valid";
			return true;

		}

		else
		{

			if (fileValidationReturnCode == 2)
				cout << "missing <netGroups> tag or incomplete file";
			else if (fileValidationReturnCode == 3)
				cout << "missing <ng_#> tag or incomplete file";
			else if (fileValidationReturnCode == 38)
				cout << "missing <groupLength> tag or incomplete file";
			else if (fileValidationReturnCode == 39)
				cout << "invalid <groupLength> value or incomplete file";
			else if (fileValidationReturnCode == 40)
				cout << "missing </groupLength> tag or incomplete file";
			else if (fileValidationReturnCode == 4)
				cout << "missing <cells> tag or incomplete file";
			else if (fileValidationReturnCode == 5)
				cout << "missing <c_#> tag or incomplete file";
			else if (fileValidationReturnCode == 6)
				cout << "missing <internalCoeff> tag or incomplete file";
			else if (fileValidationReturnCode == 7)
				cout << "invalid <internalCoeff> value or incomplete file";
			else if (fileValidationReturnCode == 8)
				cout << "missing </internalCoeff> tag or incomplete file";
			else if (fileValidationReturnCode == 9)
				cout << "missing <broadcastCoeff> tag or incomplete file";
			else if (fileValidationReturnCode == 10)
				cout << "invalid <broadcastCoeff> value or incomplete file";
			else if (fileValidationReturnCode == 11)
				cout << "missing </broadcastCoeff> tag or incomplete file";
			else if (fileValidationReturnCode == 12)
				cout << "missing <decayRate> tag or incomplete file";
			else if (fileValidationReturnCode == 13)
				cout << "invalid <decayRate> value or incomplete file";
			else if (fileValidationReturnCode == 14)
				cout << "missing </decayRate> tag or incomplete file";
			else if (fileValidationReturnCode == 15)
				cout << "missing <refractoryPeriod> tag or incomplete file";
			else if (fileValidationReturnCode == 16)
				cout << "invalid <refractoryPeriod> value or incomplete file";
			else if (fileValidationReturnCode == 17)
				cout << "missing </refractoryPeriod> tag or incomplete file";
			else if (fileValidationReturnCode == 18)
				cout << "missing <postLinks> tag or incomplete file";
			else if (fileValidationReturnCode == 19)
				cout << "missing <pl_#> tag or incomplete file";
			else if (fileValidationReturnCode == 20)
				cout << "missing <postCellIndex> tag or incomplete file";
			else if (fileValidationReturnCode == 21)
				cout << "invalid <postCellIndex> value or incomplete file";
			else if (fileValidationReturnCode == 22)
				cout << "invalid <postCellIndex> value (value == cell index)";
			else if (fileValidationReturnCode == 23)
				cout << "<postCellIndex> value is not in correct ascending list order";
			else if (fileValidationReturnCode == 24)
				cout << "duplicate <postCellIndex> found";
			else if (fileValidationReturnCode == 25)
				cout << "missing </postCellIndex> tag or incomplete file";
			else if (fileValidationReturnCode == 26)
				cout << "missing <linkCoeff> tag or incomplete file";
			else if (fileValidationReturnCode == 27)
				cout << "invalid <linkCoeff> value or incomplete file";
			else if (fileValidationReturnCode == 28)
				cout << "missing </linkCoeff> tag or incomplete file";
			else if (fileValidationReturnCode == 29)
				cout << "missing </pl_#> tag or incomplete file";
			else if (fileValidationReturnCode == 30)
				cout << "missing </postLinks> tag or incomplete file";
			else if (fileValidationReturnCode == 31)
				cout << "missing </c_#> tag or incomplete file";
			else if (fileValidationReturnCode == 32)
				cout << "discrepant totalCellCount and counted cells";
			else if (fileValidationReturnCode == 33)
				cout << "missing </cells> tag or incomplete file";
			else if (fileValidationReturnCode == 34)
				cout << "missing </ng_#> tag or incomplete file";
			else if (fileValidationReturnCode == 35)
				cout << "discrepant totalNetsCount and counted nets";
			else if (fileValidationReturnCode == 36)
				cout << "missing </netGroups> tag or incomplete file";
			else if (fileValidationReturnCode == 37)
				cout << "missing terminal </Population> tag";

			//reverse any possible partial initialization

			//numerical properties null states
			totalNetCount = 2;
			inputCellCount = 1;
			outputCellCount = 1;
			totalCellCount = 3;
			maxRoundsPerStimulate = 1;
			dataSetColumnCount = 1;
			dataSetRowCount = 1;
			totalDataFrameCount = 1;
			calculatedInputsCount = 0;

			//fixedInputs and calculatedInputs null states
			fixedInputs = DataSetCollection();
			calculatedInputs.resize_NoCopy(0);

			//outputs null state
			outputs.resize_NoCopy(1);

			//variablesList, updatingFixedInputsVariables, evaluationCriteraExpressions
			//and calculatedInputUpdateExpressions null states
			variablesList.delete_WholeList();
			updatingFixedInputsVariables.delete_WholeList();
			evaluationCriteriaExpressions.resize_NoCopy(1);
			calculatedInputUpdateExpressions.resize_NoCopy(0);

			//cellControls and trackers null states
			cellControls.resize_NoCopy(3);
			trackers.resize_NoCopy(3);

			//nets null state
			nets.delete_WholeList();
			nets.add_NewRange(2);

			return false;

		}

	}

public:

	//default constructor; ensure null state but safe default construction 
	Population() : totalNetCount(2), inputCellCount(1), outputCellCount(1), totalCellCount(3),
		maxRoundsPerStimulate(1), dataSetColumnCount(1), dataSetRowCount(1), totalDataFrameCount(1),
		calculatedInputsCount(0), fixedInputs(DataSetCollection()), calculatedInputs(0), outputs(1),
		evaluationCriteriaExpressions(1), calculatedInputUpdateExpressions(0), cellControls(3), trackers(3)
	{

		//nets null state
		nets.add_NewRange(2);

	}

	//initializes this Population
	bool initialize()
	{

		cout << "\n\n*** Population Initialization ***";

		//file name variables
		ConsoleValidation_Word populationFileName;
		FileValidation_WordGroup loadedContent;
		unsigned char fileValidationReturnCode;

		//open and load contents of populationFile, the population file from which to load
		cout << "\nenter name of population load file (including extension): ";
		populationFileName.getInput_Console();

		loadedContent.getInput_File(populationFileName);

		if (!loadedContent.test_ListPopulated())
		{

			cout << "invalid file name or empty file";
			return false;

		}

		//initial editing/validation
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode(">", 0, true, 1, 0));
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
		ValidationWord universalTag(tempConstructCodes);

		//delete all spaces
		loadedContent.editRange_DeleteWhitespace(loadedContent.get_FirstNode(),
			loadedContent.get_LastNode(), false);

		//insert word divisions before and after any angled brace tags
		loadedContent.editRange_Split(universalTag, loadedContent.get_FirstNode(),
			loadedContent.get_LastNode(), true, true);

		UniformNode<Word> *wordIterationNode = loadedContent.get_FirstNode();

		//validate the leading and numerical properties sections of the file, initialize
		//totalNetCount, inputCellCount, outputCellCount, totalCellCount,
		//maxRoundsPerStimulate, dataSetColumnCount, dataSetRowCount,
		//totalDataFrameCount, and calculatedInputsCount
		fileValidationReturnCode = sub_Initialize_Validate_NumericalProperties(
			loadedContent, wordIterationNode);
		
		if (!sub_Initialize_InterpretReturn_NumericalPropertiesValidation(fileValidationReturnCode))
			return false;

		//validate the inputs section of the file, initialize fixedInputs, calculatedInputs and calculatedInputsPrepValues
		fileValidationReturnCode = sub_Initialize_Validate_Inputs(
			loadedContent, wordIterationNode);

		if (!sub_Initialize_InterpretReturn_InputsValidation(fileValidationReturnCode))
			return false;

		//validate the outputs section of the file, initialize outputs
		fileValidationReturnCode = sub_Initialize_Validate_Outputs(
			loadedContent, wordIterationNode);

		if (!sub_Initialize_InterpretReturn_OutputsValidation(fileValidationReturnCode))
			return false;

		//validate the evaluation criteria and calculated input update expressions sections
		//of the file, initialize variablesList, updatingFixedInputsVariables,
		//evaluationCriteriaExpressions and calculatedInputUpdateExpressions
		fileValidationReturnCode = sub_Initialize_Validate_Expressions(
			loadedContent, wordIterationNode);

		if (!sub_Initialize_InterpretReturn_ExpressionsValidation(
			fileValidationReturnCode))
			return false;

		//validate the cellControls section of the file,
		//initialize cellControls and trackers
		fileValidationReturnCode = sub_Initialize_ValidateCellControls(
			loadedContent, wordIterationNode);

		if (!sub_Initialize_InterpretReturn_CellControlsValidation(fileValidationReturnCode))
			return false;

		//validate the nets and terminal sections of the file,
		//initialize nets
		fileValidationReturnCode = sub_Initialize_ValidateNetsAndTerminal(
			loadedContent, wordIterationNode);

		if (!sub_Initialize_InterpretReturn_NetsAndTerminalValidation(fileValidationReturnCode))
			return false;

		//every section of the file has been checked and cross validated
		//all population variables have been initialized
		return true;

	}

	void evolve_Asexual(const unsigned int evolutionCyclesCount,
		const unsigned short mutationsPerNetPerCycle, const double mutationAmplitude)
	{

		//used to facilitate updating calculatedInputs in parallel rather than in series
		ExtensibleArray<double> calculatedInputsPrepValues(calculatedInputsCount);

		//arrays used to store cell indices reach action potential in a given round
		//within the stimulate() function (array length set to totalCellCount + 1
		//because it is possible for every cell to be activated in a single stimulus
		//round and one additional element is needed in that case to store the terminal
		//boundary marker value of (-1)
		ExtensibleArray<int> activationListRoundA(totalCellCount + 1);
		ExtensibleArray<int> activationListRoundB(totalCellCount + 1);

		//additional initial setup
		unsigned short cycleCount(0);
		UniformNode<Net> *netNode;
		UniformNode<Net> *leadingNetNode;
		UniformNode<Net> *reverseIterationNetNode;

		unsigned short count;

		cout << "\n\npopulation evolution progress:";

		//evolution cycle loop
		while (cycleCount < evolutionCyclesCount)
		{

			//run all stimulation and performance updates
			stimulate_AllFrames_SingleEvolutionCycle(activationListRoundA,
				activationListRoundB, calculatedInputsPrepValues);

			//sort nets in order of fitness
			netNode = nets.get_FirstNode();
			leadingNetNode = netNode->get_NextNode();

			do
			{

				if (leadingNetNode->content.get_FitnessRating() < netNode->content.get_FitnessRating())
				{

					reverseIterationNetNode = netNode->get_PriorNode();

					do
					{

						if (!reverseIterationNetNode)
						{

							nets.move_Node(leadingNetNode, NULL, false);
							break;

						}

						else if (leadingNetNode->content.get_FitnessRating() >= reverseIterationNetNode->content.get_FitnessRating())
						{

							nets.move_Node(leadingNetNode, reverseIterationNetNode, true);
							break;

						}

						else //(leadingNetNode->content.get_FitnessRating() < reverseIterationNetNode->content.get_FitnessRating())
							reverseIterationNetNode = reverseIterationNetNode->get_PriorNode();

					} while (true);

				}

				else
					netNode = leadingNetNode;

				leadingNetNode = netNode->get_NextNode();

			} while (leadingNetNode);

			//asexual reproduction of top performing nets
			netNode = nets.get_FirstNode();
			reverseIterationNetNode = nets.get_LastNode();

			for (count = (totalNetCount / 2); count > 0; count--)
			{

				reverseIterationNetNode->content = netNode->content;

				netNode = netNode->get_NextNode();
				reverseIterationNetNode = reverseIterationNetNode->get_PriorNode();

			}

			//mutate all but the top performing net
			netNode = nets.get_FirstNode()->get_NextNode();

			do
			{

				netNode->content.mutate(cellControls,
					mutationsPerNetPerCycle, mutationAmplitude);

				netNode = netNode->get_NextNode();

			} while (netNode);

			cycleCount++;
	
			cout << "\n\tcycle " << cycleCount << " completed; top performing net's fitnessRating is ";
			cout << nets.get_FirstNode()->content.get_FitnessRating();

		}

		cout << "\npopulation evolution completed successfully";

	}

	//assumes maxSpliceLength > 0
	void evolve_Sexual(const unsigned int evolutionCyclesCount,
		const unsigned short mutationsPerNetPerCycle,
		const double mutationAmplitude, const unsigned short maxSpliceLength)
	{

		//used to facilitate updating calculatedInputs in parallel rather than in series
		ExtensibleArray<double> calculatedInputsPrepValues(calculatedInputsCount);

		//arrays used to store cell indices reach action potential in a given round
		//within the stimulate() function (array length set to totalCellCount + 1
		//because it is possible for every cell to be activated in a single stimulus
		//round and one additional element is needed in that case to store the terminal
		//boundary marker value of (-1)
		ExtensibleArray<int> activationListRoundA(totalCellCount + 1);
		ExtensibleArray<int> activationListRoundB(totalCellCount + 1);

		//additional initial setup
		unsigned short cycleCount = 0;
		UniformNode<Net> *netNode;
		UniformNode<Net> *leadingNetNode;
		UniformNode<Net> *reverseIterationNetNode;
		unsigned short netIndex;
		unsigned short count;
		unsigned short reproductionCount;

		cout << "\n\npopulation evolution progress:";

		//evolution cycle loop
		while (cycleCount < evolutionCyclesCount)
		{

			//run all stimulation and performance updates
			stimulate_AllFrames_SingleEvolutionCycle(activationListRoundA,
				activationListRoundB, calculatedInputsPrepValues);

			//sort nets in order of fitness
			netNode = nets.get_FirstNode();
			leadingNetNode = netNode->get_NextNode();

			do
			{

				if (leadingNetNode->content.get_FitnessRating() < netNode->content.get_FitnessRating())
				{

					reverseIterationNetNode = netNode->get_PriorNode();

					do
					{

						if (!reverseIterationNetNode)
						{

							nets.move_Node(leadingNetNode, NULL, false);
							break;

						}

						else if (leadingNetNode->content.get_FitnessRating() >= reverseIterationNetNode->content.get_FitnessRating())
						{

							nets.move_Node(leadingNetNode, reverseIterationNetNode, true);
							break;

						}

						else //(leadingNetNode->content.get_FitnessRating() < reverseIterationNetNode->content.get_FitnessRating())
							reverseIterationNetNode = reverseIterationNetNode->get_PriorNode();

					} while (true);

				}

				else
					netNode = leadingNetNode;

				leadingNetNode = netNode->get_NextNode();

			} while (leadingNetNode);

			//sexual reproduction of top performing nets
			//netNode is acting as mother, leadingNetNode as father, and reverseIterationNode as child
			netNode = nets.get_FirstNode();
			reverseIterationNetNode = nets.get_LastNode();

			reproductionCount = (totalNetCount / 2);

			for (count = 0; count < reproductionCount; count++)
			{

				//randomly select the father net
				do
				{

					netIndex = rand() % (totalNetCount - count);

				} while (netIndex == count || netIndex >= (totalNetCount - 1 - count));

				leadingNetNode = nets.get_NodeAddress(netIndex);

				//splice groups of cells from mother and father nets into child
				reverseIterationNetNode->content.meiosis(netNode->content.get_Cells(),
					leadingNetNode->content.get_Cells(), maxSpliceLength);

				//iterate to next mother and child nets 
				netNode = netNode->get_NextNode();
				reverseIterationNetNode = reverseIterationNetNode->get_PriorNode();

			}

			//mutate all but the top performing net
			netNode = nets.get_FirstNode()->get_NextNode();

			do
			{

				netNode->content.mutate(cellControls,
					mutationsPerNetPerCycle, mutationAmplitude);

				netNode = netNode->get_NextNode();

			} while (netNode);

			cycleCount++;

			cout << "\n\tcycle " << cycleCount << " completed; top performing net's fitnessRating is ";
			cout << nets.get_FirstNode()->content.get_FitnessRating();

		}

		cout << "\npopulation evolution completed successfully";

	}

	//record stimulus passing through the first netsIncluded nets by performance
	//records by data set position and data frame; assumes netsIncluded < totalNetCount
	//recordingType == false means XML, true means JSON
	bool recordedRead(Word &outputFileName,	const bool recordingType)
	{

		ofstream outputFile;

		//open file corresponding to fileName or return if opening fails
		if (!open_OutputFile(outputFile, outputFileName))
			return false;

		//used to facilitate updating calculatedInputs in parallel rather than in series
		ExtensibleArray<double> calculatedInputsPrepValues(calculatedInputsCount);

		//arrays used to store cell indices reach action potential in a given round
		//within the stimulate() function													
		ExtensibleArray<int> activationListRoundA(totalCellCount);
		ExtensibleArray<int> activationListRoundB(totalCellCount);

		stimulate_AllFrames_SingleEvolutionCycle_Recorded(activationListRoundA,
			activationListRoundB, calculatedInputsPrepValues);

		//output to file while running all stimulation and performance updates
		if (recordingType)
			recordingData.output_ToFile_JSON(outputFile, nets.get_FirstNode()->content.get_Cells(),
				totalCellCount, totalDataFrameCount);

		else
		{

			outputFile << "<?xml version = \"1.0\"?>";
			recordingData.output_ToFile_XML(outputFile, nets.get_FirstNode()->content.get_Cells(),
				totalCellCount, totalDataFrameCount);

		}

		outputFile.close();

		return true;

	}

	//outputs all population information to a file in XML format
	//if viewingType is true, the report will contain more information,
	//but wont be able to be used in validation/loading of a new population
	//w/out editing
	void output_ToFile_XML(Word &outputFileName, const bool overwrite)
	{

		ofstream outputFile;

		//open file corresponding to fileName or return if opening fails
		if (!open_OutputFile(outputFile, outputFileName, overwrite))
			return;

		unsigned long long count;
		UniformNode<Net> *netNode(nets.get_FirstNode());

		//output to file
		outputFile << "<?xml version = \"1.0\"?>";
		outputFile << "\n<Population>";

		//numerical properties
		outputFile << "\n\t<totalNetCount>" << totalNetCount << "</totalNetCount>";
		outputFile << "\n\t<inputCellCount>" << inputCellCount << "</inputCellCount>";
		outputFile << "\n\t<outputCellCount>" << outputCellCount << "</outputCellCount>";
		outputFile << "\n\t<totalCellCount>" << totalCellCount << "</totalCellCount>";
		outputFile << "\n\t<maxRoundsPerStimulate>" << maxRoundsPerStimulate << "</maxRoundsPerStimulate>";
		outputFile << "\n\t<dataSetColumnCount>" << dataSetColumnCount << "</dataSetColumnCount>";
		outputFile << "\n\t<dataSetRowCount>" << dataSetRowCount << "</dataSetRowCount>";
		outputFile << "\n\t<totalDataFrameCount>" << totalDataFrameCount << "</totalDataFrameCount>";
		outputFile << "\n\t<calculatedInputsCount>" << calculatedInputsCount << "</calculatedInputsCount>";

		//fixed inputs
		outputFile << "\n\t<fixedInputs>";
		fixedInputs.output_ToOpenFile_XML(outputFile);
		outputFile << "\n\t</fixedInputs>";

		//calculated inputs
		outputFile << "\n\t<calculatedInputs>";
		for (count = 0; count < calculatedInputsCount; count++)
		{

			outputFile << "\n\t\t<ci_" << count << ">";
			calculatedInputs.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t</ci_" << count << ">";

		}
		outputFile << "\n\t</calculatedInputs>";

		//outputs
		outputFile << "\n\t<outputs>";
		for (count = 0; count < outputCellCount; count++)
		{

			outputFile << "\n\t\t<o_" << count << ">";
			outputs.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t</o_" << count << ">";

		}
		outputFile << "\n\t</outputs>";

		//evaluationCriteriaExpressions
		outputFile << "\n\t<evaluationCriteriaExpressions>";
		for (count = 0; count < dataSetRowCount; count++)
		{

			outputFile << "\n\t\t<ece_" << count << ">";
			evaluationCriteriaExpressions.get_ElementAddress(
				count)->get_WrittenForm().output_ToOpenFile(outputFile);
			outputFile << "</ece_" << count << ">";

		}
		outputFile << "\n\t</evaluationCriteriaExpressions>";

		//calculatedInputUpdateExpressions
		outputFile << "\n\t<calculatedInputUpdateExpressions>";
		for (count = 0; count < calculatedInputsCount; count++)
		{

			outputFile << "\n\t\t<ciue_" << count << ">";
			calculatedInputUpdateExpressions.get_ElementAddress(
				count)->get_WrittenForm().output_ToOpenFile(outputFile);
			outputFile << "</ciue_" << count << ">";

		}
		outputFile << "\n\t</calculatedInputUpdateExpressions>";

		//cellControls
		outputFile << "\n\t<cellControlGroups>";
		for (count = 0; count < totalCellCount; count++)
		{

			outputFile << "\n\t\t<ccg_" << count << ">";
			cellControls.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t</ccg_" << count << ">";

		}
		outputFile << "\n\t</cellControlGroups>";

		//nets
		outputFile << "\n\t<netGroups>";
		for (count = 0; count < totalNetCount; count++)
		{

			outputFile << "\n\t\t<ng_" << count << ">";
			netNode->content.output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t</ng_" << count << ">";

			netNode = netNode->get_NextNode();

		}
		outputFile << "\n\t</netGroups>";

		outputFile << "\n</Population>";

		outputFile.close();

	}

	//outputs all population information to a file in JSON format
	//without any spaces
	void output_ToFile_JSON(Word &outputFileName, const bool overwrite)
	{

		ofstream outputFile;

		//open file corresponding to fileName or return if opening fails
		if (!open_OutputFile(outputFile, outputFileName, overwrite))
			return;

		unsigned long long count;
		UniformNode<Net> *netNode(nets.get_FirstNode());

		//output to file
		outputFile << "{";

		outputFile << "\"Population\":{";

		//numerical properties
		outputFile << "\"totalNetCount\":" << totalNetCount << ",";
		outputFile << "\"inputCellCount\":" << inputCellCount << ",";
		outputFile << "\"outputCellCount\":" << outputCellCount << ",";
		outputFile << "\"totalCellCount\":" << totalCellCount << ",";
		outputFile << "\"maxRoundsPerStimulate\":" << maxRoundsPerStimulate << ",";
		outputFile << "\"dataSetColumnCount\":" << dataSetColumnCount << ",";
		outputFile << "\"dataSetRowCount\":" << dataSetRowCount << ",";
		outputFile << "\"totalDataFrameCount\":" << totalDataFrameCount << ",";
		outputFile << "\"calculatedInputsCount\":" << calculatedInputsCount << ",";


		//fixed inputs
		outputFile << "\"fixedInputs\":{";
		fixedInputs.output_ToOpenFile_JSON(outputFile);
		outputFile << "},";

		//calculated inputs
		outputFile << "\"calculatedInputs\":[";
		for (count = 0; count < calculatedInputsCount - 1; count++)
		{

			outputFile << "{";
			calculatedInputs.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

		}
		outputFile << "{";
		calculatedInputs.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
		outputFile << "}],";

		//outputs
		outputFile << "\"outputs\":[";
		for (count = 0; count < outputCellCount - 1; count++)
		{

			outputFile << "{";
			outputs.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

		}
		outputFile << "{";
		outputs.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
		outputFile << "}],";

		//evaluationCriteriaExpressions
		outputFile << "\"evaluationCriteriaExpressions\":[";
		for (count = 0; count < dataSetRowCount - 1; count++)
		{

			outputFile << "\"";
			evaluationCriteriaExpressions.get_ElementAddress(
				count)->get_WrittenForm().output_ToOpenFile(outputFile);
			outputFile << "\",";

		}
		outputFile << "\"";
		evaluationCriteriaExpressions.get_ElementAddress(
			count)->get_WrittenForm().output_ToOpenFile(outputFile);
		outputFile << "\"],";

		//calculatedInputUpdateExpressions
		outputFile << "\"calculatedInputUpdateExpressions\":[";
		if (calculatedInputsCount)
		{


			for (count = 0; count < calculatedInputsCount - 1; count++)
			{

				outputFile << "\"";
				calculatedInputUpdateExpressions.get_ElementAddress(
					count)->get_WrittenForm().output_ToOpenFile(outputFile);
				outputFile << "\",";

			}
			outputFile << "\"";
			calculatedInputUpdateExpressions.get_ElementAddress(
				count)->get_WrittenForm().output_ToOpenFile(outputFile);
			outputFile << "\"";

		}
		outputFile << "],";

		//cellControls
		outputFile << "\"cellControls\":[";
		for (count = 0; count < totalCellCount - 1; count++)
		{

			outputFile << "{";
			cellControls.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

		}
		outputFile << "{";
		cellControls.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
		outputFile << "}],";

		//nets
		outputFile << "\"nets\":[";
		for (count = 0; count < totalNetCount - 1; count++)
		{

			outputFile << "{";
			netNode->content.output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

			netNode = netNode->get_NextNode();

		}
		outputFile << "{";
		netNode->content.output_ToOpenFile_JSON(outputFile);
		outputFile << "}]";

		outputFile << "}";

		outputFile << "}";

		outputFile.close();

	}

	//getter used in main to validate user input
	unsigned short get_TotalNetCount() const
	{

		return totalNetCount;

	}

};

#endif // POPULATION_H_INCLUDED
