#ifndef RECORDINGMODULE_H_INCLUDED
#define RECORDINGMODULE_H_INCLUDED

#include "MemoryTools.h"
#include "StimulusTracker.h"
#include "Cell.h"

class RecordedStimulusRound
{

private:

	//a copy of the state of all stimulus trackers for a net over the course of
	//a single stimulus round (array of length totalCellCount)
	ExtensibleArray<StimulusTracker> trackersStates;

public:

	//default constructor
	RecordedStimulusRound() {}

	//modified constructor
	RecordedStimulusRound(ExtensibleArray<StimulusTracker> &populationTrackers)
		: trackersStates(populationTrackers) {}

	//default destructor
	~RecordedStimulusRound() {}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short roundCount, const unsigned short totalCellCount)
	{

		unsigned short cellIndex(0);
		StimulusTracker *recordingTracker;
		Cell *recordingCell;

		outputFile << "\n\t\t\t\t\t\t\t\t<cells>";

		do
		{

			recordingTracker = trackersStates.get_ElementAddress(cellIndex);
			recordingCell = netCells.get_ElementAddress(cellIndex);

			outputFile << "\n\t\t\t\t\t<c_" << cellIndex << ">";

			if (!recordingTracker->get_Stimulus())
			{

				//cell is not in refractory period
				if (recordingTracker->get_RoundRefractionComplete() <= roundCount)
					outputFile << "\n\t\t\t\t\t\t<status>labile</status>";

				//cell is either just activated or in refractory period
				else
				{

					if (recordingTracker->get_LastRoundStimulusChanged() == roundCount)
						outputFile << "\n\t\t\t\t\t\t<status>excited</status>";

					else
						outputFile << "\n\t\t\t\t\t\t<status>refracting</status>";

				}

				outputFile << "\n\t\t\t\t\t\t<stimulus>0</stimulus>";

			}

			else
			{

				outputFile << "\n\t\t\t\t\t\t<status>labile</status>";
				outputFile << "\n\t\t\t\t\t\t<stimulus>";

				//stimulus is current
				if (recordingTracker->get_LastRoundStimulusChanged() == roundCount)
					outputDouble_ToOpenFile(recordingTracker->get_Stimulus(), outputFile, 15);

				//stimulus needs to be calculated based on decay
				else
					outputDouble_ToOpenFile(recordingTracker->calculate_DecayedStimulus(
						recordingCell->get_DecayRate(), roundCount), outputFile, 15);

				outputFile << "</stimulus>";

			}

			outputFile << "\n\t\t\t\t\t\t<lastRoundStimulusChanged>" << recordingTracker->get_LastRoundStimulusChanged() << "</lastRoundStimulusChanged>";
			outputFile << "\n\t\t\t\t\t\t<roundRefractionComplete>" << recordingTracker->get_RoundRefractionComplete() << "</roundRefractionComplete>";
			outputFile << "\n\t\t\t\t\t\t<activationCount>" << recordingTracker->get_ActivationCount() << "</activationCount>";
			outputFile << "\n\t\t\t\t\t</c_" << cellIndex << ">";

			cellIndex++;

		} while (cellIndex < totalCellCount);

		outputFile << "\n\t\t\t\t\t\t\t\t</cells>";

	}

	void output_ToOpenFile_JSON(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short roundCount, unsigned short aTotalCellCount)
	{

		aTotalCellCount--;
		unsigned short cellIndex(0);
		StimulusTracker *recordingTracker;
		Cell *recordingCell;

		outputFile << "\"cells\":[";

		for (; cellIndex < aTotalCellCount; cellIndex++)
		{

			recordingTracker = trackersStates.get_ElementAddress(cellIndex);
			recordingCell = netCells.get_ElementAddress(cellIndex);

			outputFile << "{";

			if (!recordingTracker->get_Stimulus())
			{

				//cell is not in refractory period
				if (recordingTracker->get_RoundRefractionComplete() <= roundCount)
					outputFile << "\"status\":\"labile\",";

				//cell is either just activated or in refractory period
				else
				{

					if (recordingTracker->get_LastRoundStimulusChanged() == roundCount)
						outputFile << "\"status\":\"excited\",";

					else
						outputFile << "\"status\":\"refracting\",";

				}

				outputFile << "\"stimulus\":0,";

			}

			else
			{

				outputFile << "\"status\":\"labile\",";
				outputFile << "\"stimulus\":";

				//stimulus is current
				if (recordingTracker->get_LastRoundStimulusChanged() == roundCount)
					outputDouble_ToOpenFile(recordingTracker->get_Stimulus(), outputFile, 15);

				//stimulus needs to be calculated based on decay
				else
					outputDouble_ToOpenFile(recordingTracker->calculate_DecayedStimulus(
						recordingCell->get_DecayRate(), roundCount), outputFile, 15);

				outputFile << ",";

			}

			outputFile << "\"lastRoundStimulusChanged\":" << recordingTracker->get_LastRoundStimulusChanged() << ",";
			outputFile << "\"roundRefractionComplete\":" << recordingTracker->get_RoundRefractionComplete() << ",";
			outputFile << "\"activationCount\":" << recordingTracker->get_ActivationCount() << "},";

		}

		recordingTracker = trackersStates.get_ElementAddress(cellIndex);
		recordingCell = netCells.get_ElementAddress(cellIndex);

		outputFile << "{";

		if (!recordingTracker->get_Stimulus())
		{

			//cell is not in refractory period
			if (recordingTracker->get_RoundRefractionComplete() <= roundCount)
				outputFile << "\"status\":\"labile\",";

			//cell is either just activated or in refractory period
			else
			{

				if (recordingTracker->get_LastRoundStimulusChanged() == roundCount)
					outputFile << "\"status\":\"excited\",";

				else
					outputFile << "\"status\":\"refracting\",";

			}

			outputFile << "\"stimulus\":0,";

		}

		else
		{

			outputFile << "\"status\":\"labile\",";
			outputFile << "\"stimulus\":";

			//stimulus is current
			if (recordingTracker->get_LastRoundStimulusChanged() == roundCount)
				outputDouble_ToOpenFile(recordingTracker->get_Stimulus(), outputFile, 15);

			//stimulus needs to be calculated based on decay
			else
				outputDouble_ToOpenFile(recordingTracker->calculate_DecayedStimulus(
					recordingCell->get_DecayRate(), roundCount), outputFile, 15);

			outputFile << ",";

		}

		outputFile << "\"lastRoundStimulusChanged\":" << recordingTracker->get_LastRoundStimulusChanged() << ",";
		outputFile << "\"roundRefractionComplete\":" << recordingTracker->get_RoundRefractionComplete() << ",";
		outputFile << "\"activationCount\":" << recordingTracker->get_ActivationCount() << "}]";

	}

};

