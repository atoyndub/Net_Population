#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "MemoryTools.h"
#include "StringTools.h"

/*

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
	with the DataSetColumn's frameLength, shiftLength, and the population's totalFrameCount

rowIndex - a local variable term used to track the DataSetRow index being referred to
columnIndex - a local variable term used to track the DataSetColumn index being referred to

*/

class DataSet
{

private:
	
	//main data array
	ExtensibleArray<double> dataPoints;

public:

	//default constructor (generally safe null state)
	DataSet() : dataPoints(1)
	{

		*(dataPoints.get_ElementAddress(0)) = 0.0;

	}

	//modified constructor - assumes dataValues is populated and validated
	//in population context
	DataSet(UniformList<double> &aDataPoints) : dataPoints(aDataPoints) {}

	//default destructor
	~DataSet() {}

	//get a frameshift adjusted data point value based on the frameSubIndex
	//parameter (index number of desired element in context of the data frame
	//- corresponding to an input cell's relative position within it's input sub group),
	//the frameIndex parameter corresponding to the frame number within the DataSet,
	//and shiftLength corresponding to the 'containing' DataSetColumn's shiftLength;
	//assumes dataFrameSubIndex < the 'containing' column's frameLength and
	//dataFrameIndex is less than the containing Population's totalFrameCount;
	//assumes all parameters have been validated in Population context
	double get_FrameAdjustedDataPoint(const unsigned long long dataFrameIndex,
		const unsigned short dataFrameSubIndex, const unsigned short shiftLength)
	{

		return dataPoints.get_Element(dataFrameSubIndex + (dataFrameIndex * shiftLength));

	}

	//get a frameshift adjusted data point address based on the frameSubIndex
	//parameter (index number of desired element in context of the data frame
	//- corresponding to an /input cell's relative position within it's input sub group),
	//the frameIndex parameter corresponding to the frame number within the DataSet,
	//and shiftLength corresponding to the 'containing' DataSetColumn's shiftLength;
	//assumes dataFrameSubIndex < the 'containing' column's frameLength and
	//dataFrameIndex is less than the containing Population's totalFrameCount;
	//assumes all parameters have been validated in Population context
	double *get_FrameAdjustedDataPointAddress(const unsigned long long dataFrameIndex,
		const unsigned short dataFrameSubIndex, const unsigned short shiftLength)
	{

		return dataPoints.get_ElementAddress(dataFrameSubIndex + (dataFrameIndex * shiftLength));

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		unsigned long long length = dataPoints.get_ArrayLength();
		unsigned long long quintileIterations = length / 5;
		unsigned long long dataPointIndex;

		outputFile << "\n\t\t\t\t\t\t<dataPoints>";

		if (quintileIterations)
		{

			unsigned long long quintileCount = 0;
			unsigned char count;

			do
			{

				outputFile << "\n\t\t\t\t\t\t\t";

				for (count = 0; count < 5; count++)
				{

					dataPointIndex = quintileCount * 5 + count;

					outputFile << "<dp_" << dataPointIndex << ">";
					outputDouble_ToOpenFile(dataPoints.get_Element(dataPointIndex), outputFile, 15);
					outputFile << "</dp_" << dataPointIndex << ">";

				}

				quintileCount++;

			} while (quintileCount < quintileIterations);

			dataPointIndex++;
			
			if (dataPointIndex < length)
			{

				outputFile << "\n\t\t\t\t\t\t\t";

				do
				{

					outputFile << "<dp_" << dataPointIndex << ">";
					outputDouble_ToOpenFile(dataPoints.get_Element(dataPointIndex), outputFile, 15);
					outputFile << "</dp_" << dataPointIndex << ">";

					dataPointIndex++;

				} while (dataPointIndex < length);

			}
			
		}

		else
		{

			dataPointIndex = 0;
			outputFile << "\n\t\t\t\t\t\t\t";

			do
			{

				outputFile << "<dp_" << dataPointIndex << ">";
				outputDouble_ToOpenFile(dataPoints.get_Element(dataPointIndex), outputFile, 15);
				outputFile << "</dp_" << dataPointIndex << ">";

				dataPointIndex++;

			} while (dataPointIndex < length);

		}

		outputFile << "\n\t\t\t\t\t\t</dataPoints>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		unsigned long long length = dataPoints.get_ArrayLength() - 1;
		unsigned long long dataPointIndex(0);

		outputFile << "\"dataPoints\":[";

		while (dataPointIndex < length)
		{

			outputDouble_ToOpenFile(dataPoints.get_Element(dataPointIndex), outputFile, 15);
			outputFile << ",";

			dataPointIndex++;

		}

		outputDouble_ToOpenFile(dataPoints.get_Element(dataPointIndex), outputFile, 15);
		outputFile << "]";

	}

};