class RecordedDataFrame
{

private:

	//a copy of the state of all stimulus trackers for a net over the course of
	//a all stimulus rounds in a single data frame (list length will be set in
	//through successive calls of add_StimulusRound() based on
	//the number of stimulus rounds within the data frame)
	UniformList<RecordedStimulusRound> stimulusRounds;

public:

	//default constructor
	RecordedDataFrame() {}

	//default destructor
	~RecordedDataFrame() {}

	//adds a new stimulus round at the end of stimulusRounds, copying the
	//values of populationTrackers into the new round
	void add_StimulusRound(ExtensibleArray<StimulusTracker> &populationTrackers)
	{

		stimulusRounds.add_NewNode(populationTrackers);

	}

	//assumes outputFile is already open, stimulusRounds is populated
	void output_ToFile_XML(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short totalCellCount)
	{

		UniformNode<RecordedStimulusRound> *roundNode(stimulusRounds.get_FirstNode());
		unsigned short roundCount(1);

		outputFile << "\n\t\t\t\t\t\t<stimulusRounds>";

		while (roundNode)
		{

			outputFile << "\n\t\t\t\t\t\t\t<stimulusRound_" << roundCount << ">";

			roundNode->content.output_ToOpenFile_XML(outputFile,
				netCells, roundCount, totalCellCount);

			outputFile << "\n\t\t\t\t\t\t\t</stimulusRound_" << roundCount << ">";

			roundNode = roundNode->get_NextNode();
			roundCount++;

		}

		outputFile << "\n\t\t\t\t\t\t</stimulusRounds>";

	}

	void output_ToFile_JSON(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short totalCellCount)
	{

		outputFile << "\"stimulusRounds\":[";

		unsigned short length(stimulusRounds.get_ListLength());

		if (length)
		{

			length--;
			UniformNode<RecordedStimulusRound> *roundNode(stimulusRounds.get_FirstNode());
			unsigned short roundCount(0);

			for (; roundCount < length; roundCount++)
			{

				outputFile << "{";
				roundNode->content.output_ToOpenFile_JSON(outputFile,
					netCells, roundCount + 1, totalCellCount);
				outputFile << "},";

				roundNode = roundNode->get_NextNode();

			}

			outputFile << "{";
			roundNode->content.output_ToOpenFile_JSON(outputFile,
				netCells, roundCount + 1, totalCellCount);
			outputFile << "}]";

		}

		else
			outputFile << "]";

	}

};

class RecordedDataSetRow
{

private:

	//a copy of the state of all stimulus trackers for a net over the course of
	//all stimulus rounds in all data frames in a single data set row; array
	//length corresponds to totalDataFrameCount
	ExtensibleArray<RecordedDataFrame> dataFrames;

public:

	//default constructor
	RecordedDataSetRow() {}

	//modified constructor
	RecordedDataSetRow(const unsigned long long totalDataFrameCount) : dataFrames(totalDataFrameCount) {}

	//default destructor
	~RecordedDataSetRow() {}

	//adds a new stimulus round at the end of the RecordedDataFrame element in dataFrames
	//at dataFrameIndex, copying the values of populationTrackers into the new round
	void add_StimulusRound(ExtensibleArray<StimulusTracker> &populationTrackers,
		const unsigned long long dataFrameIndex)
	{

		dataFrames.get_ElementAddress(dataFrameIndex)->add_StimulusRound(populationTrackers);

	}

	//assumes outputFile is already open, assumes every stimulus round in each element of dataFrames
	//is populated with at least one member
	void output_ToFile_XML(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short totalCellCount, const unsigned long long totalDataFrameCount)
	{

		unsigned short frameCount(0);

		outputFile << "\n\t\t\t\t<dataFrames>";

		do
		{

			outputFile << "\n\t\t\t\t\t<dataFrame_" << frameCount << ">";
			dataFrames.get_ElementAddress(frameCount)->output_ToFile_XML(outputFile, netCells, totalCellCount);
			outputFile << "\n\t\t\t\t\t</dataFrame_" << frameCount << ">";

			frameCount++;

		} while (frameCount < totalDataFrameCount);

		outputFile << "\n\t\t\t\t</dataFrames>";

	}

	//assumes outputFile is already open, assumes every stimulus round in each element of dataFrames
	//is populated with at least one member
	void output_ToFile_JSON(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short totalCellCount, const unsigned long long totalDataFrameCount)
	{

		unsigned long long frameIndex(0);
		unsigned long long length(totalDataFrameCount - 1);

		outputFile << "\"dataFrames\":[";

		for (; frameIndex < length; frameIndex++)
		{

			outputFile << "{";
			dataFrames.get_ElementAddress(frameIndex)->output_ToFile_JSON(outputFile, netCells, totalCellCount);
			outputFile << "},";

		}

		outputFile << "{";
		dataFrames.get_ElementAddress(frameIndex)->output_ToFile_JSON(outputFile, netCells, totalCellCount);
		outputFile << "}]";

	}

};

class RecordedDataSetRowSequence
{

private:

	//a copy of the state of all stimulus trackers for a net over the course of
	//all stimulus rounds in all data frames in a sequence of data set rows; list
	//length depends on the sequence of data set rows
	//***(this may be able to change to an array later taking an evolution control reference as a parameter?)***
	UniformList<RecordedDataSetRow> dataSetRows;

public:

	//default constructor
	RecordedDataSetRowSequence() {}

	//default destructor
	~RecordedDataSetRowSequence() {}

	//adds a new stimulus round at the RecordedDataFrame element in dataFrames
	//at dataFrameIndex of currentRow; assumes currentRow is the member of
	//dataSetRows in which recorded stimulus is to be stored
	void add_StimulusRound(UniformNode<RecordedDataSetRow> *currentRow, ExtensibleArray<StimulusTracker> &populationTrackers,
		const unsigned long long dataFrameIndex)
	{

		currentRow->content.add_StimulusRound(populationTrackers, dataFrameIndex);

	}