class DataSetRow
{

private:

	//identifier
	Word name;

	//main DataSet array
	ExtensibleArray<DataSet> dataSets;

public:

	//default constructor (generally safe null state)
	DataSetRow() : name("unidentified_row"), dataSets(1) {}

	//modified constructor - used for population-validated construction
	//assumes parameters have been validated in population context;
	//assumes aName and aDataSets are populated; moves aName from
	//the reference parameter
	DataSetRow(Word &aName, UniformList<DataSet> &aDataSets)
		: dataSets(aDataSets)
	{

		name.add_CutWholeList(aName);

	}

	//default destructor
	~DataSetRow() {}

	//getter, used in population initialization,
	//assumes name wont be modified
	Word &get_Name()
	{

		return name;

	}

	//get a frameshift adjusted DataPoint value from the DataSet in columnIndex
	//position of dataSets based on dataFrameIndex, dataFrameSubIndex, and
	//shiftLength parameters as described in DataSet::get_FrameAdjustedDataPoint()
	double get_FrameAdjustedDataPoint(const unsigned short columnIndex,
		const unsigned long long dataFrameIndex, const unsigned short dataFrameSubIndex,
		const unsigned short shiftLength)
	{

		return dataSets.get_ElementAddress(
			columnIndex)->get_FrameAdjustedDataPoint(dataFrameIndex,
				dataFrameSubIndex, shiftLength);

	}

	//get a frameshift adjusted DataPoint address from the DataSet in columnIndex
	//position of dataSets based on dataFrameIndex, dataFrameSubIndex, and
	//shiftLength parameters as described in DataSet::get_FrameAdjustedDataPoint()
	double *get_FrameAdjustedDataPointAddress(const unsigned short columnIndex,
		const unsigned long long dataFrameIndex, const unsigned short dataFrameSubIndex,
		const unsigned short shiftLength)
	{

		return dataSets.get_ElementAddress(columnIndex)->get_FrameAdjustedDataPointAddress(
			dataFrameIndex, dataFrameSubIndex, shiftLength);

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		outputFile << "\n\t\t\t\t<name>";
		name.output_ToOpenFile(outputFile);
		outputFile << "</name>";

		outputFile << "\n\t\t\t\t<dataSets>";
		const unsigned short length = dataSets.get_ArrayLength();

		for (unsigned short count = 0; count < length; count++)
		{

			outputFile << "\n\t\t\t\t\t<ds_" << count << ">";

			dataSets.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);

			outputFile << "\n\t\t\t\t\t</ds_" << count << ">";

		}
		outputFile << "\n\t\t\t\t</dataSets>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		outputFile << "\"name\":\"";
		name.output_ToOpenFile(outputFile);
		outputFile << "\",";

		const unsigned short length (dataSets.get_ArrayLength() - 1);
		unsigned short count(0);

		outputFile << "\"dataSets\":[";

		while (count < length)
		{

			outputFile << "{";
			dataSets.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

			count++;

		}

		outputFile << "{";
		dataSets.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);

		outputFile << "}]";

	}

};

class DataSetColumn
{

private:

	//identifier
	Word name;

	//frame control variables	
	unsigned short frameLength; //viewing frame length
	unsigned short shiftLength; //frame shift length

public:

	//default constructor (generally safe null state)
	DataSetColumn() : name("unidentified_column"), frameLength(1), shiftLength(0) {}

	//modified constructor - used for population-validated construction
	//assumes parameters have been validated in population context;
	//assumes aName is populated; assumes (population's totalFrameCount * aShiftLength)
	//+ aFrameLength == dataLength, where dataLength = the dataPoints array
	//length for any/all 'contained DataSets; moves contents of reference parameter aName to name
	DataSetColumn(Word &aName, const unsigned short aFrameLength,
		const unsigned short aShiftLength) : frameLength(aFrameLength),
		shiftLength(aShiftLength)
	{

		name.add_CutWholeList(aName);

	}

	//default destructor
	~DataSetColumn() {}

	//getter
	unsigned short get_FrameLength() const
	{

		return frameLength;

	}

	//getter
	unsigned short get_ShiftLength() const
	{

		return shiftLength;

	}

	//getter, used in population initialization,
	//assumes name wont be modified
	Word &get_Name()
	{

		return name;

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		outputFile << "\n\t\t\t\t<name>";
		name.output_ToOpenFile(outputFile);
		outputFile << "</name>";

		outputFile << "\n\t\t\t\t<frameLength>" << frameLength << "</frameLength>";
		outputFile << "\n\t\t\t\t<shiftLength>" << shiftLength << "</shiftLength>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		outputFile << "\"name\":\"";
		name.output_ToOpenFile(outputFile);
		outputFile << "\",";

		outputFile << "\"frameLength\":" << frameLength << ",";
		outputFile << "\"shiftLength\":" << shiftLength;

	}

};

class DataSetCollection
{

private:

	Word name;

	ExtensibleArray<DataSetColumn> columns;
	ExtensibleArray<DataSetRow> rows;

public:

	//default constructor - generally safe null state
	DataSetCollection() : name("unidentified_dataset_collection"), columns(1), rows(1) {}

	//modified constructor - assumes the number of DataSets in each member of aRows
	//== the list length of aColumns; assumes, each and every DataSet 'contained'
	//in a given column have the same number of DataPoints and is consistent with
	//the equation: (population's totalFrameCount * aShiftLength) + aFrameLength == dataLength
	//(where dataLength = the dataPoints array length for any/all 'contained DataSets)
	DataSetCollection(Word &aName, UniformList<DataSetRow> &aRows,
		UniformList<DataSetColumn> aColumns) : name(aName), columns(aColumns), rows(aRows) {}

	//default destructor
	~DataSetCollection() {}

	//get a frameshift adjusted DataPoint value from the DataSet at rowIndex
	//and columnIndex positions and based on dataFrameIndex, dataFrameSubIndex, and
	//shiftLength parameters as described in DataSet::get_FrameAdjustedDataPoint()
	double get_FrameAdjustedDataPoint(const unsigned long long rowIndex,
		const unsigned short columnIndex, const unsigned long long dataFrameIndex,
		const unsigned short dataFrameSubIndex)
	{

		return rows.get_ElementAddress(rowIndex)->get_FrameAdjustedDataPoint(
			columnIndex, dataFrameIndex, dataFrameSubIndex,
			columns.get_ElementAddress(columnIndex)->get_ShiftLength());

	}

	//get a frameshift adjusted DataPoint address from the DataSet at rowIndex
	//and columnIndex positions and based on dataFrameIndex, dataFrameSubIndex, and
	//shiftLength parameters as described in DataSet::get_FrameAdjustedDataPoint()
	double *get_FrameAdjustedDataPointAddress(const unsigned long long rowIndex,
		const unsigned short columnIndex, const unsigned long long dataFrameIndex,
		const unsigned short dataFrameSubIndex)
	{

		return rows.get_ElementAddress(rowIndex)->get_FrameAdjustedDataPointAddress(
			columnIndex, dataFrameIndex, dataFrameSubIndex,
			columns.get_ElementAddress(columnIndex)->get_ShiftLength());

	}

	//getter
	unsigned short get_ColumnFrameLength(const unsigned short columnIndex) const
	{

		return columns.get_ElementAddress(columnIndex)->get_FrameLength();

	}

	//getter
	unsigned short get_ColumnShiftLength(const unsigned short columnIndex) const
	{

		return columns.get_ElementAddress(columnIndex)->get_ShiftLength();

	}

	//getter, used in population reporting,
	//assumes name wont be modified
	Word &get_Name()
	{

		return name;

	}