	//used to add a new row and store the address of the corresponding node
	UniformNode<RecordedDataSetRow> *add_NewDataSetRow(const unsigned long long totalDataFrameCount)
	{

		return dataSetRows.add_NewNode(RecordedDataSetRow(totalDataFrameCount));

	}

	//deletes everything in the object
	void reset()
	{

		dataSetRows.delete_WholeList();

	}

	//assumes outputFile is already open, assumes every stimulus round in each element of dataFrames
	//in every dataSetRow is populated with at least one member, assumes dataSetRows is populated with
	//at least one member
	void output_ToFile_XML(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short totalCellCount, const unsigned long long totalDataFrameCount)
	{

		UniformNode<RecordedDataSetRow> *dataSetRowNode(dataSetRows.get_FirstNode());
		
		//this will need to change later to also show the index
		unsigned short rowIndex(0);

		//temporary
		outputFile << "\n\t<net_0>";
		//temporary

		outputFile << "\n\t\t<dataSetRows>";

		do
		{

			outputFile << "\n\t\t\t<dataSetRow_" << rowIndex << ">";
			dataSetRowNode->content.output_ToFile_XML(outputFile,
				netCells, totalCellCount, totalDataFrameCount);
			outputFile << "\n\t\t\t</dataSetRow_" << rowIndex << ">";

			rowIndex++;
			dataSetRowNode = dataSetRowNode->get_NextNode();

		} while (dataSetRowNode);

		outputFile << "\n\t\t</dataSetRows>";
		
		//temporary
		outputFile << "\n\t</net_0>";
		//temporary

	}

	//assumes outputFile is already open, assumes every stimulus round in each element of dataFrames
	//in every dataSetRow is populated with at least one member, assumes dataSetRows is populated with
	//at least one member
	void output_ToFile_JSON(ofstream &outputFile, ExtensibleArray<Cell> &netCells,
		const unsigned short totalCellCount, const unsigned long long totalDataFrameCount)
	{

		UniformNode<RecordedDataSetRow> *dataSetRowNode(dataSetRows.get_FirstNode());

		//this will need to change later to also show the index
		unsigned long long rowIndex(0);
		unsigned long long rowLength(dataSetRows.get_ListLength() - 1);

		//temporary
		outputFile << "{";
		//temporary

		outputFile << "\"dataSetRows\":[";

		for (; rowIndex < rowLength; rowIndex++)
		{

			outputFile << "{";
			dataSetRowNode->content.output_ToFile_JSON(outputFile, netCells,
				totalCellCount, totalDataFrameCount);
			outputFile << "},";

			dataSetRowNode = dataSetRowNode->get_NextNode();

		}

		outputFile << "{";
		dataSetRowNode->content.output_ToFile_JSON(outputFile, netCells,
			totalCellCount, totalDataFrameCount);
		outputFile << "}]";

		//temporary
		outputFile << "}";
		//temporary

	}

};

//***WORK FROM HERE TO CREATE ANOTHER LEVEL INTENDED FOR RECORDING STIMULUS DATA FOR A GROUP OF NETS?***

class RecordingModule
{

private:

	/*

	a RecordingModule object will be passed by reference to the population::evolve() function(s); it will be passed starting in a null state and will grow with each evolutionary cycle

	note: nets may need to have a new property: a long long index number which will be used in linking parental nets to their children when a recording module is
	converted to a full readable recreation of each net from each evolutionary cycle; this property may also be needed/helpfull in linking nets to recorded reads/performance?

	properties:
		
		*a copied array or list of all the starting nets in the population

		*note: I probably need to create a new type of branching list which is cross-listed by level (can be iterated over level by level), this is necessary
		because I need to be able to jump quickly to the last row to find the mother/father for creating each new pseudo net. without a level indexing mechanism
		finding these nets would become very processing intensive very quickly
		

		*an indexed branching list of PseudoNet objects which store the following:
			*implicitly a pointer to the mother (the PseudoNet's parent in the branching structure)
			*a stored pointer to the father (if any)
			*a map of how the the sexual or asexual production of this net unfolded (likely a list or array of parent bool and cell group length couplets)
			*an array representing any mutations which occurred after the production of this net (unsigned char mutationType and double change in value couplets) 

	*/

public:



};

#endif // RECORDINGMODULE_H_INCLUDED