	//getter, used in population initialization,
	//assumes name wont be modified
	Word &get_ColumnName(const unsigned short columnIndex)
	{

		return columns.get_ElementAddress(columnIndex)->get_Name();

	}

	//getter, used in population read recording,
	//assumes name wont be modified
	Word &get_RowName(const unsigned short rowIndex)
	{

		return rows.get_ElementAddress(rowIndex)->get_Name();

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		unsigned short columnCount(columns.get_ArrayLength());
		unsigned long long rowCount(rows.get_ArrayLength());
		unsigned long long count;

		outputFile << "\n\t\t<name>";
		name.output_ToOpenFile(outputFile);
		outputFile << "\n\t\t</name>";

		outputFile << "\n\t\t<columns>";
		for (count = 0; count < columnCount; count++)
		{

			outputFile << "\n\t\t\t<c_" << count << ">";
			columns.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t\t</c_" << count << ">";

		}	
		outputFile << "\n\t\t</columns>";

		outputFile << "\n\t\t<rows>";
		for (count = 0; count < rowCount; count++)
		{

			outputFile << "\n\t\t\t<r_" << count << ">";
			rows.get_ElementAddress(count)->output_ToOpenFile_XML(outputFile);
			outputFile << "\n\t\t\t</r_" << count << ">";

		}
		outputFile << "\n\t\t</rows>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		unsigned short columnCount(columns.get_ArrayLength() - 1);
		unsigned long long rowCount(rows.get_ArrayLength() - 1);
		unsigned long long count;

		outputFile << "\"name\":\"";
		name.output_ToOpenFile(outputFile);
		outputFile << "\",";

		outputFile << "\"columns\":[";
		for (count = 0; count < columnCount; count++)
		{

			outputFile << "{";
			columns.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

		}

		outputFile << "{";
		columns.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
		outputFile << "}],";

		outputFile << "\"rows\":[";
		for (count = 0; count < rowCount; count++)
		{

			outputFile << "{";
			rows.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
			outputFile << "},";

		}

		outputFile << "{";
		rows.get_ElementAddress(count)->output_ToOpenFile_JSON(outputFile);
		outputFile << "}]";

	}

};

//stores and provides access to a single calculated input value used to stimulate nets;
//for data intended to change at runtime

class Input_Calculated
{

private:

	//identifier
	Word name;

	//main functional member variables
	double defaultValue;
	double workingValue;

public:

	//default constructor (generally safe null state)
	Input_Calculated() : name("unidentified"), defaultValue(0.0) {}

	//modified constructor - assumes parameters have been validated
	//in population context; assumes aName is populated
	Input_Calculated(const double aDefaultValue, Word &aName)
	{
	
		defaultValue = aDefaultValue;

		//move the name from the reference word parameter
		name.add_CutWholeList(aName);
	
	}

	//default destructor
	~Input_Calculated() {}

	//getter for current workingValue
	double get_CurrentValue() const
	{

		return workingValue;

	}

	//used to get workingValue address in population initialization, elsewhere?
	double *get_WorkingValueAddress()
	{

		return &workingValue;

	}

	//getter, used in population initialization,
	//assumes name wont be modified
	Word &get_Name()
	{

		return name;

	}

	//setter
	void set_CurrentValue(const double newValue)
	{

		workingValue = newValue;

	}

	//purpose: resets workingValue to equal defaultValue (called after each data set
	void reset()
	{

		workingValue = defaultValue;

	}

	//assumes that outputFile is already open
	void outputName_ToOpenFile(ofstream &outputFile)
	{

		name.output_ToOpenFile(outputFile);

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		outputFile << "\n\t\t\t\t<name>";
		name.output_ToOpenFile(outputFile);
		outputFile << "</name>";

		outputFile << "\n\t\t\t\t<defaultValue>";
		outputDouble_ToOpenFile(defaultValue, outputFile, 15);
		outputFile << "</defaultValue>";
	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		outputFile << "\"name\":\"";
		name.output_ToOpenFile(outputFile);
		outputFile << "\",";

		outputFile << "\"defaultValue\":";
		outputDouble_ToOpenFile(defaultValue, outputFile, 15);

	}

};

#endif // INPUT_H_INCLUDED