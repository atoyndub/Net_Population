#ifndef STRINGTOOLS_H_INCLUDED
#define STRINGTOOLS_H_INCLUDED

#include <iostream>
#include <fstream>
#include "MemoryTools.h"

using namespace std;

//outputs doubleToOutput to outputFile rounded to the number digits
//(pre and/or post-decimal point) represented by absolutePrecision;
//leading zeros (pre and post-decimal point) are not counted
//toward absolutePrecision; trailing zeros are not included-
//assumes absolutePrecision > 0; assumes outputFile is already
//open; assumes the integer portion and any desired decimal portion
//of doubleToReturn are each individually less than 18 digits long?;
void outputDouble_ToOpenFile(double doubleToOutput,
	ofstream &outputFile, unsigned char absolutePrecision)
{

	bool negative(false);

	if (doubleToOutput < 0.0)
	{

		negative = true;
		doubleToOutput *= -1;

	}

	UniformList<unsigned short> integerPortion;
	UniformNode<unsigned short> *digitNode;
	unsigned long long workingValue(doubleToOutput);

	//create integerPortion
	do
	{

		integerPortion.add_NewNode(workingValue % 10, NULL, false);
		workingValue /= 10;

	} while (workingValue);

	workingValue = doubleToOutput;

	unsigned char integerPortionLength(integerPortion.get_ListLength());

	//doubleToOutput has no decimal component
	if (workingValue == doubleToOutput)
	{

		if (absolutePrecision < integerPortionLength)
		{

			digitNode = integerPortion.get_NodeAddress(absolutePrecision - 1);
			unsigned char count(integerPortionLength - absolutePrecision);

			//round up if necessary
			if (digitNode->get_NextNode()->content > 4)
			{

				do
				{

					if (digitNode->content == 9)
					{

						digitNode->content = 0;
						digitNode = digitNode->get_PriorNode();

					}

					else
					{

						digitNode->content++;
						break;

					}

				} while (digitNode);

				if (!digitNode)
				{

					integerPortion.add_NewNode(1, NULL, false);
					count++;

				}

			}

			//output everything
			if (negative)
				outputFile << "-";

			digitNode = integerPortion.get_FirstNode();
			do
			{

				outputFile << digitNode->content;

				digitNode = digitNode->get_NextNode();
				absolutePrecision--;

			} while (absolutePrecision);

			do
			{

				outputFile << "0";
				count--;

			} while (count);

		}

		else
		{

			//output everything
			digitNode = integerPortion.get_FirstNode();

			if (integerPortionLength == 1 && digitNode->content == 0)
				outputFile << "0";

			else
			{

				if (negative)
					outputFile << "-";

				do
				{

					outputFile << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

			}

		}

	}

	//doubleToOutput has a decimal component
	else
	{

		//the decimal portion will not be included
		if (absolutePrecision < integerPortionLength)
		{

			digitNode = integerPortion.get_NodeAddress(absolutePrecision - 1);

			//round up if necessary
			if (digitNode->get_NextNode()->content > 4)
			{

				do
				{

					if (digitNode->content == 9)
					{

						digitNode->content = 0;
						digitNode = digitNode->get_PriorNode();

					}

					else
					{

						digitNode->content++;
						break;

					}

				} while (digitNode);

				if (!digitNode)
					integerPortion.add_NewNode(1, NULL, false);

			}

			absolutePrecision = integerPortion.get_ListLength() - absolutePrecision;

			integerPortion.delete_Range(integerPortion.get_LastNode(),
				absolutePrecision, false);

			//output everything
			if (negative)
				outputFile << "-";

			digitNode = integerPortion.get_FirstNode();

			do
			{

				outputFile << digitNode->content;

				digitNode = digitNode->get_NextNode();

			} while (digitNode);

			do
			{

				outputFile << "0";
				absolutePrecision--;

			} while (absolutePrecision);

		}

		//the decimal portion may be included if not rounded to zero or one
		else
		{

			bool zeroInteger(false);

			if (integerPortionLength == 1 && integerPortion.get_FirstNode()->content == 0)
				zeroInteger = true;

			//absolute precision begins at the first significant decimal digit
			if (zeroInteger)
			{

				UniformList<unsigned short> decimalPortion;

				doubleToOutput -= workingValue;

				//create decimalPortion
				do
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;
					workingValue = workingValue % 10;

					decimalPortion.add_NewNode(workingValue);

					if (workingValue)
					{

						absolutePrecision--;
						break;

					}

				} while (true);

				while (absolutePrecision)
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;

					decimalPortion.add_NewNode(workingValue % 10);

					absolutePrecision--;

				}

				//round up if necessary
				doubleToOutput *= 10;
				workingValue = doubleToOutput;

				if (workingValue % 10 > 4)
				{

					digitNode = decimalPortion.get_LastNode();

					do
					{

						if (digitNode->content == 9)
						{

							digitNode->content = 0;
							digitNode = digitNode->get_PriorNode();

						}

						else
						{

							digitNode->content++;
							break;

						}

					} while (digitNode);

					//decimal portion has rounded to one
					if (!digitNode)
					{

						if (negative)
							outputFile << "-";

						outputFile << "1";

						return;

					}

				}

				if (negative)
					outputFile << "-";

				digitNode = integerPortion.get_FirstNode();

				do
				{

					outputFile << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

				outputFile << ".";

				//remove any trailing zeros from decimalPortion
				digitNode = decimalPortion.get_LastNode();

				while (digitNode->content == 0)
					digitNode = decimalPortion.delete_Node(digitNode, false);

				//output the remaining decimal portion
				digitNode = decimalPortion.get_FirstNode();

				do
				{

					outputFile << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

			}

			//integer portion is not zero; absolute precision began in 
			//integer portion and continues into decimal portion
			else if (absolutePrecision > integerPortionLength)
			{

				UniformList<unsigned short> decimalPortion;
				bool zeroDecimal(true);

				doubleToOutput -= workingValue;

				//create decimalPortion
				absolutePrecision -= integerPortionLength;

				do
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;
					workingValue = workingValue % 10;

					decimalPortion.add_NewNode(workingValue);

					absolutePrecision--;

					if (workingValue)
					{

						zeroDecimal = false;
						break;

					}

				} while (absolutePrecision);

				while (absolutePrecision)
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;

					decimalPortion.add_NewNode(workingValue % 10);

					absolutePrecision--;

				}

				//round up if necessary
				doubleToOutput *= 10;
				workingValue = doubleToOutput;

				if (workingValue % 10 > 4)
				{

					zeroDecimal = false;

					digitNode = decimalPortion.get_LastNode();

					do
					{

						if (digitNode->content == 9)
						{

							digitNode->content = 0;
							digitNode = digitNode->get_PriorNode();

						}

						else
						{

							digitNode->content++;
							break;

						}

					} while (digitNode);

					//decimal portion has rounded to one
					if (!digitNode)
					{

						zeroDecimal = true;

						digitNode = integerPortion.get_LastNode();

						do
						{

							if (digitNode->content == 9)
							{

								digitNode->content = 0;
								digitNode = digitNode->get_PriorNode();

							}

							else
							{

								digitNode->content++;
								break;

							}

						} while (digitNode);

						if (!digitNode)
							integerPortion.add_NewNode(1, NULL, false);

					}

				}

				//output everything
				if (negative)
					outputFile << "-";

				digitNode = integerPortion.get_FirstNode();

				do
				{

					outputFile << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

				if (!zeroDecimal)
				{

					outputFile << ".";

					//remove any trailing zeros from decimalPortion
					digitNode = decimalPortion.get_LastNode();

					while (digitNode->content == 0)
						digitNode = decimalPortion.delete_Node(digitNode, false);

					//output the remaining decimal portion
					digitNode = decimalPortion.get_FirstNode();

					do
					{

						outputFile << digitNode->content;

						digitNode = digitNode->get_NextNode();

					} while (digitNode);

				}

			}

			else //absolutePrecision == integerLength && !zeroInteger
			{

				doubleToOutput -= workingValue;

				//round up if necessary
				doubleToOutput *= 10;
				workingValue = doubleToOutput;

				if (workingValue % 10 > 4)
				{

					digitNode = integerPortion.get_LastNode();

					do
					{

						if (digitNode->content == 9)
						{

							digitNode->content = 0;
							digitNode = digitNode->get_PriorNode();

						}

						else
						{

							digitNode->content++;
							break;

						}

					} while (digitNode);

					if (!digitNode)
						integerPortion.add_NewNode(1, NULL, false);

				}

				//output everything
				if (negative)
					outputFile << "-";

				digitNode = integerPortion.get_FirstNode();

				do
				{

					outputFile << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

			}

		}

	}

}

//outputs doubleToOutput to console rounded to the number digits
//(pre and/or post-decimal point) represented by absolutePrecision;
//leading zeros (pre and post-decimal point) are not counted
//toward absolutePrecision; trailing zeros are not included-
//assumes absolutePrecision > 0; assumes the integer portion and
//any desired decimal portion of doubleToReturn are each individually
//less than 18 digits long?;
void outputDouble_ToConsole(double doubleToOutput,
unsigned char absolutePrecision)
{

	bool negative(false);

	if (doubleToOutput < 0.0)
	{

		negative = true;
		doubleToOutput *= -1;

	}

	UniformList<unsigned short> integerPortion;
	UniformNode<unsigned short> *digitNode;
	unsigned long long workingValue(doubleToOutput);

	//create integerPortion
	do
	{

		integerPortion.add_NewNode(workingValue % 10, NULL, false);
		workingValue /= 10;

	} while (workingValue);

	workingValue = doubleToOutput;
	
	unsigned char integerPortionLength(integerPortion.get_ListLength());

	//doubleToOutput has no decimal component
	if (workingValue == doubleToOutput)
	{

		if (absolutePrecision < integerPortionLength)
		{

			digitNode = integerPortion.get_NodeAddress(absolutePrecision - 1);
			unsigned char count(integerPortionLength - absolutePrecision);

			//round up if necessary
			if (digitNode->get_NextNode()->content > 4)
			{

				do
				{

					if (digitNode->content == 9)
					{

						digitNode->content = 0;
						digitNode = digitNode->get_PriorNode();

					}

					else
					{

						digitNode->content++;
						break;

					}

				} while (digitNode);

				if (!digitNode)
				{

					integerPortion.add_NewNode(1, NULL, false);
					count++;

				}
					
			}

			//output everything
			if (negative)
				cout << "-";

			digitNode = integerPortion.get_FirstNode();
			do
			{

				cout << digitNode->content;
				
				digitNode = digitNode->get_NextNode();
				absolutePrecision--;

			} while (absolutePrecision);

			do
			{

				cout << "0";
				count--;

			} while (count);

		}

		else
		{

			//output everything
			digitNode = integerPortion.get_FirstNode();

			if (integerPortionLength == 1 && digitNode->content == 0)
				cout << "0";

			else
			{

				if (negative)
					cout << "-";

				do
				{

					cout << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

			}

		}

	}

	//doubleToOutput has a decimal component
	else
	{

		//the decimal portion will not be included
		if (absolutePrecision < integerPortionLength)
		{

			digitNode = integerPortion.get_NodeAddress(absolutePrecision - 1);

			//round up if necessary
			if (digitNode->get_NextNode()->content > 4)
			{

				do
				{

					if (digitNode->content == 9)
					{

						digitNode->content = 0;
						digitNode = digitNode->get_PriorNode();

					}

					else
					{

						digitNode->content++;
						break;

					}

				} while (digitNode);

				if (!digitNode)
					integerPortion.add_NewNode(1, NULL, false);

			}

			absolutePrecision = integerPortion.get_ListLength() - absolutePrecision;

			integerPortion.delete_Range(integerPortion.get_LastNode(),
				absolutePrecision, false);

			//output everything
			if (negative)
				cout << "-";

			digitNode = integerPortion.get_FirstNode();
			
			do
			{

				cout << digitNode->content;

				digitNode = digitNode->get_NextNode();

			} while (digitNode);

			do
			{

				cout << "0";
				absolutePrecision--;

			} while (absolutePrecision);

		}

		//the decimal portion may be included if not rounded to zero or one
		else
		{

			bool zeroInteger(false);

			if (integerPortionLength == 1 && integerPortion.get_FirstNode()->content == 0)
				zeroInteger = true;

			//absolute precision begins at the first significant decimal digit
			if (zeroInteger)
			{

				UniformList<unsigned short> decimalPortion;

				doubleToOutput -= workingValue;

				//create decimalPortion
				do
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;
					workingValue = workingValue % 10;

					decimalPortion.add_NewNode(workingValue);

					if (workingValue)
					{

						absolutePrecision--;
						break;

					}

				} while (true);

				while (absolutePrecision)
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;

					decimalPortion.add_NewNode(workingValue % 10);

					absolutePrecision--;

				}

				//round up if necessary
				doubleToOutput *= 10;
				workingValue = doubleToOutput;

				if (workingValue % 10 > 4)
				{

					digitNode = decimalPortion.get_LastNode();

					do
					{

						if (digitNode->content == 9)
						{

							digitNode->content = 0;
							digitNode = digitNode->get_PriorNode();

						}

						else
						{

							digitNode->content++;
							break;

						}

					} while (digitNode);

					//decimal portion has rounded to one
					if (!digitNode)
					{

						if (negative)
							cout << "-";

						cout << "1";
						
						return;

					}

				}

				if (negative)
					cout << "-";

				digitNode = integerPortion.get_FirstNode();

				do
				{

					cout << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

				cout << ".";

				//remove any trailing zeros from decimalPortion
				digitNode = decimalPortion.get_LastNode();

				while (digitNode->content == 0)
					digitNode = decimalPortion.delete_Node(digitNode, false);

				//output the remaining decimal portion
				digitNode = decimalPortion.get_FirstNode();

				do
				{

					cout << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

			}

			//integer portion is not zero; absolute precision began in 
			//integer portion and continues into decimal portion
			else if (absolutePrecision > integerPortionLength)
			{

				UniformList<unsigned short> decimalPortion;
				bool zeroDecimal(true);

				doubleToOutput -= workingValue;

				//create decimalPortion
				absolutePrecision -= integerPortionLength;

				do
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;
					workingValue = workingValue % 10;

					decimalPortion.add_NewNode(workingValue);

					absolutePrecision--;

					if (workingValue)
					{

						zeroDecimal = false;
						break;

					}

				} while (absolutePrecision);

				while (absolutePrecision)
				{

					doubleToOutput *= 10;
					workingValue = doubleToOutput;

					decimalPortion.add_NewNode(workingValue % 10);

					absolutePrecision--;

				}

				//round up if necessary
				doubleToOutput *= 10;
				workingValue = doubleToOutput;

				if (workingValue % 10 > 4)
				{

					zeroDecimal = false;

					digitNode = decimalPortion.get_LastNode();

					do
					{

						if (digitNode->content == 9)
						{

							digitNode->content = 0;
							digitNode = digitNode->get_PriorNode();

						}

						else
						{

							digitNode->content++;
							break;

						}

					} while (digitNode);

					//decimal portion has rounded to one
					if (!digitNode)
					{

						zeroDecimal = true;

						digitNode = integerPortion.get_LastNode();

						do
						{

							if (digitNode->content == 9)
							{

								digitNode->content = 0;
								digitNode = digitNode->get_PriorNode();

							}

							else
							{

								digitNode->content++;
								break;

							}

						} while (digitNode);

						if (!digitNode)
							integerPortion.add_NewNode(1, NULL, false);

					}

				}

				//output everything
				if (negative)
					cout << "-";

				digitNode = integerPortion.get_FirstNode();

				do
				{

					cout << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

				if (!zeroDecimal)
				{

					cout << ".";

					//remove any trailing zeros from decimalPortion
					digitNode = decimalPortion.get_LastNode();

					while (digitNode->content == 0)
						digitNode = decimalPortion.delete_Node(digitNode, false);

					//output the remaining decimal portion
					digitNode = decimalPortion.get_FirstNode();

					do
					{

						cout << digitNode->content;

						digitNode = digitNode->get_NextNode();

					} while (digitNode);

				}

			}

			else //absolutePrecision == integerLength && !zeroInteger
			{

				doubleToOutput -= workingValue;

				//round up if necessary
				doubleToOutput *= 10;
				workingValue = doubleToOutput;

				if (workingValue % 10 > 4)
				{

					digitNode = integerPortion.get_LastNode();

					do
					{

						if (digitNode->content == 9)
						{

							digitNode->content = 0;
							digitNode = digitNode->get_PriorNode();

						}

						else
						{

							digitNode->content++;
							break;

						}

					} while (digitNode);

					if (!digitNode)
						integerPortion.add_NewNode(1, NULL, false);

				}

				//output everything
				if (negative)
					cout << "-";

				digitNode = integerPortion.get_FirstNode();

				do
				{

					cout << digitNode->content;

					digitNode = digitNode->get_NextNode();

				} while (digitNode);

			}

		}

	}

}

//returns true if the function successfully opens the file corresponding to fileName,
//false if fileName is NULL, an empty c-string, or if the corresponding file does not
//open (doesn't exist?)
bool open_InputFile(ifstream &inputFile, UniformList<char> &fileName)
{

	unsigned long long nameLength = fileName.get_ListLength();

	//fileName is empty
	if (!nameLength)
		return false;

	char* tempCharArray = new char[(nameLength + 1)];
	UniformNode<char> *charNode = fileName.get_LastNode();	

	tempCharArray[nameLength] = '\0';

	do
	{

		nameLength--;
		tempCharArray[nameLength] = charNode->content;

		charNode = charNode->get_PriorNode();

	} while (charNode);

	inputFile.open(tempCharArray);
	delete[] tempCharArray;

	//test whether file opened successfully
	if (inputFile.is_open())
		return true;

	else
		return false;

}

//returns true if the function successfully opens the file corresponding to fileName,
//false if fileName is NULL, an empty c-string, or if the corresponding file does not
//open (doesn't exist?)
bool open_OutputFile(ofstream &outputFile, UniformList<char> &fileName,
	const bool overwrite = true)
{

	unsigned long long nameLength = fileName.get_ListLength();

	//fileName is empty
	if (!nameLength)
		return false;

	char* tempCharArray = new char[(nameLength + 1)];
	UniformNode<char> *charNode = fileName.get_LastNode();

	tempCharArray[nameLength] = '\0';

	do
	{

		nameLength--;
		tempCharArray[nameLength] = charNode->content;

		charNode = charNode->get_PriorNode();

	} while (charNode);


	if (overwrite)
		outputFile.open(tempCharArray);

	else
		outputFile.open(tempCharArray, std::ofstream::app);
	
	delete[] tempCharArray;

	//test whether file opened successfully
	if (outputFile.is_open())
		return true;

	else
		return false;

}

class ValidationLetter_ConstructCode
{

private:

	//0 means disparate, 1 single range, 2 single range and disparate,
	//3 dual range, 4 dual range and disparate, 5 triple range, 6 triple range and disparate
	unsigned char type;

	//ValidationLetter variables
	ExtensibleArray<char> chars;
	bool inverted;
	unsigned short minLength;
	unsigned short maxLength;

public:

	//default constructor - null simple disparate
	ValidationLetter_ConstructCode() : type(0), chars(1), inverted(false),
		minLength(1), maxLength(1)
	{

		*(chars.get_ElementAddress(0)) = '\0';

	}

	//modified constructor - creates a single char disparate
	ValidationLetter_ConstructCode(const char singleChar) : type(0), chars(1),
		inverted(false), minLength(1), maxLength(1)
	{

		*(chars.get_ElementAddress(0)) = singleChar;

	}

	//modified constructor
	ValidationLetter_ConstructCode(const char aChars[], unsigned char aType,
		const bool aInverted = false, const unsigned short aMinLength = 1,
		const unsigned short aMaxLength = 1)
	{

		unsigned char count;
		unsigned char minValidCharsLength;

		if (aType < 7)
			minValidCharsLength = aType + 1;

		else
		{

			minValidCharsLength = 1;
			aType = 0;

		}

		//aChars is NULL
		if (!aChars)
		{

			chars.resize_NoCopy(minValidCharsLength);

			for (count = 0; count < minValidCharsLength; count++)
				*(chars.get_ElementAddress(count)) = '\0';

			type = aType;
			inverted = false;

			minLength = 1;
			maxLength = 1;

			return;

		}

		count = 0;

		while (aChars[count] != '\0')
			count++;

		//aChars is too short for this type
		if (count < minValidCharsLength)
		{

			chars.resize_NoCopy(minValidCharsLength);

			for (count = 0; count < minValidCharsLength; count++)
				*(chars.get_ElementAddress(count)) = '\0';

			type = aType;
			inverted = false;

			minLength = 1;
			maxLength = 1;

			return;

		}

		chars.resize_NoCopy(count);
		count--;

		//proceed to copy over content
		while (count)
		{

			*(chars.get_ElementAddress(count)) = aChars[count];
			count--;

		}

		*(chars.get_ElementAddress(count)) = aChars[count];

		//wrap up;
		type = aType;
		inverted = aInverted;

		minLength = aMinLength;

		if (aMaxLength >= aMinLength || aMaxLength == 0)
			maxLength = aMaxLength;

		else //aMinLength is higher than aMaxLength (not 0)
			maxLength = aMinLength;

	}

	//modified constructor (any aTypeCode)
	ValidationLetter_ConstructCode(UniformList<char> &aChars, unsigned char aType,
		bool aInverted = false, unsigned short aMinLength = 1,
		unsigned short aMaxLength = 1)
	{

		unsigned char count = aChars.get_ListLength();
		unsigned char minValidCharsLength;

		if (aType < 7)
			minValidCharsLength = aType + 1;

		else
		{

			minValidCharsLength = 1;
			aType = 0;

		}

		//aChars is too short for this type
		if (count < minValidCharsLength)
		{

			chars.resize_NoCopy(minValidCharsLength);

			for (count = 0; count < minValidCharsLength; count++)
				*(chars.get_ElementAddress(count)) = '\0';

			type = aType;
			inverted = false;

			minLength = 1;
			maxLength = 1;

			return;

		}

		chars.resize_NoCopy(count);
		count--;

		//proceed to copy over content
		UniformNode<char> *charNode = aChars.get_LastNode();

		while (charNode)
		{

			*(chars.get_ElementAddress(count)) = charNode->content;

			charNode = charNode->get_PriorNode();
			count--;

		}

		//wrap up;
		type = aType;
		inverted = aInverted;

		minLength = aMinLength;

		if (aMaxLength >= aMinLength || aMaxLength == 0)
			maxLength = aMaxLength;

		else //aMinLength is higher than aMaxLength (not 0)
			maxLength = aMinLength;

	}

	//getter
	unsigned char get_Type() const
	{

		return type;

	}

	//getter
	ExtensibleArray<char> &get_Chars()
	{

		return chars;

	}

	//getter
	bool get_Inverted() const
	{

		return inverted;

	}

	//getter
	unsigned short get_MinLength() const
	{

		return minLength;

	}

	//getter
	unsigned short get_MaxLength() const
	{

		return maxLength;

	}

};

class ValidationLetter
{

private:

	//switch controlling whether a meaningfull return value will be returned
	//where a test char is found in the word (false) or where it is not found (true)
	bool inverted;

	//variables controlling the min and max length of any sequenced repetions allowed
	//for this node in context of a search term; a minLength value of 0 means the node
	//is optional in context of a ValidationWord and maxLength value of 0 means that
	//infinite sequnced repetions of this node are allowed in context of a ValidationWord
	unsigned short minLength;
	unsigned short maxLength;

	//test block concept - a test block represents a contiguous range of char nodes
	//within a UniformList<char> which meet this searchTerm's criteria; the test block is a range
	//extending from a starting node (startPosition of setTestBlock()) to at least
	//a node at which minLength is satisfied (node before testBlockTerminal_Min), and at
	//most to a node extending the test block to maxLength (node before
	//testBlockTerminal_Max), or as close as possible to maxLength; the test block
	//concept is a construct which is implemented in partnership through SearchNode,
	//ValidationWord, and UniformList<char> classes

	//marks a UniformList<char> char node which comes immedately after the final node in this
	//SearchNode's current test block (the node at which test block of the next
	//SearchNode in the ValidationWord will begin); a NULL value indicates this SearchNode's
	//current test block extends to the final char node of the UniformList<char>
	UniformNode<char> *testBlockTerminal_Max;

	//marks a UniformList<char> char node which is the first possible value of testBlockTerminal_Max
	//in this SearchNode's current test block; a NULL value indicates this SearchNode's
	//minLength requirement is met at the final char node of the UniformList<char>
	UniformNode<char> *testBlockTerminal_Min;

	//only used in copying, refers to the type of the associated derived class:
	//0 means disparate, 1 means single range, 2 means single range and disparate,
	//3 means dual range, 4 means dual range and disparate, 5 means triple range,
	//6 means triple range and disparate
	unsigned char type;

public:

	//default constructor
	ValidationLetter() : inverted(false), minLength(1), maxLength(1),
		testBlockTerminal_Min(NULL), testBlockTerminal_Max(NULL), type(0) {}

	//modified constructor for various derived null states
	ValidationLetter(const unsigned char aType) : inverted(false), minLength(1), maxLength(1),
		testBlockTerminal_Min(NULL), testBlockTerminal_Max(NULL), type(aType) {}

	//modified constructor //assumes maxLength is 0 or >= minLength
	ValidationLetter(const bool aInverted, const unsigned short aMinLength,
		const unsigned short aMaxLength, const unsigned char aType) : inverted(aInverted),
		minLength(aMinLength), maxLength(aMaxLength), testBlockTerminal_Min(NULL),
		testBlockTerminal_Max(NULL), type(aType) {}

	//default destructor
	~ValidationLetter() {}

	//getter
	bool get_Inverted() const
	{

		return inverted;

	}

	//getter
	unsigned char get_Type() const
	{
	
		return type;
	
	}

	//main operating function - overwritten in all derived classes
	//and virtual here so that derived class overwrites can be called from
	//base class pointers
	virtual bool test_Char(const char charToTest) const
	{
	
		return false;
	
	}

	friend class ValidationWord;

};

class Disparate : public ValidationLetter
{

private:

	//char array used in test_Char()
	ExtensibleArray<char> chars;

public:

	//default constructor
	Disparate() : ValidationLetter(0), chars(1)
	{

		*(chars.get_ElementAddress(0)) = '\0';

	}

	//modified constructor
	Disparate(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 0)
	{

		if (constructCode.get_Type() == 0)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(1);
			*(chars.get_ElementAddress(0)) = '\0';

		}

	}

	//default destructor
	~Disparate() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		unsigned char count = 0;
		const unsigned char maxCount(chars.get_ArrayLength());

		if (!ValidationLetter::get_Inverted())
		{

			while (count < maxCount)
			{

				if (charToTest == chars.get_Element(count))
					return true;

				count++;

			}

			return false;

		}

		//inverted == true
		else
		{

			while (count < maxCount)
			{

				if (charToTest == chars.get_Element(count))
					return false;

				count++;

			}

			return true;

		}

	}

};

class SingleRange : public ValidationLetter
{

private:

	//stores chars used in test_Char function
	ExtensibleArray<char> chars;

public:

	//default constructor
	SingleRange() : ValidationLetter(1), chars(2)
	{

		*(chars.get_ElementAddress(0)) = '\0';
		*(chars.get_ElementAddress(1)) = '\0';

	}

	//modified constructor
	SingleRange(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 1)
	{

		if (constructCode.get_Type() == 1)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(2);
			*(chars.get_ElementAddress(0)) = '\0';
			*(chars.get_ElementAddress(1)) = '\0';

		}

	}

	//default destructor
	~SingleRange() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		if (!ValidationLetter::get_Inverted())
		{

			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return true;

			else
				return false;

		}

		//inverted == true
		else
		{

			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return false;

			else
				return true;

		}

	}

};

class SingleRangeAndDisparate : public ValidationLetter
{

private:

	//stores chars used in test_Char function
	ExtensibleArray<char> chars;

public:

	//default constructor
	SingleRangeAndDisparate() : ValidationLetter(2), chars(3)
	{

		for (unsigned char count = 0; count < 3; count++)
			*(chars.get_ElementAddress(count)) = '\0';

	}

	//modified constructor
	SingleRangeAndDisparate(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 2)
	{

		if (constructCode.get_Type() == 2)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(3);

			for (unsigned char count = 0; count < 3; count++)
				*(chars.get_ElementAddress(count)) = '\0';

		}

	}

	//default destructor
	~SingleRangeAndDisparate() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		if (!ValidationLetter::get_Inverted())
		{

			//range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return true;

			//disparate char test
			else
			{

				unsigned char count = 2;
				const unsigned char maxCount(chars.get_ArrayLength());

				while (count < maxCount)
				{

					if (charToTest == chars.get_Element(count))
						return true;

					count++;

				}

				return false;

			}

		}

		//inverted == true
		else
		{

			//range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return false;

			//disparate char test
			else
			{

				unsigned char count = 2;
				const unsigned char maxCount(chars.get_ArrayLength());

				while (count < maxCount)
				{

					if (charToTest == chars.get_Element(count))
						return false;

					count++;

				}

				return true;

			}

		}

	}

};

class DualRange : public ValidationLetter
{

private:

	//stores chars used in test_Char function
	ExtensibleArray<char> chars;

public:

	//default constructor
	DualRange() : ValidationLetter(3), chars(4)
	{

		for (unsigned char count = 0; count < 4; count++)
			*(chars.get_ElementAddress(count)) = '\0';

	}

	//modified constructor
	DualRange(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 3)
	{

		if (constructCode.get_Type() == 3)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(4);

			for (unsigned char count = 0; count < 4; count++)
				*(chars.get_ElementAddress(count)) = '\0';

		}

	}

	//default destructor
	~DualRange() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		if (!ValidationLetter::get_Inverted())
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return true;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return true;

			else
				return false;

		}

		//inverted == true
		else
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return false;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return false;

			else
				return true;

		}

	}

};

class DualRangeAndDisparate : public ValidationLetter
{

private:

	//stores chars used in test_Char function
	ExtensibleArray<char> chars;

public:

	//default constructor
	DualRangeAndDisparate() : ValidationLetter(4), chars(5)
	{

		for (unsigned char count = 0; count < 5; count++)
			*(chars.get_ElementAddress(count)) = '\0';

	}

	//modified constructor
	DualRangeAndDisparate(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 4)
	{

		if (constructCode.get_Type() == 4)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(5);

			for (unsigned char count = 0; count < 5; count++)
				*(chars.get_ElementAddress(count)) = '\0';

		}

	}

	//default destructor
	~DualRangeAndDisparate() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		if (!ValidationLetter::get_Inverted())
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return true;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return true;

			//disparate char test
			else
			{

				unsigned char count(4);
				const unsigned char maxCount(chars.get_ArrayLength());

				while (count < maxCount)
				{

					if (charToTest == chars.get_Element(count))
						return true;

					count++;

				}

				return false;

			}

		}

		//inverted == true
		else
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return false;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return false;

			//disparate char test
			else
			{

				unsigned char count(4);
				const unsigned char maxCount(chars.get_ArrayLength());

				while (count < maxCount)
				{

					if (charToTest == chars.get_Element(count))
						return false;

					count++;

				}

				return true;

			}

		}

	}

};

class TripleRange : public ValidationLetter
{

private:

	//stores chars used in test_Char function
	ExtensibleArray<char> chars;

public:

	//default constructor
	TripleRange() : ValidationLetter(5), chars(6)
	{

		for (unsigned char count = 0; count < 6; count++)
			*(chars.get_ElementAddress(count)) = '\0';

	}

	//modified constructor
	TripleRange(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 5)
	{

		if (constructCode.get_Type() == 5)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(6);

			for (unsigned char count = 0; count < 6; count++)
				*(chars.get_ElementAddress(count)) = '\0';

		}

	}

	//default destructor
	~TripleRange() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		if (!ValidationLetter::get_Inverted())
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return true;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return true;

			//third range test
			else if (charToTest >= chars.get_Element(4)
				&& charToTest <= chars.get_Element(5))
				return true;

			else
				return false;

		}

		//inverted == true
		else
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return false;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return false;

			//third range test
			else if (charToTest >= chars.get_Element(4)
				&& charToTest <= chars.get_Element(5))
				return false;

			else
				return true;

		}

	}

};

class TripleRangeAndDisparate : public ValidationLetter
{

private:

	//stores chars used in test_Char function
	ExtensibleArray<char> chars;

public:

	//default constructor
	TripleRangeAndDisparate() : ValidationLetter(6), chars(7)
	{

		for (unsigned char count = 0; count < 7; count++)
			*(chars.get_ElementAddress(count)) = '\0';

	}

	//modified constructor
	TripleRangeAndDisparate(ValidationLetter_ConstructCode &constructCode) :
		ValidationLetter(constructCode.get_Inverted(), constructCode.get_MinLength(),
			constructCode.get_MaxLength(), 6)
	{

		if (constructCode.get_Type() == 6)
			chars = constructCode.get_Chars();

		else
		{

			chars.resize_NoCopy(7);

			for (unsigned char count = 0; count < 7; count++)
				*(chars.get_ElementAddress(count)) = '\0';

		}

	}

	//default destructor
	~TripleRangeAndDisparate() {}

	//main operating function
	bool test_Char(const char charToTest) const
	{

		if (!ValidationLetter::get_Inverted())
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return true;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return true;

			//third range test
			else if (charToTest >= chars.get_Element(4)
				&& charToTest <= chars.get_Element(5))
				return true;

			//disparate char test
			else
			{

				unsigned char count(6);
				const unsigned char maxCount(chars.get_ArrayLength());

				while (count < maxCount)
				{

					if (charToTest == chars.get_Element(count))
						return true;

					count++;

				}

				return false;

			}

		}

		//inverted == true
		else
		{

			//first range test
			if (charToTest >= chars.get_Element(0)
				&& charToTest <= chars.get_Element(1))
				return false;

			//second range test
			else if (charToTest >= chars.get_Element(2)
				&& charToTest <= chars.get_Element(3))
				return false;

			//third range test
			else if (charToTest >= chars.get_Element(4)
				&& charToTest <= chars.get_Element(5))
				return false;

			//disparate char test
			else
			{

				unsigned char count(6);
				const unsigned char maxCount(chars.get_ArrayLength());

				while (count < maxCount)
				{

					if (charToTest == chars.get_Element(count))
						return false;

					count++;

				}

				return true;

			}

		}

	}

};

/* GENERAL CLASS ASSUMPTIONS ***********************************************************
1)	A ValidationWord will always have at least 1 non-optional SearchNode based on logic
embedded in constructors; ValidationWords cannot be constructed to an empty state
***************************************************************************************/

class ValidationWord
{

private:

	//pointers to sequenced char tests
	ExtensibleArray<ValidationLetter*> charTests;

	//storage lists
	UniformList<Disparate> disparates;
	UniformList<SingleRange> singleRanges;
	UniformList<SingleRangeAndDisparate> singleRangeAndDisparates;
	UniformList<DualRange> dualRanges;
	UniformList<DualRangeAndDisparate> dualRangeAndDisparates;
	UniformList<TripleRange> tripleRanges;
	UniformList<TripleRangeAndDisparate> tripleRangeAndDisparates;

	//Purpose: assign testBlockTerminal_Max and testBlockTerminal_Min pointers of
	//currentValidationLetter to a test block within subjectWord, based on startPosition; sets the
	//test block to its max starting size
	//returns: true if minLength requirement is met (even if testBlockTerminal_Min/Max
	//are NULL) and false if not
	//assumes: startPosition is not NULL, currentValidationLetter
	//is a member of charTests
	//Note: biases related operations to return longer rather than shorter found subwords
	inline bool setTestBlock(ValidationLetter *currentValidationLetter,
		UniformNode<char> *startPosition)
	{

		unsigned short count;

		//test whether the ValidationLetter meets minLength requirements in this position
		for (count = 0; count < currentValidationLetter->minLength; count++)
		{

			if (!startPosition || !currentValidationLetter->test_Char(startPosition->content))
				return false; //minLength requirement not met

			startPosition = startPosition->get_NextNode();

		}

		currentValidationLetter->testBlockTerminal_Min = startPosition;

		//currentValidationLetter reached minLength at end of subjectWord
		if (currentValidationLetter->testBlockTerminal_Min == NULL)
		{

			currentValidationLetter->testBlockTerminal_Max = NULL;
			return true;

		}

		else //the word contains additional character(s)
		{

			if (currentValidationLetter->maxLength > 0) //maxLength is finite
			{

				unsigned short minMaxDifference = currentValidationLetter->maxLength - currentValidationLetter->minLength;

				for (count = 0; count < minMaxDifference; count++)
				{

					if (startPosition && currentValidationLetter->test_Char(startPosition->content))
						startPosition = startPosition->get_NextNode();

					else
						break;

				}

			}

			else //maxLength is infinite
			{

				//loop to find the first char not meeting this
				//ValidationLetter's singleValidationLetter()
				while (startPosition && currentValidationLetter->test_Char(startPosition->content))
					startPosition = startPosition->get_NextNode();

			}

			//maxLength is reached, UniformList<char> end is reached, or a char failed this
			//ValidationLetter's singleValidationLetter at the current value of startPosition
			currentValidationLetter->testBlockTerminal_Max = startPosition;
			return true;

		}

	}

	//Purpose: shorten currentValidationLetter's test block by one char node if possible
	//assumes: currentValidationLetter is a member of charTests
	//Returns: true if the operation is successfull, false if the test block
	//cannot be shortened any further
	bool iterateTestBlock(ValidationLetter *currentValidationLetter, UniformList<char> &subjectWord)
	{

		//in the case that testBlockTerminal_Max == testBlockTerminal_Min (whether
		//both are a single node address or both are NULL) the test block cannot
		//be shorteneed any further
		if (currentValidationLetter->testBlockTerminal_Max == currentValidationLetter->testBlockTerminal_Min)
			return false;

		//the current test block concludes at word end
		if (currentValidationLetter->testBlockTerminal_Max == NULL)
			currentValidationLetter->testBlockTerminal_Max = subjectWord.get_LastNode();

		//the current test block concludes before word end
		else
			currentValidationLetter->testBlockTerminal_Max = currentValidationLetter->testBlockTerminal_Max->get_PriorNode();

		return true;

	}

public:

	//default constructor
	ValidationWord()
	{

		charTests.resize_NoCopy(1);
		*(charTests.get_ElementAddress(0)) = &(disparates.add_NewNode()->content);

	}

	//modified constructor to create a simple ValidationWord
	//corresponding exactly to a C string
	ValidationWord(const char termRepresentation[])
	{

		if (termRepresentation && termRepresentation[0] != '\0')
		{

			unsigned long long count = 1;

			while (termRepresentation[count] != '\0')
				count++;

			charTests.resize_NoCopy(count);

			//copy in SearchNode contents
			ValidationLetter_ConstructCode tempConstructCode;
			count--;

			while (count)
			{

				tempConstructCode = ValidationLetter_ConstructCode(termRepresentation[count]);

				*(charTests.get_ElementAddress(count)) = &(disparates.add_NewNode(
					Disparate(tempConstructCode), NULL, false)->content);

				count--;

			}

			tempConstructCode = ValidationLetter_ConstructCode(termRepresentation[count]);

			*(charTests.get_ElementAddress(count)) = &(disparates.add_NewNode(
				Disparate(tempConstructCode), NULL, false)->content);

		}

		//set to null state
		else
		{

			charTests.resize_NoCopy(1);
			*(charTests.get_ElementAddress(0)) = &(disparates.add_NewNode()->content);

		}

	}

	//modified constructor to create a simple ValidationWord
	//corresponding exactly to a UniformList<char>
	ValidationWord(UniformList<char> &termRepresentation)
	{

		unsigned long long wordLength = termRepresentation.get_ListLength();

		if (wordLength)
		{

			charTests.resize_NoCopy(wordLength);

			//copy in SearchNode contents
			UniformNode<char> *charNode = termRepresentation.get_LastNode();
			ValidationLetter_ConstructCode tempConstructCode;
			wordLength--;

			while (wordLength)
			{

				tempConstructCode = ValidationLetter_ConstructCode(charNode->content);

				*(charTests.get_ElementAddress(wordLength)) = &(disparates.add_NewNode(
					Disparate(tempConstructCode), NULL, false)->content);

				charNode = charNode->get_PriorNode();
				wordLength--;

			}

			tempConstructCode = ValidationLetter_ConstructCode(charNode->content);

			*(charTests.get_ElementAddress(wordLength)) = &(disparates.add_NewNode(
				Disparate(tempConstructCode), NULL, false)->content);

		}

		//set to null state
		else
		{

			charTests.resize_NoCopy(1);
			*(charTests.get_ElementAddress(0)) = &(disparates.add_NewNode()->content);

		}

	}

	//modified constructor to create a ValidationWord with an undefined number
	//of ValidationLetters using a UniformList of existing ValidationLetter_ConstructCode objects
	ValidationWord(UniformList<ValidationLetter_ConstructCode> &constructCodeList)
	{

		UniformNode<ValidationLetter_ConstructCode> *iterationNode = constructCodeList.get_FirstNode();

		while (iterationNode)
		{

			if (iterationNode->content.get_MinLength())
			{

				iterationNode = constructCodeList.get_FirstNode();
				unsigned long long count = 0;

				charTests.resize_NoCopy(constructCodeList.get_ListLength());

				while (iterationNode)
				{

					//disparate
					if(iterationNode->content.get_Type() == 0)
						*(charTests.get_ElementAddress(count)) = &(disparates.add_NewNode(
							Disparate(iterationNode->content))->content);

					//single range
					else if (iterationNode->content.get_Type() == 1)
						*(charTests.get_ElementAddress(count)) = &(singleRanges.add_NewNode(
							SingleRange(iterationNode->content))->content);

					//single range and disparate
					else if(iterationNode->content.get_Type() == 2)
						*(charTests.get_ElementAddress(count)) = &(singleRangeAndDisparates.add_NewNode(
							SingleRangeAndDisparate(iterationNode->content))->content);

					//dual range
					else if (iterationNode->content.get_Type() == 3)
						*(charTests.get_ElementAddress(count)) = &(dualRanges.add_NewNode(
							DualRange(iterationNode->content))->content);

					//dual range and disparate
					else if (iterationNode->content.get_Type() == 4)
						*(charTests.get_ElementAddress(count)) = &(dualRangeAndDisparates.add_NewNode(
							DualRangeAndDisparate(iterationNode->content))->content);

					//triple range
					else if (iterationNode->content.get_Type() == 5)
						*(charTests.get_ElementAddress(count)) = &(tripleRanges.add_NewNode(
							TripleRange(iterationNode->content))->content);

					//triple and disparate
					else //(iterationNode->content.get_Type() == 6)
						*(charTests.get_ElementAddress(count)) = &(tripleRangeAndDisparates.add_NewNode(
							TripleRangeAndDisparate(iterationNode->content))->content);

					iterationNode = iterationNode->get_NextNode();
					count++;

				}

				return;

			}

			iterationNode = iterationNode->get_NextNode();

		}

		//set to null state if there is no non-optional SearchNode
		charTests.resize_NoCopy(1);
		*(charTests.get_ElementAddress(0)) = &(disparates.add_NewNode()->content);

	}

	//default destructor
	~ValidationWord() {}

	//copy constructor
	ValidationWord(ValidationWord& otherTerm)
	{

		ValidationLetter *currentValidationLetter;

		UniformNode<Disparate> *disparatesNode = otherTerm.disparates.get_FirstNode();
		UniformNode<SingleRange> *singleRangesNode = otherTerm.singleRanges.get_FirstNode();
		UniformNode<SingleRangeAndDisparate> *singleRangeAndDisparatesNode = otherTerm.singleRangeAndDisparates.get_FirstNode();
		UniformNode<DualRange> *dualRangesNode = otherTerm.dualRanges.get_FirstNode();
		UniformNode<DualRangeAndDisparate> *dualRangeAndDisparatesNode = otherTerm.dualRangeAndDisparates.get_FirstNode();
		UniformNode<TripleRange> *tripleRangesNode = otherTerm.tripleRanges.get_FirstNode();
		UniformNode<TripleRangeAndDisparate> *tripleRangeAndDisparatesNode = otherTerm.tripleRangeAndDisparates.get_FirstNode();

		unsigned long long length(otherTerm.charTests.get_ArrayLength());
		charTests.resize_NoCopy(length);

		unsigned long long count = 0;

		do
		{

			currentValidationLetter = otherTerm.charTests.get_Element(count);

			if(currentValidationLetter->get_Type() == 0)
			{

				*(charTests.get_ElementAddress(count)) = &(disparates.add_NewNode(disparatesNode->content)->content);
				disparatesNode = disparatesNode->get_NextNode();

			}

			else if (currentValidationLetter->get_Type() == 1)
			{

				*(charTests.get_ElementAddress(count)) = &(singleRanges.add_NewNode(singleRangesNode->content)->content);
				singleRangesNode = singleRangesNode->get_NextNode();

			}
				
			else if (currentValidationLetter->get_Type() == 2)
			{

				*(charTests.get_ElementAddress(count)) = &(singleRangeAndDisparates.add_NewNode(singleRangeAndDisparatesNode->content)->content);
				singleRangeAndDisparatesNode = singleRangeAndDisparatesNode->get_NextNode();

			}
				
			else if (currentValidationLetter->get_Type() == 3)
			{

				*(charTests.get_ElementAddress(count)) = &(dualRanges.add_NewNode(dualRangesNode->content)->content);
				dualRangesNode = dualRangesNode->get_NextNode();

			}
				
			else if (currentValidationLetter->get_Type() == 4)
			{

				*(charTests.get_ElementAddress(count)) = &(dualRangeAndDisparates.add_NewNode(dualRangeAndDisparatesNode->content)->content);
				dualRangeAndDisparatesNode = dualRangeAndDisparatesNode->get_NextNode();

			}
				
			else if (currentValidationLetter->get_Type() == 5)
			{

				*(charTests.get_ElementAddress(count)) = &(tripleRanges.add_NewNode(tripleRangesNode->content)->content);
				tripleRangesNode = tripleRangesNode->get_NextNode();

			}

			else //(currentValidationLetter->get_Type() == 6)
			{

				*(charTests.get_ElementAddress(count)) = &(tripleRangeAndDisparates.add_NewNode(tripleRangeAndDisparatesNode->content)->content);
				tripleRangeAndDisparatesNode = tripleRangeAndDisparatesNode->get_NextNode();

			}

			count++;

		} while (count < length);

	}

	//copy assignment operator
	ValidationWord& operator=(ValidationWord const & otherTerm)
	{

		if (this == &otherTerm)
			return *this;

		ValidationLetter *currentValidationLetter;

		UniformNode<Disparate> *disparatesNode = otherTerm.disparates.get_FirstNode();
		UniformNode<SingleRange> *singleRangesNode = otherTerm.singleRanges.get_FirstNode();
		UniformNode<SingleRangeAndDisparate> *singleRangeAndDisparatesNode = otherTerm.singleRangeAndDisparates.get_FirstNode();
		UniformNode<DualRange> *dualRangesNode = otherTerm.dualRanges.get_FirstNode();
		UniformNode<DualRangeAndDisparate> *dualRangeAndDisparatesNode = otherTerm.dualRangeAndDisparates.get_FirstNode();
		UniformNode<TripleRange> *tripleRangesNode = otherTerm.tripleRanges.get_FirstNode();
		UniformNode<TripleRangeAndDisparate> *tripleRangeAndDisparatesNode = otherTerm.tripleRangeAndDisparates.get_FirstNode();

		unsigned long long length(otherTerm.charTests.get_ArrayLength());
		charTests.resize_NoCopy(length);

		unsigned long long count = 0;

		do
		{

			currentValidationLetter = otherTerm.charTests.get_Element(count);

			if (currentValidationLetter->get_Type() == 0)
			{

				*(charTests.get_ElementAddress(count)) = &(disparates.add_NewNode(disparatesNode->content)->content);
				disparatesNode = disparatesNode->get_NextNode();

			}

			else if (currentValidationLetter->get_Type() == 1)
			{

				*(charTests.get_ElementAddress(count)) = &(singleRanges.add_NewNode(singleRangesNode->content)->content);
				singleRangesNode = singleRangesNode->get_NextNode();

			}

			else if (currentValidationLetter->get_Type() == 2)
			{

				*(charTests.get_ElementAddress(count)) = &(singleRangeAndDisparates.add_NewNode(singleRangeAndDisparatesNode->content)->content);
				singleRangeAndDisparatesNode = singleRangeAndDisparatesNode->get_NextNode();

			}

			else if (currentValidationLetter->get_Type() == 3)
			{

				*(charTests.get_ElementAddress(count)) = &(dualRanges.add_NewNode(dualRangesNode->content)->content);
				dualRangesNode = dualRangesNode->get_NextNode();

			}

			else if (currentValidationLetter->get_Type() == 4)
			{

				*(charTests.get_ElementAddress(count)) = &(dualRangeAndDisparates.add_NewNode(dualRangeAndDisparatesNode->content)->content);
				dualRangeAndDisparatesNode = dualRangeAndDisparatesNode->get_NextNode();

			}

			else if (currentValidationLetter->get_Type() == 5)
			{

				*(charTests.get_ElementAddress(count)) = &(tripleRanges.add_NewNode(tripleRangesNode->content)->content);
				tripleRangesNode = tripleRangesNode->get_NextNode();

			}

			else //(currentValidationLetter->get_Type() == 6)
			{

				*(charTests.get_ElementAddress(count)) = &(tripleRangeAndDisparates.add_NewNode(tripleRangeAndDisparatesNode->content)->content);
				tripleRangeAndDisparatesNode = tripleRangeAndDisparatesNode->get_NextNode();

			}

			count++;

		} while (count < length);

		return *this;

	}

	/* BASIC WORD TESTING FUNCTIONS ***************************************************/

	//searches for a subword within subjectWord which matching this search term
	//returns: either found subword start or end node address or NULL if
	//a matching subword is not found
	//assumes: subjectWord is not empty and searchStartChar is either NULL or a member
	//of subjectWord; general class assumptions (SearchNode, ValidationWord)
	inline UniformNode<char> *testWord_ContainsTerm(UniformList<char> &subjectWord,
		UniformNode<char> *searchStartChar = NULL, bool returnFoundWordStart = true)
	{

		//initial setup
		if (!searchStartChar)
			searchStartChar = subjectWord.get_FirstNode();

		const unsigned long long termLength = charTests.get_ArrayLength();
		unsigned long long currentSearchNodeIndex = 0;
		UniformNode<char> *currentCharNode = searchStartChar;
		bool validTestBlock;

		//main testing loop
		do
		{

		NEXT_ROUND:

			validTestBlock = setTestBlock(charTests.get_Element(currentSearchNodeIndex),
				currentCharNode);

			//the currentSearchNode has identified a valid test block in this position
			if (validTestBlock)
			{

				currentCharNode = charTests.get_Element(
					currentSearchNodeIndex)->testBlockTerminal_Max;

				currentSearchNodeIndex++;

			}

			//the current SearchNode has no valid test block in this position
			else
			{

				if (currentSearchNodeIndex)
				{

					currentSearchNodeIndex--;

					do
					{

						validTestBlock = iterateTestBlock(charTests.get_Element(
							currentSearchNodeIndex), subjectWord);

						if (validTestBlock)
						{

							currentCharNode = charTests.get_Element(
								currentSearchNodeIndex)->testBlockTerminal_Max;

							currentSearchNodeIndex++;

							goto NEXT_ROUND;

						}

						if (currentSearchNodeIndex)
							currentSearchNodeIndex--;

						else
							break;

					} while (true);

				}

				//upon reaching this point, each SearchNode has tested and exhausted
				//in every possible iteration in the current position
				searchStartChar = searchStartChar->get_NextNode();

				if (searchStartChar)
				{

					currentCharNode = searchStartChar;
					currentSearchNodeIndex = 0;

				}

				else //the test has reached the Word end
					return NULL;

			}

		} while (currentSearchNodeIndex < termLength);

		//the last SearchNode completed it's test successfully
		if (returnFoundWordStart)
			return searchStartChar;

		else
		{

			if (currentCharNode) //found word end is an internal Word character
				return currentCharNode->get_PriorNode();

			else //found word end is the final Word character
				return subjectWord.get_LastNode();

		}

	}

	//searches for a subword within subjectWord which matching this search term
	//returns: both found word start and end node values via reference parameters
	//assumes: subjectWord is not empty and searchStartChar is either NULL or a member
	//of subjectWord; general class assumptions (SearchNode, ValidationWord)
	void testWord_ContainsTerm(UniformList<char> &subjectWord,
		UniformNode<char> *&returnNodeStart, UniformNode<char> *&returnNodeEnd,
		UniformNode<char> *searchStartChar = NULL)
	{

		//initial setup
		if (!searchStartChar)
			searchStartChar = subjectWord.get_FirstNode();

		const unsigned long long termLength = charTests.get_ArrayLength();
		unsigned long long currentSearchNodeIndex = 0;
		UniformNode<char> *currentCharNode = searchStartChar;
		bool validTestBlock;

		//main testing loop
		do
		{

		NEXT_ROUND:

			validTestBlock = setTestBlock(charTests.get_Element(currentSearchNodeIndex),
				currentCharNode);

			//the currentSearchNode has identified a valid test block in this position
			if (validTestBlock)
			{

				currentCharNode = charTests.get_Element(
					currentSearchNodeIndex)->testBlockTerminal_Max;

				currentSearchNodeIndex++;

			}

			//the current SearchNode has no valid test block in this position
			else
			{

				if (currentSearchNodeIndex)
				{

					currentSearchNodeIndex--;

					do
					{

						validTestBlock = iterateTestBlock(charTests.get_Element(
							currentSearchNodeIndex), subjectWord);

						if (validTestBlock)
						{

							currentCharNode = charTests.get_Element(
								currentSearchNodeIndex)->testBlockTerminal_Max;

							currentSearchNodeIndex++;

							goto NEXT_ROUND;

						}

						if (currentSearchNodeIndex)
							currentSearchNodeIndex--;

						else
							break;

					} while (true);

				}

				//upon reaching this point, each SearchNode has tested and exhausted
				//in every possible iteration in the current position
				searchStartChar = searchStartChar->get_NextNode();

				if (searchStartChar)
				{

					currentCharNode = searchStartChar;
					currentSearchNodeIndex = 0;

				}

				else //the test has reached the Word end
				{

					returnNodeStart = NULL;
					returnNodeEnd = NULL;
					return;

				}

			}

		} while (currentSearchNodeIndex < termLength);

		//the last SearchNode completed it's test successfully
		returnNodeStart = searchStartChar;

		if (currentCharNode) //found word end is an internal Word character
			returnNodeEnd = currentCharNode->get_PriorNode();

		else //found word end is the final Word character
			returnNodeEnd = subjectWord.get_LastNode();

	}

	//identifies whether the whole of subjectWord matches this ValidationWord
	//returns: true if the search term matches the whole word, false otherwise
	//assumes: subjectWord is not or empty; general class assumptions (SearchNode,
	//ValidationWord)
	inline bool testWord_MatchesTerm(UniformList<char> &subjectWord)
	{

		const unsigned long long termLength = charTests.get_ArrayLength();
		unsigned long long currentSearchNodeIndex = 0;
		UniformNode<char> *currentCharNode = subjectWord.get_FirstNode();
		bool validTestBlock;

		//main testing loop
		do
		{

		NEXT_ROUND:

			validTestBlock = setTestBlock(charTests.get_Element(currentSearchNodeIndex),
				currentCharNode);

			//the currentSearchNode has identified a valid test block in this position
			if (validTestBlock)
			{

				currentCharNode = charTests.get_Element(
					currentSearchNodeIndex)->testBlockTerminal_Max;

				currentSearchNodeIndex++;

			}

			//the current SearchNode has no valid test block in this position
			else
			{

				if (currentSearchNodeIndex)
				{

					currentSearchNodeIndex--;

					do
					{

						validTestBlock = iterateTestBlock(charTests.get_Element(
							currentSearchNodeIndex), subjectWord);

						if (validTestBlock)
						{

							currentCharNode = charTests.get_Element(
								currentSearchNodeIndex)->testBlockTerminal_Max;

							currentSearchNodeIndex++;

							goto NEXT_ROUND;

						}

						if (currentSearchNodeIndex)
							currentSearchNodeIndex--;

						else
							break;

					} while (true);

				}

				//upoin reaching this point, each SearchNode has tested and exhausted
				//in every possible iteration in the current position
				return false;

			}

		} while (currentSearchNodeIndex < termLength);

		//the last SearchNode completed it's test successfully

		if (!currentCharNode) //found word end the final Word character
			return true;

		else //found word end is not the final word character
			return false;

	}

};

/* WORD CLASS DEFINITION **************************************************************/

class Word : public UniformList<char>
{

public:

	//default constructor
	Word() : UniformList<char>() {}

	//modified constructor
	Word(const char string[]) : UniformList<char>()
	{

		unsigned long long count = 0;

		while (string[count] != '\0')
		{

			add_NewNode(string[count]);
			count++;

		}

	}

	//default destructor
	~Word() {}

	/* INTERPRETATION FUNCTIONS *******************************************************/

	//interprets thius word as a double regardless of whether it has valid floating
	//point structure; assumes this Word is within the range of possible double values
	//(if syntactically valid as a decimal floating point and if desired result is to
	//be interpreted as a meaningfull value); returns this Word's value interpreted as
	//a double, 0 if this Word is empty
	double interpret_Double()
	{

		//test whether list is empty
		if (!test_ListPopulated())
			return 0.0;

		short sign(1);
		double integerPart(0.0);
		double decimalPart(0.0);
		unsigned long long decimalPartDivisor(1);

		UniformNode<char> *charNode(get_FirstNode());

		//negative sign
		if (charNode->content == '-')
		{

			sign = -1;
			charNode = charNode->get_NextNode();

		}

		//integer part
		while (charNode)
		{

			if (charNode->content == '.')
			{

				charNode = charNode->get_NextNode();
				break;

			}

			integerPart = (integerPart * 10) + (charNode->content - '0');
			charNode = charNode->get_NextNode();

		}

		//decimal part
		while (charNode)
		{

			decimalPart = (decimalPart * 10) + (charNode->content - '0');
			decimalPartDivisor *= 10;

			charNode = charNode->get_NextNode();

		}

		//return the final value
		return (sign * (integerPart + (decimalPart / decimalPartDivisor)));

	}

	//interprets this word as an int regardless of whether it has valid integer syntax;
	//assumes this Word is within the range of possible int values (if syntactically
	//valid as an int and if desired result is to be interpreted as a meaningfull
	//value); returns this Word's value as an int, 0 if this Word is empty
	int interpret_Int()
	{

		//test whether list is empty
		if (!test_ListPopulated())
			return 0;

		short sign(1);
		int calculatedValue(0);
		UniformNode<char> *charNode(get_FirstNode());

		if (charNode->content == '-')
		{

			sign = -1;
			charNode = charNode->get_NextNode();

		}

		while (charNode)
		{

			calculatedValue = (calculatedValue * 10) + (charNode->content - '0');
			charNode = charNode->get_NextNode();

		}

		return sign * calculatedValue;

	}

	//interprets this Word as an unsigned char regardless of whether it has valid
	//unsigned integer syntax; assumes this Word is within the range of possible
	//unsigned char values (if syntactically valid as an unsigned char and if desired
	//result is to be interpreted as a meaningfull value); returns this Word's value as
	//an unsigned char; 0 if this Word is empty
	unsigned char interpret_UnsignedChar()
	{

		unsigned char calculatedValue(0);
		UniformNode<char> *charNode(get_FirstNode());

		while (charNode)
		{

			calculatedValue = (calculatedValue * 10) + (charNode->content - '0');
			charNode = charNode->get_NextNode();

		}

		return calculatedValue;

	}

	//interprets the word as an unsigned short regardless of whether it has valid
	//unsigned integer syntax; assumes this Word is within the range of possible
	//unsigned short values (if syntactically valid as an unsigned int and if desired
	//result is to be interpreted as a meaningfull value); returns this Word's value as
	//an unsigned short, 0 if this Word is empty
	unsigned short interpret_UnsignedShort()
	{

		unsigned short calculatedValue(0);
		UniformNode<char> *charNode(get_FirstNode());

		while (charNode)
		{

			calculatedValue = (calculatedValue * 10) + (charNode->content - '0');
			charNode = charNode->get_NextNode();

		}

		return calculatedValue;

	}

	//interprets this Word as an unsigned int regardless of whether it has valid unsigned
	//integer syntax; assumes this Word is within the range of possible unsigned int
	//values (if syntactically valid as an unsigned int and if desired result is to be
	//interpreted as a meaningfull value); returns this Word's value as an unsigned short,
	//0 if this Word is empty
	unsigned int interpret_UnsignedInt()
	{

		unsigned int calculatedValue(0);
		UniformNode<char> *charNode(get_FirstNode());

		while (charNode)
		{

			calculatedValue = (calculatedValue * 10) + (charNode->content - '0');
			charNode = charNode->get_NextNode();

		}

		return calculatedValue;

	}

	//interprets the word as an unsigned long long regardless of whether it has valid
	//unsigned integer syntax; assumes this Word is within the range of possible
	//unsigned long long values (if syntactically valid as an unsigned int and if
	//desired result is to be interpreted as a meaningfull value); returns this Word's
	//value as an unsigned long long, 0 if this Word is empty
	unsigned long long interpret_UnsignedLongLong()
	{

		unsigned long long calculatedValue(0);
		UniformNode<char> *charNode(get_FirstNode());

		while (charNode)
		{

			calculatedValue = (calculatedValue * 10) + (charNode->content - '0');
			charNode = charNode->get_NextNode();

		}

		return calculatedValue;

	}

	/* TRANSCRIPTION FUNCTIONS ********************************************************/
	
	//transcribes valueToTranscribe as a string of chars and writes them into this
	//word; simply appends, does not overwrite
	void transcribe_UnsignedShort(unsigned short valueToTranscribe)
	{

		//first iteration
		char currentChar((valueToTranscribe % 10) + 48);
		UniformNode<char> *charNode(add_NewNode(currentChar));
		valueToTranscribe /= 10;

		//subsequent iterations
		while (valueToTranscribe)
		{

			currentChar = (valueToTranscribe % 10) + 48;
			charNode = (add_NewNode(currentChar, charNode, false));
			valueToTranscribe /= 10;

		}

	}

	/* INPUT/OUTPUT FUNCTIONS *********************************************************/

	//populate this Word with a string of consol input chars terminated by '\n';
	//overwrites contents of this Word if overwrite is true, includes the terminal
	//'\n' char in this word if excludeTerminator is false; assumes '\n' is the last
	//char of user console input (otherwise any additional subsequent cin data may need)
	//to be cleared before the next input
	void getInput_Console(bool overwrite = false, bool excludeTerminator = true)
	{

		//tracking variable
		char nextChar = cin.get();

		if (overwrite)
		{

			UniformNode<char> *charNode = get_FirstNode();

			if (excludeTerminator)
			{

				//copy nextChar contents into charNode till the list runs
				//out of nodes, till '\n', or till the user ends input
				//manually (via ctrl + d or ctrl + z)
				while (charNode)
				{

					if (cin.eof() || nextChar == '\n')
					{

						delete_Range(charNode, get_LastNode());
						return;

					}

					else
					{

						charNode->content = nextChar;
						charNode = charNode->get_NextNode();

						nextChar = cin.get();

					}

				}

				//list is out of nodes, now add new nodes till
				//'\n', or till the user ends input manually
				//(via ctrl + d or ctrl + z)
				while (!cin.eof() && nextChar != '\n')
				{

					add_NewNode(nextChar);
					nextChar = cin.get();

				}

			}

			else //include the terminal '/n'
			{

				//copy nextChar contents into charNode till the list runs
				//out of nodes, till '\n', or till the user ends input
				//manually (via ctrl + d or ctrl + z)
				while (charNode)
				{

					if (cin.eof())
					{

						delete_Range(charNode, get_LastNode());
						return;

					}

					else if (nextChar != '\n')
					{

						charNode->content = nextChar;
						charNode = charNode->get_NextNode();

						nextChar = cin.get();

					}

					else //nextChar == '\n'
					{

						charNode->content = nextChar;
						charNode = charNode->get_NextNode();

						if (charNode)
							delete_Range(charNode, get_LastNode());

						return;

					}

				}

				//list is out of nodes, now add new nodes till
				//'\n', or till the user ends input manually
				//(via ctrl + d or ctrl + z)
				while (!cin.eof())
				{

					if (nextChar != '\n')
					{

						add_NewNode(nextChar);
						nextChar = cin.get();

					}

					else //nextChar == '\n'
					{

						add_NewNode(nextChar);
						return;

					}

				}

			}

		}

		else
		{

			if (excludeTerminator)
			{

				//add new nodes till '\n', or till the user ends input manually
				//(via ctrl + d or ctrl + z)
				while (!cin.eof() && nextChar != '\n')
				{

					add_NewNode(nextChar);
					nextChar = cin.get();

				}

			}

			else //include the terminal '/n'
			{

				//add new nodes till and including '\n', or till the user
				//ends input manually (via ctrl + d or ctrl + z)
				while (!cin.eof())
				{

					add_NewNode(nextChar);

					if (nextChar != '\n')
						nextChar = cin.get();

					else
						return;

				}

			}

		}

	}

	//populates this word with a string of chars from an input file; overwrites contents
	//of this Word if overwrite is true; does nothing if fileName does not correspond
	//to a valid file or if the file is empty
	void getInput_File(UniformList<char> &fileName, const bool overwrite = false)
	{

		ifstream inputFile;

		//open file corresponding to fileName or return if opening fails
		if (!open_InputFile(inputFile, fileName))
			return;

		char nextChar = inputFile.get();

		if (overwrite)
		{

			UniformNode<char> *charNode = get_FirstNode();

			//copy nextChar contents into charNode till the list runs
			//out of nodes or till the end of file
			while (charNode)
			{

				if (inputFile.eof())
				{

					delete_Range(charNode, get_LastNode());

					inputFile.close();
					return;

				}

				else
				{

					charNode->content = nextChar;
					charNode = charNode->get_NextNode();

					nextChar = inputFile.get();

				}

			}

			//list is out of nodes, now add new nodes till
			//the end of file
			while (!inputFile.eof())
			{

				add_NewNode(nextChar);
				nextChar = inputFile.get();

			}

		}

		else
		{

			//add new nodes till the end of file
			while (!inputFile.eof())
			{

				add_NewNode(nextChar);
				nextChar = inputFile.get();

			}

		}

		inputFile.close();

	}

	//outputs the characters stored in this Word to console
	void output_Console()
	{

		UniformNode<char> *charNode = get_FirstNode();

		while (charNode)
		{

			cout << charNode->content;
			charNode = charNode->get_NextNode();

		}

	}

	//outputs the characters stored in this Word to an unopened file
	void output_File(UniformList<char> &fileName, const bool overwrite = true)
	{

		if (test_ListPopulated())
		{

			ofstream outputFile;

			//open file corresponding to fileName or return if opening fails
			if (!open_OutputFile(outputFile, fileName, overwrite))
				return;

			UniformNode<char> *charNode = get_FirstNode();

			do
			{

				outputFile << charNode->content;
				charNode = charNode->get_NextNode();

			} while (charNode);

			outputFile.close();

		}

	}

	//output the characters stored in this word to a file which is already open;
	//assumes this Word is populated and outputFile is already open
	void output_ToOpenFile(ofstream &outputFile)
	{

		UniformNode<char> *charNode(get_FirstNode());

		do
		{

			outputFile << charNode->content;
			charNode = charNode->get_NextNode();

		} while (charNode);

	}

	/* OPERATOR OVERLOAD FUNCTIONS ****************************************************/

	//tests if this Word and otherWord contain the same number and sequence of chars;
	//returns true if so, false if not
	bool operator==(const Word& otherWord)
	{

		if (get_ListLength() == otherWord.get_ListLength())
		{

			//tracking nodes/variables
			UniformNode<char> *thisCharNode(get_FirstNode());
			UniformNode<char> *otherCharNode(otherWord.get_FirstNode());

			while (thisCharNode) //thisCharNode and otherCharNode will go to NULL at same time
			{

				if (thisCharNode->content != otherCharNode->content)
					return false;

				//iterate the tracking variables
				thisCharNode = thisCharNode->get_NextNode();
				otherCharNode = otherCharNode->get_NextNode();

			}

			return true;

		}

		else //words arent the same length
			return false;

	}

	//tests whether this Word and comparisonChars contain the same number and sequence
	//of chars; returns true if so, false if not
	bool operator==(const char comparisonChars[])
	{

		//tracking nodes/variables
		UniformNode<char> *charNode(get_FirstNode());
		unsigned long long count(0);

		while (charNode)
		{

			if (charNode->content != comparisonChars[count])
				return false;

			//iterate the tracking variables
			charNode = charNode->get_NextNode();
			count++;

		}

		if (comparisonChars[count] == '\0')
			return true; //char array is same length as word

		else
			return false; //char array is longer than word

	}

	//tests whether this Word and otherWord do not contain the same number and sequence of
	//chars; returns true if so, false otherwise
	bool operator!=(const Word& otherWord)
	{

		if (*this == otherWord)
			return false;

		else
			return true;

	}

	//tests whether this Word and comparisonChars do not contain the same number and
	//sequence of chars; returns true if so, false otherwise
	bool operator!=(const char comparisonChars[])
	{

		if (*this == comparisonChars)
			return false;

		else
			return true;

	}

	/* TERM FINDING FUNCTIONS *********************************************************/

	//searches for a subword within this word matching termToFind
	//returns: either found subword start or end node address or NULL if
	//a matching subword is not found
	//assumes: this word is not empty and searchStartChar is either NULL or a member
	//of this word; general class assumptions (SearchNode, ValidationWord)
	inline bool find_Term(ValidationWord &termToFind,
		UniformNode<char> *searchStartChar = NULL, bool returnFoundWordStart = true)
	{

		return termToFind.testWord_ContainsTerm(*this,
			searchStartChar, returnFoundWordStart);

	}

	//searches for a subword within this word matching termToFind
	//returns: both found word start and end node values via reference parameters
	//assumes: this word is not empty and searchStartChar is either NULL or a member
	//of this word; general class assumptions (SearchNode, ValidationWord)
	void find_Term(ValidationWord &termToFind,
		UniformNode<char> *&returnNodeStart, UniformNode<char> *&returnNodeEnd,
		UniformNode<char> *searchStartChar = NULL)
	{

		termToFind.testWord_ContainsTerm(*this, returnNodeStart,
			returnNodeEnd, searchStartChar);

	}

	/* VALIDATION FUNCTIONS ***********************************************************/

	//tests whether this Word has valid syntax as an unsigned int; returns true if so,
	//false otherwise; assumes this word is not empty
	bool validate_UnsignedInt()
	{

		UniformNode<char> *charNode(get_FirstNode());

		do
		{

			if (charNode->content < '0' || charNode->content > '9')
				return false;

			charNode = charNode->get_NextNode();

		} while (charNode);

		return true;

	}

	//tests whether this Word has valid syntax as an int; returns true if so,
	//false otherwise; assumes this word is not empty
	bool validate_Int()
	{

		UniformNode<char> *charNode(get_FirstNode());

		//test for negative
		if (charNode->content == '-')
		{

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

		}
		
		//integer portion
		do
		{

			if (charNode->content < '0' || charNode->content > '9')
				return false;

			charNode = charNode->get_NextNode();

		} while (charNode);

		return true;

	}

	//tests whether this Word has valid syntax as a floating point; returns true if so,
	//false otherwise; assumes this word is not empty
	bool validate_FloatingPoint()
	{

		UniformNode<char> *charNode(get_FirstNode());

		//test for negative
		if (charNode->content == '-')
		{

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

		}

		//integer portion
		if (charNode->content >= '0' || charNode->content <= '9')
		{

			charNode = charNode->get_NextNode();

			while (charNode)
			{
					

				if (charNode->content >= '0' || charNode->content <= '9')
					charNode = charNode->get_NextNode();

				//test for decimal point (after integer portion)
				else if (charNode->content == '.')
				{

					charNode = charNode->get_NextNode();

					//decimal portion
					while (charNode)
					{

						if (charNode->content < '0' || charNode->content > '9')
							return false;

						charNode = charNode->get_NextNode();

					}

					return true;

				}

				else
					return false;

			}

			return true;

		}

		//test for decimal point (no preceeding integer portion)
		else if (charNode->content == '.')
		{

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

			//decimal portion
			do
			{

				if (charNode->content < '0' || charNode->content > '9')
					return false;

				charNode = charNode->get_NextNode();

			} while (charNode);

			return true;

		}

		else
			return false;

	}

	//tests whether this Word has valid syntax as an alphanumeric variable; allows only
	//alphabetical characters in first position then allows optional alphanumeric or '_'
	//characters in any subsequent positions; returns true if the Word follows this
	//syntax, false otherwise; assumes this word is not empty
	bool validate_AlphanumericVariable()
	{

		UniformNode<char> *charNode(get_FirstNode());

		if ((charNode->content >= 'a' && charNode->content <= 'z')
			|| (charNode->content >= 'A' && charNode->content <= 'Z'))
		{

			charNode = charNode->get_NextNode();

			while (charNode)
			{

				if ((charNode->content >= 'a' && charNode->content <= 'z')
					|| (charNode->content >= 'A' && charNode->content <= 'Z')
					|| (charNode->content >= '0' && charNode->content <= '9')
					|| charNode->content == '_')
					charNode = charNode->get_NextNode();

				else
					return false;

			}

			return true;

		}

		else
			return false;

	}

	//tests whether this Word has valid syntax as an indexed alphanumeric variable;
	//allows only alphabetical characters in first position then allows optional alphanumeric
	//or '_' characters in subsequent positions, then requires a '[', then characters composing
	//an unsigned integer, then a terminal ']' character; returns true if the Word follows this
	//syntax, false otherwise; assumes this word is not empty
	bool validate_IndexedAlphanumericVariable()
	{

		UniformNode<char> *charNode(get_FirstNode());

		//first character
		if ((charNode->content >= 'a' && charNode->content <= 'z')
			|| (charNode->content >= 'A' && charNode->content <= 'Z'))
		{

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

			//any remaining alphanumeric portion
			while ((charNode->content >= 'a' && charNode->content <= 'z')
				|| (charNode->content >= 'A' && charNode->content <= 'Z')
				|| (charNode->content >= '0' && charNode->content <= '9')
				|| charNode->content == '_')
			{

				charNode = charNode->get_NextNode();

				if (!charNode)
					return false;

			}

			//left bracket
			if (charNode->content != '[')
				return false;

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

			//integer portion
			if (charNode->content < '0' || charNode->content > '9')
				return false;

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

			while (charNode->content >= '0' && charNode->content <= '9')
			{

				charNode = charNode->get_NextNode();

				if (!charNode)
					return false;

			}

			//terminal right bracket
			if (charNode->content != ']')
				return false;

			if (charNode->get_NextNode())
				return false;

			return true;

		}

		else
			return false;

	}

	//tests whether this word represents "yes" via either a single 'y' or 'Y' character,
	//or via the full word with any combination of lowercase and uppercase letters; returns
	//true if the word matches this syntax, false otherwise; assumes this word is not empty
	bool validate_Yes()
	{

		UniformNode<char> *charNode(get_FirstNode());

		if (charNode->content != 'y' && charNode->content != 'Y')
			return false;

		charNode = charNode->get_NextNode();

		if (charNode)
		{

			if (charNode->content != 'e' && charNode->content != 'E')
				return false;

			charNode = charNode->get_NextNode();

			if (!charNode)
				return false;

			if (charNode->content != 's' && charNode->content != 'S')
				return false;

			if (charNode->get_NextNode())
				return false;

		}

		return true;

	}

	//tests whether this word represents "no" via either a single 'n' or 'N' character,
	//or via the full word with any combination of lowercase and uppercase letters; returns
	//true if the word matches this syntax, false otherwise; assumes this word is not empty
	bool validate_No()
	{

		UniformNode<char> *charNode(get_FirstNode());

		if (charNode->content != 'n' && charNode->content != 'N')
			return false;

		charNode = charNode->get_NextNode();

		if (charNode)
		{

			if (charNode->content != 'o' && charNode->content != 'O')
				return false;

			if (charNode->get_NextNode())
				return false;

		}

		return true;

	}

	//tests whether this word contains only whitespace characters (\n\t\r\f\v); returns
	//true if the word matches this syntax, false otherwise; assumes this word is not empty
	bool validate_Whitespace()
	{

		UniformNode<char> *charNode(get_FirstNode());

		do
		{

			if (charNode->content == '\n' || charNode->content == '\t'
				|| charNode->content == '\r' || charNode->content == '\f'
				|| charNode->content == '\v')
				charNode = charNode->get_NextNode();

			else
				return false;

		} while (charNode);

		return true;

	}

	//tests whether the whole of this word matches termToMatch
	//returns: true if termToFind matches this whole word, false otherwise
	//assumes: this word is not or empty; general class assumptions (SearchNode,
	//ValidationWord)
	inline bool validate_MatchesTerm(ValidationWord &termToMatch)
	{

		return termToMatch.testWord_MatchesTerm(*this);

	}

	/* CONSOLE VALIDATION FUNCTIONS ***************************************************/

	//overwrites this Word with console input then (a) sets the value of returnBool
	//(true for yes, false for no) and (b) returns true if console input is a valid
	//yes/no response; otherwise returns false
	bool validateFromConsole_YesNo(bool &returnBool)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_Yes())
		{

			returnBool = true;
			return true;

		}

		else if (validate_No())
		{

			returnBool = false;
			return true;

		}

		//the captured word does not match yes or no
		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of returnBool
	//(true for trueWord, false for falseWord) and (b) returns true if console input
	//is matches either trueWord or falseWord; otherwise returns false
	bool validateFromConsole_Bool(bool &returnBool, Word &trueWord, Word &falseWord)
	{

		getInput_Console(true);

		if (*this == trueWord)
		{

			returnBool = true;
			return true;

		}

		else if (*this == falseWord)
		{

			returnBool = false;
			return true;

		}

		//the captured word does not match either falseWord or trueWord
		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedChar and (b) returns true if console input is a valid
	//unsigned char within the range encompassed by and including minLimit
	//and maxLimit (if maxLimitActive == true); otherwise returns false
	bool validateFromConsole_UnsignedInt_Limited(unsigned char &returnUnsignedChar,
		const unsigned char minLimit = 0, const bool maxLimitActive = false,
		const unsigned char maxLimit = 0)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			unsigned char returnValue = interpret_UnsignedChar();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			//the word is validated
			returnUnsignedChar = returnValue;
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedChar and (b) returns true if console input is a valid
	//unsigned char; otherwise returns false
	bool validateFromConsole_UnsignedInt_Unlimited(unsigned char &returnUnsignedChar)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			//the word is validated
			returnUnsignedChar = interpret_UnsignedChar();
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedShort and (b) returns true if console input is a valid
	//unsigned short within the range encompassed by and including minLimit
	//and maxLimit (if maxLimitActive == true); otherwise returns false
	bool validateFromConsole_UnsignedInt_Limited(unsigned short &returnUnsignedShort,
		const unsigned short minLimit = 0, const bool maxLimitActive = false,
		const unsigned short maxLimit = 0)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			unsigned short returnValue = interpret_UnsignedShort();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			//the word is validated
			returnUnsignedShort = returnValue;
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedShort and (b) returns true if console input is a valid
	//unsigned short; otherwise returns false
	bool validateFromConsole_UnsignedInt_Unlimited(unsigned short &returnUnsignedShort)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			//the word is validated
			returnUnsignedShort = interpret_UnsignedShort();
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedInt and (b) returns true if console input is a valid
	//unsigned int within the range encompassed by and including minLimit
	//and maxLimit (if maxLimitActive == true); otherwise returns false
	bool validateFromConsole_UnsignedInt_Limited(unsigned int &returnUnsignedInt,
		const unsigned int minLimit = 0, const bool maxLimitActive = false,
		const unsigned int maxLimit = 0)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			unsigned int returnValue = interpret_UnsignedInt();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			//the word is validated
			returnUnsignedInt = returnValue;
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedInt and (b) returns true if console input is a valid
	//unsigned int; otherwise returns false
	bool validateFromConsole_UnsignedInt_Unlimited(unsigned int &returnUnsignedInt)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			//the word is validated
			returnUnsignedInt = interpret_UnsignedInt();
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedLongLong and (b) returns true if console input is a valid
	//unsigned long long within the range encompassed by and including minLimit
	//and maxLimit (if maxLimitActive == true); otherwise returns false
	bool validateFromConsole_UnsignedInt_Limited(unsigned long long &returnUnsignedLongLong,
		const unsigned long long minLimit = 0, const bool maxLimitActive = false,
		const unsigned long long maxLimit = 0)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			unsigned long long returnValue = interpret_UnsignedLongLong();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			//the word is validated
			returnUnsignedLongLong = returnValue;
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnUnsignedLongLong and (b) returns true if console input is a valid
	//unsigned long long; otherwise returns false
	bool validateFromConsole_UnsignedInt_Unlimited(unsigned long long &returnUnsignedLongLong)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_UnsignedInt())
		{

			//the word is validated
			returnUnsignedLongLong = interpret_UnsignedLongLong();
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnInt and (b) returns true if console input is a valid
	//int within the range encompassed by and including minLimit (if
	//minLimitActive == true) and maxLimit (if maxLimitActive == true);
	//otherwise returns false
	bool validateFromConsole_Int_Limited(int &returnInt,
		const bool minLimitActive = false, const int minLimit = 0,
		const bool maxLimitActive = false, const int maxLimit = 0)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_Int())
		{

			unsigned int returnValue = interpret_Int();

			if (minLimitActive)
			{

				if (returnValue < minLimit)
					return false;

			}

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			//the word is validated
			returnInt = returnValue;
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnInt and (b) returns true if console input is a valid int;
	//otherwise returns false
	bool validateFromConsole_Int_Unlimited(int &returnInt)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_Int())
		{

			//the word is validated
			returnInt = interpret_Int();
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnDouble and (b) returns true if console input is a valid
	//double within the range encompassed by and including minLimit
	//(if minLimitActive == true) and maxLimit (if maxLimitActive == true);
	//otherwise returns false
	bool validateFromConsole_FloatingPoint_Limited(double &returnDouble,
		const bool minLimitActive = false, double minLimit = 0.0,
		const bool maxLimitActive = false, double maxLimit = 0.0)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_FloatingPoint())
		{

			unsigned long long returnValue = interpret_Double();

			if (minLimitActive)
			{

				if (returnValue < minLimit)
					return false;

			}

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			//the word is validated
			returnDouble = returnValue;
			return true;

		}

		else
			return false;

	}

	//overwrites this Word with console input then (a) sets the value of
	//returnDouble and (b) returns true if console input is a valid double;
	//otherwise returns false
	bool validateFromConsole_FloatingPoint_Unlimited(double &returnDouble)
	{

		getInput_Console(true);

		if (!test_ListPopulated())
			return false;

		if (validate_FloatingPoint())
		{

			//the word is validated
			returnDouble = interpret_Double();
			return true;

		}

		else
			return false;

	}

	//overwrites any current contents of targetTerm with
	//the new term provided by the user
	void validateFromConsole_ValidationWord(ValidationWord &targetTerm)
	{

		bool complexConstruction;
		bool includeReturn;

		//prompt user input
		cout << "*** ValidationWord Creation ***";

		//simple construction decision
		do
		{

			cout << "\nconstruct a complex ValidationWord manually by individual ValidationLetters (Y/N)? ";

			if (validateFromConsole_YesNo(complexConstruction))
				break;

			else
				cout << "\ninvalid entry, try again";

		} while (true);

		//fast simple ValidationWord
		if (!complexConstruction)
		{

			//includeReturn decision
			do
			{

				cout << "\nshould the terminal return be included in this ValidationLetter (Y/N): ";

				if (validateFromConsole_YesNo(includeReturn))
					break;

				else
					cout << "\ninvalid entry, try again";

			} while (true);

			cout << "\ntype the Word to be used for populating this simple ValidationWord: ";

			getInput_Console(true, !includeReturn);
			targetTerm = ValidationWord(*this);

		}

		//slow manual ValidationWord
		else
		{

			UniformList<ValidationLetter_ConstructCode> tempConstructCodeList;
			Word anyTypeChars;
			UniformNode<char> *anyTypeCharsIterationNode;
			unsigned short searchNodeType;
			bool invertDecision;
			unsigned short minLength;
			unsigned short maxLength;
			bool continueChoice;

			do
			{

				//SearchNode type
				do
				{

					cout << "\n\t0) disparate character list";
					cout << "\n\t1) one contiguous character range";
					cout << "\n\t2) one contiguous character range and a disparate character list";
					cout << "\n\t3) two seperate contiguous character ranges";
					cout << "\n\t4) two seperate contiguous character ranges and a disparate character list";
					cout << "\n\t5) three seperate contiguous character ranges";
					cout << "\nselect a type for the current SearchNode: ";

					if (validateFromConsole_UnsignedInt_Limited(searchNodeType, 0, true, 5))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				do
				{

					//includeReturn decision
					do
					{

						cout << "\nshould the terminal return be included in this ValidationLetter (Y/N): ";

						if (validateFromConsole_YesNo(includeReturn))
							break;

						else
							cout << "\ninvalid entry, try again";

					} while (true);

					//populate anyTypeChars and validate it's length
					if (searchNodeType == 0)
					{

						cout << "\nenter consecutive char(s) to populate this ValidationLetter: ";

						anyTypeChars.getInput_Console(true, !includeReturn);

						if (anyTypeChars.get_ListLength() >= 1)
							break;

					}

					else if (searchNodeType == 1)
					{

						cout << "\nenter the range start and range end chars ";
						cout << "consecutively to populate this ValidationLetter: ";

						anyTypeChars.getInput_Console(true, !includeReturn);

						if (anyTypeChars.get_ListLength() >= 2)
							break;

					}

					else if (searchNodeType == 2)
					{

						cout << "\nenter the range start and range end chars ";
						cout << "then any additional disparate chars consecutively ";
						cout << "to populate this ValidationLetter: ";

						anyTypeChars.getInput_Console(true, !includeReturn);

						if (anyTypeChars.get_ListLength() >= 3)
							break;

					}

					else if (searchNodeType == 3)
					{

						cout << "\nenter the first range start and end chars, ";
						cout << "then the second range start and end chars all ";
						cout << "consecutively to populate this ValidationLetter: ";

						anyTypeChars.getInput_Console(true, !includeReturn);

						if (anyTypeChars.get_ListLength() >= 4)
							break;

					}

					else if (searchNodeType == 4)
					{

						cout << "\nenter the first range start and end chars, ";
						cout << "the second range start and end chars, then ";
						cout << "any additional disparate chars consecutively ";
						cout << "to populate this ValidationLetter: ";

						anyTypeChars.getInput_Console(true, !includeReturn);

						if (anyTypeChars.get_ListLength() >= 5)
							break;

					}

					else if (searchNodeType == 5)
					{

						cout << "\nenter the first range start and end chars, ";
						cout << "then the second range start and end chars, then\n";
						cout << "the third range start and end chars, all ";
						cout << "consecutively to populate this ValidationLetter: ";

						anyTypeChars.getInput_Console(true, !includeReturn);

						if (anyTypeChars.get_ListLength() >= 6)
							break;

					}

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//inverted decision
				do
				{

					cout << "\nshould the current ValidationLetter be inverted, ";
					cout << "i.e. test positive for any other char than those listed (Y/N)? ";

					if (validateFromConsole_YesNo(invertDecision))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//min length
				do
				{

					cout << "\nwhat is the minimum character length for this ValidationLetter (0 means it is optional)? ";

					if (validateFromConsole_UnsignedInt_Unlimited(minLength))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//max length
				do
				{

					cout << "\nwhat is the maximum character length for this ValidationLetter (0 means it can be infinite)? ";

					if (validateFromConsole_UnsignedInt_Unlimited(maxLength))
					{

						//testing this manually instead of via parameter because 
						//a zero value of max length is valid regardless of minlength
						if (!maxLength || maxLength >= minLength)
							break;

					}

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//test for exit
				do
				{

					cout << "\nadd another SearchNode to this ValidationWord (Y/N)?";

					if (validateFromConsole_YesNo(continueChoice))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//add the SearchNode to the temporary list
				tempConstructCodeList.add_NewNode(ValidationLetter_ConstructCode(anyTypeChars, searchNodeType,
					invertDecision, minLength, maxLength));

			} while (continueChoice);

			//create the ValidationWord
			targetTerm = ValidationWord(tempConstructCodeList);

		}

	}

};

class WordGroup : public UniformList<Word>
{

public:

	//default constructor
	WordGroup() : UniformList<Word>() {}

	//default destructor
	~WordGroup() {}

	/* INPUT/OUTPUT FUNCTIONS *********************************************************/

	//generates a single conglomorate word based from console input
	//should be used in conjunction w/ rebase() if seperationg 
	//wordgroup into multiple words is desired
	void getInput_Console(bool overwrite = false, bool excludeTerminator = true)
	{

		UniformNode<Word> *wordNode;

		//overwrite new input into the first Word in this WordGroup if
		//the WordGroup is not empty
		if (overwrite)
		{

			wordNode = get_LastNode();

			if (wordNode)
			{

				unsigned long long surplusGroupLength = get_ListLength() - 1;

				if (surplusGroupLength)
					wordNode = delete_Range(wordNode, surplusGroupLength, false, false);

				wordNode->content.getInput_Console(true, excludeTerminator);
				return;

			}

		}

		//overwrite == false and/or the WordGroup is empty
		wordNode = add_NewNode();
		wordNode->content.getInput_Console(false, excludeTerminator);

	}

	void getInput_File(Word &fileName, bool overwrite = false)
	{

		ifstream inputFile;

		//open file corresponding to fileName or return if opening fails
		if (!open_InputFile(inputFile, fileName))
			return;

		//additional variables
		UniformNode<Word> *wordNode;
		UniformNode<char> *charNode;
		char nextChar = inputFile.get();

		//overwrite new input into the first Word in this WordGroup if
		//the WordGroup is not empty
		if (overwrite)
		{

			wordNode = get_LastNode();

			if (wordNode)
			{

				unsigned long long surplusGroupLength = get_ListLength() - 1;

				if (surplusGroupLength)
					wordNode = delete_Range(wordNode,
						surplusGroupLength, false, false);

				charNode = wordNode->content.get_FirstNode();

				//copy nextChar contents into charNode till the list runs
				//out of nodes or till the end of file
				while (charNode)
				{

					if (inputFile.eof())
					{

						wordNode->content.delete_Range(charNode,
							wordNode->content.get_LastNode());

						inputFile.close();
						return;

					}

					else
					{

						charNode->content = nextChar;
						charNode = charNode->get_NextNode();

						nextChar = inputFile.get();

					}

				}

				//list is out of nodes, now add new nodes till
				//the end of file
				while (!inputFile.eof())
				{

					wordNode->content.add_NewNode(nextChar);
					nextChar = inputFile.get();

				}

				inputFile.close();
				return;

			}

		}

		//overwrite == false and/or the WordGroup is empty
		//add new nodes to a new Word till the end of file
		wordNode = add_NewNode();

		while (!inputFile.eof())
		{

			wordNode->content.add_NewNode(nextChar);
			nextChar = inputFile.get();

		}

		inputFile.close();

	}

	//outputs all the contents of this WordGroup to the console
	void output_Console()
	{

		if (!test_ListPopulated())
			return;

		UniformNode<char> *charNode;
		UniformNode<Word> *wordNode = get_FirstNode();

		do
		{

			charNode = wordNode->content.get_FirstNode();

			while (charNode)
			{

				cout << charNode->content;
				charNode = charNode->get_NextNode();

			}

			wordNode = wordNode->get_NextNode();

		} while (wordNode);

	}

	//outputs all the contents of this WordGroup to the specified file
	//has no effect if the WordGroup is empty
	void output_File(Word &fileName, bool overwrite = false)
	{

		if (!test_ListPopulated())
			return;

		ofstream outputFile;

		//open file corresponding to fileName or return if opening fails
		if (!open_OutputFile(outputFile, fileName, overwrite))
			return;

		UniformNode<char> *charNode;
		UniformNode<Word> *wordNode = get_FirstNode();

		do
		{

			charNode = wordNode->content.get_FirstNode();

			while (charNode)
			{

				outputFile << charNode->content;
				charNode = charNode->get_NextNode();

			}

			wordNode = wordNode->get_NextNode();

		} while (wordNode);

		outputFile.close();

	}

	/* EDITING FUNCTIONS **************************************************************/

	//assumes: rangeFirstWord and rangeLastWord are not NULL, are members of
	//this WordGroup, and encompass a valid range in their respective positions
	//effects: condenses all the range words' contents into rangeLastWord, does
	//nothing if rangeFirstWord == rangeLastWord
	void editRange_Merge(UniformNode<Word> *rangeFirstWord,
		UniformNode<Word> *rangeLastWord)
	{

		if (rangeFirstWord != rangeLastWord)
		{

			UniformNode<Word> *laggingIterationWord = rangeFirstWord;
			UniformNode<Word> *leadingIterationWord;

			do
			{

				leadingIterationWord = laggingIterationWord->get_NextNode();

				if (laggingIterationWord->content.test_ListPopulated())
					leadingIterationWord->content.add_CutWholeList(laggingIterationWord->content,
						NULL, false);

				laggingIterationWord = leadingIterationWord;

			} while (laggingIterationWord != rangeLastWord);

			//delete the empty word shells
			delete_Range(rangeFirstWord, rangeLastWord->get_PriorNode());

		}

	}

	//assumes: wordToSplit is not NULL (or empty) and is a member of this WordGroup;
	//splitPoint is not NULL and is a member of wordToSplit 
	//maximal efficiency with forward iteration (split point assumed closer to beginning
	//of wordToSplit than end)
	//returns nothing but effect is to insert a new node before wordToSplit which
	//then contains all the noted before splitPoint (and including splitPoint if
	//splitAfter is true) so that wordToSplit retains only the remaining later nodes
	void editWord_Split(UniformNode<Word> *wordToSplit,
		UniformNode<char> *splitPoint, const bool splitAfter = true)
	{

		if (!splitAfter)
		{

			splitPoint = splitPoint->get_PriorNode();

			//splitAfter was false and split point was the first node of wordToSplit
			if (!splitPoint)
				return;

		}

		//insert a new empty word right before subjectWord
		UniformNode<Word> *newWordNode = add_NewNode(wordToSplit, false);

		//cut and paste nodes into the new word
		newWordNode->content.add_CutRange(wordToSplit->content,
			wordToSplit->content.get_FirstNode(), splitPoint);

	}

	//*** DOCUMENT ***
	//this should do nothing if the 
	void editRange_Intersperse(Word &spacerWord,
		UniformNode<Word> *startWord, UniformNode<Word> *endWord,
		const bool addAtStart = false, const bool addAtEnd = false)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//add first spacer before startWord if indicated
		if(addAtStart)
			add_NewNode(spacerWord, startWord, false);

		//add spacers in the body of the range
		while (startWord != endWord)
		{

			startWord = startWord->get_NextNode();
			add_NewNode(spacerWord, startWord, false);

		}

		//add last spacer after endWord if indicated
		if (addAtEnd)
			add_NewNode(spacerWord, startWord);

	}

	//purpose: invert subranges of words within a larger range of words in
	//this WordGroup seperated by a repeating subword matching sentinelTerm
	//assumes:	general class assumptions (SearchNode, ValidationWord)
	void editRange_InvertSentinelBoundedSubRanges(ValidationWord &sentinelTerm,
		UniformNode<Word> *startWord, UniformNode<Word> *endWord,
		const bool sentinelLeads = true, const bool includeSentinel = false)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//initial setup (NULL values are meaningless)
		UniformNode<Word> *trailingWordNode = NULL;
		UniformNode<Word> *leadingWordNode = NULL;

		if (sentinelLeads)
		{

			//ensure the original endWord is included in operations
			endWord = endWord->get_NextNode();

			//find the first sentinel word
			do
			{

				if (startWord->content.test_ListPopulated()
					&& sentinelTerm.testWord_MatchesTerm(startWord->content))
				{

					trailingWordNode = startWord;
					leadingWordNode = startWord->get_NextNode();
					break;

				}

				else
					startWord = startWord->get_NextNode();

			} while (startWord != endWord);

			if (startWord == endWord)
				return;

			//loop to find and invert 
			while (leadingWordNode != endWord)
			{

				if (leadingWordNode->content.test_ListPopulated()
					&& sentinelTerm.testWord_MatchesTerm(leadingWordNode->content))
				{

					if (includeSentinel)
						startWord = invert_Range(startWord, trailingWordNode);

					else
					{

						if (trailingWordNode != startWord)
						{

							startWord = startWord->get_NextNode();
							startWord = invert_Range(startWord, trailingWordNode);

						}

						else
							startWord = startWord->get_NextNode();

					}

					if (startWord != endWord)
					{

						trailingWordNode = startWord;
						leadingWordNode = startWord->get_NextNode();

					}

					else
						return;

				}

				else
				{

					trailingWordNode = leadingWordNode;
					leadingWordNode = leadingWordNode->get_NextNode();

				}

			}

			//final iteration
			if (includeSentinel)
				invert_Range(startWord, trailingWordNode);

			else if (trailingWordNode != startWord)
			{


				startWord = startWord->get_NextNode();
				invert_Range(startWord, trailingWordNode);

			}

		}

		else
		{

			//ensure the original startWord is included in operations
			startWord = startWord->get_PriorNode();

			//find the last sentinel word (iterating backward)
			do
			{

				if (endWord->content.test_ListPopulated()
					&& sentinelTerm.testWord_MatchesTerm(endWord->content))
				{

					trailingWordNode = endWord;
					leadingWordNode = endWord->get_PriorNode();
					break;

				}

				else
					endWord = endWord->get_PriorNode();

			} while (endWord != startWord);

			if (endWord == startWord)
				return;

			//loop to find and invert 
			while (leadingWordNode != startWord)
			{

				if (leadingWordNode->content.test_ListPopulated()
					&& sentinelTerm.testWord_MatchesTerm(leadingWordNode->content))
				{

					if (includeSentinel)
						endWord = invert_Range(trailingWordNode, endWord, false);

					else
					{

						if (trailingWordNode != endWord)
						{

							endWord = endWord->get_PriorNode();
							endWord = invert_Range(trailingWordNode, endWord, false);

						}

						else
							endWord = endWord->get_PriorNode();

					}

					if (endWord != startWord)
					{

						trailingWordNode = endWord;
						leadingWordNode = endWord->get_NextNode();

					}

					else
						return;

				}

				else
				{

					trailingWordNode = leadingWordNode;
					leadingWordNode = leadingWordNode->get_NextNode();

				}

			}

			//final iteration
			if (includeSentinel)
				invert_Range(trailingWordNode, endWord);

			else if (trailingWordNode != endWord)
			{


				endWord = endWord->get_PriorNode();
				invert_Range(trailingWordNode, endWord);

			}

		}

	}

	//purpose: find a delete all subwords within a range of words
	//in this WordGroup with optional associated word splitting
	//assumes:	general class assumptions (SearchNode, ValidationWord)
	void editRange_DeleteSubwords(ValidationWord &termToDelete,
		UniformNode<Word> *startWord, UniformNode<Word> *endWord,
		const bool thenSplit)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//tracking variables
		UniformNode<char> *charNode1 = NULL;
		UniformNode<char> *charNode2;

		//so that the original endWord is included in operations
		endWord = endWord->get_NextNode();

		if (thenSplit)
		{

			//loop to find, delete, and split all words in the range
			do
			{

				if (startWord->content.test_ListPopulated())
				{

					//assign the first and last found word node if a subword is found
					termToDelete.testWord_ContainsTerm(startWord->content,
						charNode1, charNode2, charNode1);

					//delete the subword and split the word after if charNode1 and 2 are assigned
					if (charNode1)
					{

						//set charNode1 to the next node after the deleted found word
						charNode1 = startWord->content.delete_Range(charNode1, charNode2);

						//there are nodes remaining in the current word
						if (charNode1)
						{

							//charNode1 is not in first position of startWord following the
							//preceeding deletion (otherwise, no split occurs and the control
							//continues searching the remainder of the current startWord for
							//matching subwords)
							if (charNode1 != startWord->content.get_FirstNode())
								editWord_Split(startWord, charNode1, false);

						}

						//there are no nodes remaining in the current word
						else
							startWord = startWord->get_NextNode();

					}

					else
						startWord = startWord->get_NextNode();

				}

				else //startWord is empty
					startWord = startWord->get_NextNode();

			} while (startWord != endWord);

		}

		else //thenSplit == false
		{

			//loop to find and delete the found searchTerm chars as many times as necessary
			//for all words in the identified range
			do
			{

				if (startWord->content.test_ListPopulated())
				{

					//assign the first and last found word node if a subword is found
					termToDelete.testWord_ContainsTerm(startWord->content,
						charNode1, charNode2, charNode1);

					//delete the subword if charNode1 and 2 are assigned
					if (charNode1)
					{

						//set charNode1 to the next node after the deleted found word
						charNode1 = startWord->content.delete_Range(charNode1, charNode2);

						//there are no nodes remaining in the current word
						if (!charNode1)
							startWord = startWord->get_NextNode();

					}

					else
						startWord = startWord->get_NextNode();

				}

				else //startWord is empty
					startWord = startWord->get_NextNode();

			} while (startWord != endWord);

		}

	}

	//purpose: find a delete all whitespace subwords within a range of words
	//in this WordGroup with optional associated word splitting
	//assumes:	general class assumptions (SearchNode, ValidationWord)
	void editRange_DeleteWhitespace(UniformNode<Word> *startWord,
		UniformNode<Word> *endWord, const bool thenSplit)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//initial setup
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode(" \n\t\r\f\v", 0, false, 1, 0));

		ValidationWord Whitespace(tempConstructCodes);
		
		UniformNode<char> *charNode1 = NULL;
		UniformNode<char> *charNode2;

		//so that the original endWord is included in operations
		endWord = endWord->get_NextNode();

		if (thenSplit)
		{

			//loop to find, delete, and split all words in the range
			do
			{

				if (startWord->content.test_ListPopulated())
				{

					//assign the first and last found word node if a subword is found
					Whitespace.testWord_ContainsTerm(startWord->content,
						charNode1, charNode2, charNode1);

					//delete the subword and split the word after if charNode1 and 2 are assigned
					if (charNode1)
					{

						//set charNode1 to the next node after the deleted found word
						charNode1 = startWord->content.delete_Range(charNode1, charNode2);

						//there are nodes remaining in the current word
						if (charNode1)
						{

							//charNode1 is not in first position of startWord following the
							//preceeding deletion (otherwise, no split occurs and the control
							//continues searching the remainder of the current startWord for
							//matching subwords)
							if (charNode1 != startWord->content.get_FirstNode())
								editWord_Split(startWord, charNode1, false);

						}

						//there are no nodes remaining in the current word
						else
							startWord = startWord->get_NextNode();

					}

					else
						startWord = startWord->get_NextNode();

				}

				else //startWord is empty
					startWord = startWord->get_NextNode();

			} while (startWord != endWord);

		}

		else //thenSplit == false
		{

			//loop to find and delete the found searchTerm chars as many times as necessary
			//for all words in the identified range
			do
			{

				if (startWord->content.test_ListPopulated())
				{

					//assign the first and last found word node if a subword is found
					Whitespace.testWord_ContainsTerm(startWord->content,
						charNode1, charNode2, charNode1);

					//delete the subword if charNode1 and 2 are assigned
					if (charNode1)
					{

						//set charNode1 to the next node after the deleted found word
						charNode1 = startWord->content.delete_Range(charNode1, charNode2);

						//there are no nodes remaining in the current word
						if (!charNode1)
							startWord = startWord->get_NextNode();

					}

					else
						startWord = startWord->get_NextNode();

				}

				else //startWord is empty
					startWord = startWord->get_NextNode();

			} while (startWord != endWord);

		}

	}

	//document
	void editRange_InsertSubwords(ValidationWord &insertionPointTerm,
		Word &insertWord, UniformNode<Word> *startWord,
		UniformNode<Word> *endWord, const bool asNewWord = false,
		const bool insertAfter = true)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//tracking variables
		UniformNode<char> *charNode1 = NULL;

		//so that the original endWord is included in operations
		endWord = endWord->get_NextNode();

		if (!asNewWord)
		{

			if (insertAfter)
			{

				//loop to find and insert at all found subwords in the range
				do
				{

					if (startWord->content.test_ListPopulated())
					{

						//assign the last found word node if a subword is found
						charNode1 = insertionPointTerm.testWord_ContainsTerm(
							startWord->content, charNode1, false);

						//insert insertWord after end of any found subword
						if (charNode1)
						{

							//set charNode1 to the next node after the inserted word
							charNode1 = startWord->content.add_CopiedWholeList(
								insertWord, charNode1)->get_NextNode();

							//there are no nodes remaining in the current word
							if (!charNode1)
								startWord = startWord->get_NextNode();

						}

						else
							startWord = startWord->get_NextNode();

					}

					else //startWord is empty
						startWord = startWord->get_NextNode();

				} while (startWord != endWord);

			}

			else
			{

				UniformNode<char> *charNode2;

				//loop to find and insert at all found subwords in the range
				do
				{

					if (startWord->content.test_ListPopulated())
					{

						//assign the first and last found word node if a subword is found
						insertionPointTerm.testWord_ContainsTerm(startWord->content,
							charNode2, charNode1, charNode1);

						//insert insertWord before beginning of any found subword
						if (charNode1)
						{

							//insert the word and set charNode1 to the next node 
							//after the found subword
							startWord->content.add_CopiedWholeList(insertWord, charNode2, false);
							charNode1 = charNode1->get_NextNode();

							//there are no nodes remaining in the current word
							if (!charNode1)
								startWord = startWord->get_NextNode();

						}

						else
							startWord = startWord->get_NextNode();

					}

					else //startWord is empty
						startWord = startWord->get_NextNode();

				} while (startWord != endWord);

			}

		}

		else
		{

			if (insertAfter)
			{

				//loop to find and insert at all found subwords in the range
				do
				{

					if (startWord->content.test_ListPopulated())
					{

						//assign the last found word node if a subword is found
						charNode1 = insertionPointTerm.testWord_ContainsTerm(
							startWord->content, charNode1, false);

						//insert insertWord after end of any found subword
						if (charNode1)
						{

							//set charNode1 to the next node after found subword
							charNode1 = charNode1->get_NextNode();

							if (charNode1)
							{

								editWord_Split(startWord, charNode1, false);
								add_NewNode(insertWord, startWord, false);

							}

							else
							{

								startWord = startWord->get_NextNode();

								if (startWord)
									add_NewNode(insertWord, startWord, false);

								else
								{

									add_NewNode(insertWord);
									return;

								}

							}

						}

						else
							startWord = startWord->get_NextNode();

					}

					else //startWord is empty
						startWord = startWord->get_NextNode();

				} while (startWord != endWord);

			}

			else
			{

				UniformNode<char> *charNode2;

				//loop to find and insert at all found subwords in the range
				do
				{

					if (startWord->content.test_ListPopulated())
					{

						//assign the first and last found word node if a subword is found
						insertionPointTerm.testWord_ContainsTerm(startWord->content,
							charNode2, charNode1, charNode1);

						//insert insertWord after end of any found subword
						if (charNode1)
						{

							if (charNode2 != startWord->content.get_FirstNode())
							{

								editWord_Split(startWord, charNode2, false);
								add_NewNode(insertWord, startWord, false);

							}

							else
								add_NewNode(insertWord, startWord, false);

							//set charNode1 to the next node after found subword
							charNode1 = charNode1->get_NextNode();

							if (!charNode1)
								startWord = startWord->get_NextNode();

						}

						else
							startWord = startWord->get_NextNode();

					}

					else //startWord is empty
						startWord = startWord->get_NextNode();

				} while (startWord != endWord);

			}

		}

	}

	//document
	void editRange_ReplaceSubwords(ValidationWord &termToReplace,
		const Word &replacementWord, UniformNode<Word> *startWord,
		UniformNode<Word> *endWord, const bool splitBefore, const bool splitAfter)
	{

		//ensure the replacement word isnt empty
		if (!replacementWord.test_ListPopulated())
			return;

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//tracking variables
		UniformNode<Word> *wordNode = startWord;
		UniformNode<char> *charNode1;
		UniformNode<char> *charNode2 = NULL;
		UniformNode<char> *replacementWordFirstNode = replacementWord.get_FirstNode();
		UniformNode<char> *replacementWordLastNode = replacementWord.get_LastNode();

		//ensure the original endWord is included in operations
		endWord = endWord->get_NextNode();

		if (splitBefore)
		{

			//both split before and after
			if (splitAfter)
			{

				do
				{

					if (wordNode->content.test_ListPopulated())
					{

						//assign first and last found word node if subword is found
						termToReplace.testWord_ContainsTerm(wordNode->content, charNode1,
							charNode2, charNode2);

						//charNode1 and 2 are assigned
						if (charNode2)
						{

							//replace the found subword
							charNode2 = wordNode->content.replace_Range(
								replacementWordFirstNode, replacementWordLastNode,
								charNode1, charNode2);

							//split before found word if not at word start
							if (charNode1 != wordNode->content.get_FirstNode())
								editWord_Split(wordNode, charNode1, false);

							//split after found word if not at word end
							if (charNode2)
								editWord_Split(wordNode, charNode2, false);

							else //the found subword was at the end of the word
								wordNode = wordNode->get_NextNode();

						}

						else //sub word was not found
							wordNode = wordNode->get_NextNode();

					}

					else //wordNode is empty
						wordNode = wordNode->get_NextNode();

				} while (wordNode != endWord);

			}

			//only split before
			else
			{

				//loop to find split before subword for all words in the range
				do
				{

					if (wordNode->content.test_ListPopulated())
					{

						//assign the first and last found word node if a subword is found
						termToReplace.testWord_ContainsTerm(wordNode->content,
							charNode1, charNode2, charNode2);

						//charNode1 and 2 are assigned
						if (charNode2)
						{

							//replace the found subword
							charNode2 = wordNode->content.replace_Range(
								replacementWordFirstNode, replacementWordLastNode,
								charNode1, charNode2);

							//split before found word if not at word start
							if (charNode1 != wordNode->content.get_FirstNode())
								editWord_Split(wordNode, charNode1, false);

							//the found subword was not at the end of the word
							if (!charNode2)
								wordNode = wordNode->get_NextNode();

						}

						else //sub word was not found
							wordNode = wordNode->get_NextNode();

					}

					else //wordNode is empty
						wordNode = wordNode->get_NextNode();

				} while (wordNode != endWord);

			}

		}

		//only splitAfter
		else if (splitAfter)
		{

			//loop to find split before subword for all words in the range
			do
			{

				if (wordNode->content.test_ListPopulated())
				{

					//assign the first and last found word node if a subword is found
					termToReplace.testWord_ContainsTerm(wordNode->content,
						charNode1, charNode2);

					//charNode1 and 2 are assigned
					if (charNode2)
					{

						//replace the found subword
						charNode2 = wordNode->content.replace_Range(
							replacementWordFirstNode, replacementWordLastNode,
							charNode1, charNode2);

						//split after found word if not at word end
						if (charNode2)
							editWord_Split(wordNode, charNode2, false);

						else
							wordNode = wordNode->get_NextNode();

					}

					else //sub word was not found
						wordNode = wordNode->get_NextNode();

				}

				else //wordNode is empty
					wordNode = wordNode->get_NextNode();

			} while (wordNode != endWord);

		}

		//no splitting at all
		else
		{

			//loop to find split before subword for all words in the range
			do
			{

				if (wordNode->content.test_ListPopulated())
				{

					//assign the first and last found word node if a subword is found
					termToReplace.testWord_ContainsTerm(wordNode->content,
						charNode1, charNode2, charNode2);

					//charNode1 and 2 are assigned
					if (charNode2)
					{

						//replace the found subword
						charNode2 = wordNode->content.replace_Range(
							replacementWordFirstNode, replacementWordLastNode,
							charNode1, charNode2);

						//found sub word was at word end - iterate to next word
						if (!charNode2)
							wordNode = wordNode->get_NextNode();

					}

					else //sub word was not found
						wordNode = wordNode->get_NextNode();

				}

				else //wordNode is empty
					wordNode = wordNode->get_NextNode();

			} while (wordNode != endWord);

		}

	}

	//document
	void editRange_Split(ValidationWord &splitPointTerm,
		UniformNode<Word> *startWord, UniformNode<Word> *endWord,
		const bool splitBefore, const bool splitAfter)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//tracking variables
		UniformNode<Word> *wordNode = startWord;
		UniformNode<char> *charNode1 = NULL;

		//ensure the original endWord is included in operations
		endWord = endWord->get_NextNode();

		//main operation
		if (splitBefore)
		{

			UniformNode<char> *charNode2;

			//both splitAfter and splitBefore are true
			if (splitAfter)
			{

				//loop to find split before and after subword for all words in the range
				do
				{

					if (wordNode->content.test_ListPopulated())
					{

						//assign the first and last found word node if a subword is found
						splitPointTerm.testWord_ContainsTerm(wordNode->content,
							charNode1, charNode2, charNode1);

						//charNode1 and 2 are assigned
						if (charNode1)
						{

							//the found subword was not at the Word start
							if (charNode1 != wordNode->content.get_FirstNode())
								editWord_Split(wordNode, charNode1, false);

							charNode1 = charNode2->get_NextNode();

							//the found subword was not at the end of the word
							if (charNode1)
								editWord_Split(wordNode, charNode2);

							else //the found subword was at the end of the word
								wordNode = wordNode->get_NextNode();

						}

						else //sub word was not found
							wordNode = wordNode->get_NextNode();

					}

					else //wordNode is empty
						wordNode = wordNode->get_NextNode();

				} while (wordNode != endWord);

			}

			else //splitBefore only is true
			{

				//loop to find split before subword for all words in the range
				do
				{

					if (wordNode->content.test_ListPopulated())
					{

						//assign the first and last found word node if a subword is found
						splitPointTerm.testWord_ContainsTerm(wordNode->content,
							charNode1, charNode2, charNode1);

						//charNode1 and 2 are assigned
						if (charNode1)
						{

							//the found subword was not at the Word start
							if (charNode1 != wordNode->content.get_FirstNode())
								editWord_Split(wordNode, charNode1, false);

							charNode1 = charNode2->get_NextNode();

							//the found subword was at the end of the word
							if (!charNode1)
								wordNode = wordNode->get_NextNode();

						}

						else //sub word was not found
							wordNode = wordNode->get_NextNode();

					}

					else //wordNode is empty
						wordNode = wordNode->get_NextNode();

				} while (wordNode != endWord);

			}

		}

		//splitAfter only is true
		else if (splitAfter)
		{

			//loop to find split after subword for all words in the range
			do
			{

				if (wordNode->content.test_ListPopulated())
				{

					//assign the last found word node if a subword is found
					charNode1 = splitPointTerm.testWord_ContainsTerm(
						wordNode->content, NULL, false);

					//split the word if charNode1 is assigned and is not at Word end
					if (charNode1 && charNode1 != wordNode->content.get_LastNode())
						editWord_Split(wordNode, charNode1);

					else //sub word was not found or was found at the Word end
						wordNode = wordNode->get_NextNode();

				}

				else //wordNode is empty
					wordNode = wordNode->get_NextNode();

			} while (wordNode != endWord);

		}

	}

	//document
	void editRange_DeleteWords(ValidationWord &termToDelete,
		UniformNode<Word> *startWord, UniformNode<Word> *endWord,
		const bool thenMerge = false)
	{

		//ensure startWord and endWord are not NULL, are members of this WordGroup,
		//and encompass a valid range in their respective positions
		if (!startWord || !endWord
			|| !test_RangeValidityAndMembership(startWord, endWord))
			return;

		//ensure the original endWord is included in operations
		endWord = endWord->get_NextNode();

		if (!thenMerge)
		{

			do
			{

				//the word is not empty and whole word matches the ValidationWord
				if (startWord->content.test_ListPopulated()
					&& termToDelete.testWord_MatchesTerm(startWord->content))
					startWord = delete_Node(startWord);

				else
					startWord = startWord->get_NextNode();

			} while (startWord != endWord);

		}

		else
		{

			//tracking variables
			UniformNode<Word> *laggingNode;
			bool lastWordDeleted = false;

			do
			{

				//word is not empty and wholly matches the search term,
				//word is deleted and startWord assigned to next node
				if (startWord->content.test_ListPopulated()
					&& termToDelete.testWord_MatchesTerm(startWord->content))
				{

					startWord = delete_Node(startWord);
					lastWordDeleted = true;

				}

				//word is not empty and does not wholly match the search term
				else
				{

					if (lastWordDeleted && startWord)
					{

						lastWordDeleted = false;
						laggingNode = startWord->get_PriorNode();

						//merge the preceding node with the subsequent node
						if (laggingNode)
							editRange_Merge(laggingNode, startWord);

						startWord = startWord->get_NextNode();

					}

				}

			} while (startWord != endWord);

			//final merge if warranted
			if (lastWordDeleted && startWord)
			{

				laggingNode = startWord->get_PriorNode();

				//merge the preceding node with the subsequent node
				if (laggingNode)
					editRange_Merge(laggingNode, startWord);

			}

		}

	}

	/* NOT CURRENTLY DEVELOPED

	void editRange_InsertWords(ValidationWord &insertionPointTerm,
		const Word &insertWord, UniformNode<Word> *startWord,
		UniformNode<Word> *endWord, const bool insertAfter)
	{



	}

	void editRange_ReplaceWords(ValidationWord termToReplace,
		const Word &replacementWord, UniformNode<Word> *startWord,
		UniformNode<Word> *endWord)
	{



	}

	*/

	/* WORD NODE VALIDATION FUNCTIONS *************************************************/

	//first tests whether wordNodeToTest contains termToFind; if so and if iterate is
	//true, second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_ContainsTerm(ValidationWord &termToFind,
		UniformNode<Word> *&wordNodeToTest, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (termToFind.testWord_ContainsTerm(wordNodeToTest->content))
		{

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches termToMatch; if so and if iterate is
	//true, second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_MatchesTerm(ValidationWord &termToMatch,
		UniformNode<Word> *&wordNodeToTest, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (termToMatch.testWord_MatchesTerm(wordNodeToTest->content))
		{

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches WHITESPACE; if so and if iterate is
	//true, second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_Whitespace(UniformNode<Word> *&wordNodeToTest,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_Whitespace())
		{

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches ALPHANUMERIC_VARIABLE; if so and if iterate is
	//true, then iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise;
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_AlphaNumeric(UniformNode<Word> *&wordNodeToTest,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_AlphanumericVariable())
		{

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches ALPHANUMERIC_VARIABLE_INDEXED; if so
	//and if iterate is true, then iterates wordNodeToTest; if so and if ensureNonTerminal
	//is true, then tests to ensure wordNodeToTest != NULL; returns true if conditions
	//are met as defined by parameter values, false otherwise;
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_IndexedAlphaNumericVariable(UniformNode<Word> *&wordNodeToTest,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_IndexedAlphanumericVariable())
		{

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches matches ALPHANUMERIC_VARIABLE;
	//if so then sets returnVariable to wordNodeToTest->content; if iterate is true,
	//then iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_CaptureAlphaNumeric(UniformNode<Word> *&wordNodeToTest,
		Word &returnVariable, const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_AlphanumericVariable())
		{

			returnVariable = wordNodeToTest->content;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches ALPHANUMERIC_VARIABLE_INDEXED;
	//if so then sets returnVariable to wordNodeToTest->content; if iterate is true,
	//then iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values and both trims the "[#]" index portion off of returnVariable
	//and assigns the variable index value to returnIndex; returns false otherwise;
	//assumes wordNodeToTest != NULL and is a member of this WordGroup
	bool validateWordNode_CaptureIndexedAlphaNumericVariable(UniformNode<Word> *&wordNodeToTest,
		Word &returnVariable, unsigned long long &returnIndex, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_IndexedAlphanumericVariable())
		{

			returnVariable = wordNodeToTest->content;

			UniformNode<char> *charNode(returnVariable.delete_Node(
				returnVariable.get_LastNode(), false));
			unsigned long long tempIndex(0);

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					do
					{

						tempIndex = (tempIndex * 10) + (charNode->content - '0');

						charNode = returnVariable.delete_Node(charNode, false);

					} while (charNode->content != '[');

					returnVariable.delete_Node(charNode);

					returnIndex = tempIndex;

					return true;

				}

			}

			do
			{

				tempIndex = (tempIndex * 10) + (charNode->content - '0');

				charNode = returnVariable.delete_Node(charNode, false);

			} while (charNode->content != '[');

			returnVariable.delete_Node(charNode);

			returnIndex = tempIndex;

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches wordToMatch; if so and if iterate is
	//true, second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_MatchesWord(Word &wordToMatch,
		UniformNode<Word> *&wordNodeToTest, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content == wordToMatch)
		{

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is populated; if so then sets
	//returnWord to wordNodeToTest->content; if iterate is true, second iterates
	//wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_CaptureWord(UniformNode<Word> *&wordNodeToTest,
		Word &returnWord, const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (wordNodeToTest->content.test_ListPopulated())
		{

			returnWord = wordNodeToTest->content;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest matches falseWord or trueWord; if so then sets
	//value of returnBool accordingly; if iterate is true, second iterates
	//wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_CaptureBool(UniformNode<Word> *&wordNodeToTest,
		bool &returnBool, Word &falseWord, Word &trueWord,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content == falseWord)
		{

			returnBool = false;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else if (wordNodeToTest->content == trueWord)
		{

			returnBool = true;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid unsigned char within the range encompassed by and
	//including minLimit and maxLimit (if minLimitActive and maxLimitActive are true); if so then sets
	//returnUnsignedChar to wordNodeToTest's interpreted value; if iterate is true,
	//second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_UnsignedInt_Limited(UniformNode<Word> *&wordNodeToTest,
		unsigned char &returnUnsignedChar, const unsigned char minLimit = 0,
		const bool maxLimitActive = false, const unsigned char maxLimit = 0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_UnsignedInt())
		{

			unsigned char returnValue = wordNodeToTest->content.interpret_UnsignedShort();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			returnUnsignedChar = returnValue;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid unsigned char; if so then sets
	//returnUnsignedChar to wordNodeToTest's interpreted value; if iterate is true,
	//second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_UnsignedInt_Unlimited(UniformNode<Word> *&wordNodeToTest,
		unsigned char &returnUnsignedChar, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_UnsignedInt())
		{

			returnUnsignedChar = wordNodeToTest->content.interpret_UnsignedShort();

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid unsigned short within the range encompassed by and
	//including minLimit and maxLimit (if minLimitActive and maxLimitActive are true); if so then sets
	//returnUnsignedShort to wordNodeToTest's interpreted value; if iterate is true,
	//second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_UnsignedInt_Limited(UniformNode<Word> *&wordNodeToTest,
		unsigned short &returnUnsignedShort, const unsigned short minLimit = 0,
		const bool maxLimitActive = false, const unsigned short maxLimit = 0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_UnsignedInt())
		{

			unsigned short returnValue = wordNodeToTest->content.interpret_UnsignedShort();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			returnUnsignedShort = returnValue;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid unsigned short; if so then sets
	//returnUnsignedShort to wordNodeToTest's interpreted value; if iterate is true,
	//second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_UnsignedInt_Unlimited(UniformNode<Word> *&wordNodeToTest,
		unsigned short &returnUnsignedShort, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_UnsignedInt())
		{

			returnUnsignedShort = wordNodeToTest->content.interpret_UnsignedShort();

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid unsigned long long within the range encompassed by and
	//including minLimit and maxLimit (if minLimitActive and maxLimitActive are true); if so then sets
	//returnUnsignedLongLong to wordNodeToTest's interpreted value; if iterate is true,
	//second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_UnsignedInt_Limited(UniformNode<Word> *&wordNodeToTest,
		unsigned long long &returnUnsignedLongLong, const unsigned long long minLimit = 0,
		const bool maxLimitActive = false, const unsigned long long maxLimit = 0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_UnsignedInt())
		{

			unsigned long long returnValue = wordNodeToTest->content.interpret_UnsignedLongLong();

			if (returnValue < minLimit)
				return false;

			if (maxLimitActive)
			{

				if (returnValue > maxLimit)
					return false;

			}

			returnUnsignedLongLong = returnValue;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid unsigned long long; if so then sets
	//returnUnsignedLongLong to wordNodeToTest's interpreted value; if iterate is true,
	//second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_UnsignedInt_Unlimited(UniformNode<Word> *&wordNodeToTest,
		unsigned long long &returnUnsignedLongLong, const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_UnsignedInt())
		{

			returnUnsignedLongLong = wordNodeToTest->content.interpret_UnsignedLongLong();

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid floating point within the range encompassed by and
	//including minLimit and maxLimit (if minLimitActive and maxLimitActive are true) and non zero
	//(if nonZero is true); if so then sets returnDouble to wordNodeToTest's interpreted value;
	//if iterate is true, second iterates wordNodeToTest; if so and if ensureNonTerminal is true, then
	//tests to ensure wordNodeToTest != NULL; returns true if conditions are met as
	//defined by parameter values, false otherwise; assumes wordNodeToTest is not NULL
	//and is a member of this WordGroup
	bool validateWordNode_FloatingPoint_Limited(UniformNode<Word> *&wordNodeToTest,
		double &returnDouble, const bool nonZero = false,
		const bool minLimitActive = false, const double minLimit = 0.0,
		const bool maxLimitActive = false, const double maxLimit = 0.0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_FloatingPoint())
		{

			double testValue = wordNodeToTest->content.interpret_Double();

			if (nonZero)
			{

				if (testValue == 0.0)
					return false;

			}

			if (minLimitActive)
			{

				if (testValue < minLimit)
					return false;

			}

			if (maxLimitActive)
			{

				if (testValue > maxLimit)
					return false;

			}

			returnDouble = testValue;

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//first tests whether wordNodeToTest is a valid floating point; if so then sets returnDouble
	//to wordNodeToTest's interpreted value; if iterate is true, second iterates wordNodeToTest;
	//if so and if ensureNonTerminal is true, then tests to ensure wordNodeToTest != NULL;
	//returns true if conditions are met as defined by parameter values, false otherwise;
	//assumes wordNodeToTest is not NULL and is a member of this WordGroup
	bool validateWordNode_FloatingPoint_Unlimited(UniformNode<Word> *&wordNodeToTest,
		double &returndouble, const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (!wordNodeToTest->content.test_ListPopulated())
			return false;

		if (wordNodeToTest->content.validate_FloatingPoint())
		{

			returndouble = wordNodeToTest->content.interpret_Double();

			if (iterate)
			{

				wordNodeToTest = wordNodeToTest->get_NextNode();

				if (ensureNonTerminal)
				{

					if (wordNodeToTest == NULL)
						return false;

					else
						return true;

				}

			}

			return true;

		}

		else
			return false;

	}

	//a console text editor, primarily for managing/editing file strings
	void edit_FromConsole()
	{

		//Construct variables
		Word userInput;
		ValidationWord searchTerm;

		cout << "\t\t\t*** WELCOME TO ANTHONY'S TEXT EDITOR V 1.0 ***";

		//main menu//
		do
		{

		MAIN_MENU:

			cout << "\n\nplease select from the following options:";
			cout << "\n\n\t1) populate this WordGroup via console input";
			cout << "\n\t2) populate this WordGroup via file input";
			cout << "\n\t3) simple console output of all Words in this WordGroup";
			cout << "\n\t4) save this WordGroup's contents to file";

			cout << "\n\n\t10) find and delete subwords of Words in the WordGroup";
			cout << "\n\t11) find and replace subwords of Words in the WordGroup";
			cout << "\n\t12) find and split on subwords of Words in the WordGroup";
			cout << "\n\t13) invert the order of characters in a Word within the WordGroup";
			cout << "\n\t14) find and insert at subwords of Words within the WordGroup";

			cout << "\n\n\t20) add a spacer Word between contiguous Words in the WordGroup";
			cout << "\n\t21) find and delete Words in the WordGroup which wholly match a ValidationWord";
			cout << "\n\t22) merge contiguous words within the WordGroup";
			cout << "\n\t23) invert the order of Words in a single range in the WordGroup";
			cout << "\n\t24) invert the order of Words in all ranges in the WordGroup seperated by a sentinel Word";

			cout << "\n\n\tE) exit the text editor";

			cout << "\n\nyour selection: ";
			userInput.getInput_Console(true);

			//get Wordgroup input via console
			if (userInput == "1")
			{

				bool exclude;
				bool overwrite;

				cout << "\n\nWORDGROUP CONSOLE POPULATION";

				//overwrite decision
				do
				{

					cout << "\nshould current WordGroup be overwritten(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(overwrite))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//exclusion decision
				do
				{

					cout << "\nshould the terminal return be excluded(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(exclude))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				cout << "\ntype input to populate the WordGroup (press enter when finished): ";
				getInput_Console(overwrite, exclude);

				cout << "\noperation completed, return to main menu";

			}

			//get Wordgroup input via file
			else if (userInput == "2")
			{

				bool overWrite;
				Word fileName;

				cout << "\n\nWORDGROUP FILE POPULATION";

				//overwrite decision
				do
				{

					cout << "\nshould current WordGroup be overwritten(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(overWrite))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				cout << "\ntype the exact name of the source input file (press enter when finished): ";
				fileName.getInput_Console();

				getInput_File(fileName, overWrite);

				cout << "\noperation completed, return to main menu";

			}

			//simple WordGroup output
			else if (userInput == "3")
			{

				UniformNode<Word> *wordNode = get_FirstNode();
				unsigned long long count = 0;

				cout << "\n\nWORDGROUP SIMPLE OUTPUT";

				while (wordNode)
				{

					cout << "\nword (" << count << "): \"";
					wordNode->content.output_Console();
					cout << "\"";

					wordNode = wordNode->get_NextNode();
					count++;

				}

				cout << "\noperation completed, return to main menu";

			}

			//WordGroup file output
			else if (userInput == "4")
			{

				Word fileName;
				bool overWrite;

				cout << "\n\nWORDGROUP FILE OUTPUT";

				//populate the fileName
				cout << "\ntype the exact name of the destination file with extension\n";
				fileName.getInput_Console();

				//overwrite decision
				do
				{

					cout << "\nshould data currently contained in the file be overwritten(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(overWrite))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				output_File(fileName, overWrite);

				cout << "\noperation completed, return to main menu";

			}

			//find and delete subwords
			else if (userInput == "10")
			{

				bool splitDecision;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP FIND AND DELETE SUBWORDS";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//populate the searchTerm
				cout << "\ncreate a search term to identify subwords for deletion\n";
				userInput.validateFromConsole_ValidationWord(searchTerm);

				//split decision
				do
				{

					cout << "\nshould words be split where the sub word is found and deleted?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(splitDecision))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_DeleteSubwords(searchTerm, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), splitDecision);

				cout << "\noperation completed, return to main menu";

			}

			//find and replace subwords
			else if (userInput == "11")
			{

				Word replacementWord;
				bool splitBefore;
				bool splitAfter;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP FIND AND REPLACE SUBWORDS";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//populate the searchTerm
				cout << "\ncreate a search term to identify subwords to trigger replacement\n";
				userInput.validateFromConsole_ValidationWord(searchTerm);

				//populate the replacementWord
				cout << "\ncreate a Word which will replace found subwords\n";
				replacementWord.getInput_Console();

				//splitBefore decision
				do
				{

					cout << "\nshould words be split before the found subword?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(splitBefore))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//splitAfter decision
				do
				{

					cout << "\nshould words be split After the found subword?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(splitAfter))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_ReplaceSubwords(searchTerm, replacementWord, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), splitBefore, splitAfter);

				cout << "\noperation completed, return to main menu";

			}

			//find and split on subwords
			else if (userInput == "12")
			{

				bool splitBefore;
				bool splitAfter;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP FIND AND SPLIT ON SUBWORDS";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//populate the searchTerm
				cout << "\ncreate a search term to identify subwords to trigger splitting\n";
				userInput.validateFromConsole_ValidationWord(searchTerm);

				//splitBefore decision
				do
				{

					cout << "\nshould words be split before the found subword?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(splitBefore))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//splitAfter decision
				do
				{

					cout << "\nshould words be split After the found subword?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(splitAfter))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_Split(searchTerm, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), splitBefore, splitAfter);

				cout << "\noperation completed, return to main menu";

			}

			//invert a range of Characters in a Word in the WordGroup
			else if (userInput == "13")
			{

				cout << "\n\n\t\tWORD RANGE INVERSION";

				unsigned short targetWordIndex;
				UniformNode<Word> *targetWord;
				unsigned long long startCharIndex;
				unsigned long long endCharIndex;
				unsigned long long wordGroupLength = get_ListLength();

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//determine target Word
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the target word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(targetWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				targetWord = get_NodeAddress(targetWordIndex);

				//test whether selected Word is populated
				if (!wordGroupLength)
				{

					cout << "\nthe selected Word is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//determine startWordIndex and endWordIndex
				unsigned long long wordLength = targetWord->content.get_ListLength();

				cout << "\nthe selected Word contains " << wordLength << " characters";

				do
				{

					cout << "\nenter the starting character index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startCharIndex,
						0, true, (wordLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending character index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endCharIndex,
						startCharIndex, true, (wordLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				targetWord->content.invert_Range(targetWord->content.get_NodeAddress(startCharIndex),
					targetWord->content.get_NodeAddress(endCharIndex));

				cout << "\noperation completed, return to main menu";

			}

			//find subwords and insert a subword
			else if (userInput == "14")
			{

				ValidationWord searchTerm;
				bool includeTerminalReturn;
				Word insertionWord;
				bool insertAsNewWord;
				bool insertAfter;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP FIND AND INSERT ON SUBWORDS";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//populate the searchTerm
				cout << "\ncreate a search term to identify subwords to trigger splitting\n";
				userInput.validateFromConsole_ValidationWord(searchTerm);

				//includeTerminalReturn decision
				do
				{

					cout << "\nshould the terminal return be included in the Word for insertion?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(includeTerminalReturn))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//populate the replacementWord
				cout << "\ncreate a Word which will be inserted at found subwords\n";
				insertionWord.getInput_Console(false, !includeTerminalReturn);

				//insertAsNewWord decision
				do
				{

					cout << "\nshould the Word be inserted as a new Word?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(insertAsNewWord))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//insertAfter decision
				do
				{

					cout << "\nshould the word be inserted after the found subword?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(insertAfter))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_InsertSubwords(searchTerm, insertionWord, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), insertAsNewWord, insertAfter);

				cout << "\noperation completed, return to main menu";

			}

			//add spacers
			else if (userInput == "20")
			{

				Word spacerWord;
				bool beforeFirst;
				bool afterLast;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP SPACER WORD INSERTION";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				cout << "\ntype to populate the word to be inserted (press enter when finished): ";
				spacerWord.getInput_Console();

				//add before first word decision
				do
				{

					cout << "\nshould the spacer be inserted before the first Word in the range(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(beforeFirst))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//add after last word decision
				do
				{

					cout << "\nshould the spacer be inserted after the last Word in the range(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(afterLast))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_Intersperse(spacerWord, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), beforeFirst, afterLast);

				cout << "\noperation completed, return to main menu";

			}

			//find and delete whole words
			else if (userInput == "21")
			{

				ValidationWord searchTerm;
				bool mergeDecision;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP FIND AND DELETE WHOLE WORDS";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//populate the searchTerm
				cout << "\ncreate a search term to identify whole words for deletion\n";
				userInput.validateFromConsole_ValidationWord(searchTerm);

				//merge decision
				do
				{

					cout << "\nwhen a matching word is found/deleted, should words before and after be merged?(Y/N)? ";

					if (userInput.validateFromConsole_YesNo(mergeDecision))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_DeleteWords(searchTerm, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), mergeDecision);

				cout << "\noperation completed, return to main menu";

			}

			//WordGroup word mergers
			else if (userInput == "22")
			{

				cout << "\n\nWORDGROUP WORD MERGERS";

				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_Merge(get_NodeAddress(startWordIndex), get_NodeAddress(endWordIndex));

				cout << "\noperation completed, return to main menu";

			}

			//invert a range of Words in the WordGroup
			else if (userInput == "23")
			{

				cout << "\n\n\t\tWORDGROUP RANGE INVERSION";

				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				invert_Range(get_NodeAddress(startWordIndex), get_NodeAddress(endWordIndex));

				cout << "\noperation completed, return to main menu";

			}

			//add spacers
			else if (userInput == "24")
			{

				ValidationWord sentinelTerm;
				bool sentinelLeads;
				bool includeSentinel;
				unsigned long long startWordIndex;
				unsigned long long endWordIndex;
				unsigned long long wordGroupLength = get_ListLength();

				cout << "\n\nWORDGROUP MULTI-RANGE INVERSION";

				//test whether WordGroup is populated
				if (!wordGroupLength)
				{

					cout << "\nthe WordGroup is currently empty, populate ";
					cout << "before using this function, return to main menu";
					goto MAIN_MENU;

				}

				//populate the searchTerm
				cout << "\ncreate a search term to identify sentinel Words to bound range inversions\n";
				userInput.validateFromConsole_ValidationWord(searchTerm);

				//sentinel leads decision
				do
				{

					cout << "\nshould the sentinel word lead inverted ranges (Y/N)? ";

					if (userInput.validateFromConsole_YesNo(sentinelLeads))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//include sentinel word decision
				do
				{

					cout << "\nshould the sentinel word be included in the inversion (Y/N)? ";

					if (userInput.validateFromConsole_YesNo(includeSentinel))
						break;

					else
						cout << "\ninvalid entry, try again";

				} while (true);

				//determine startWordIndex and endWordIndex
				cout << "\nthe wordgroup contains " << wordGroupLength << " words";

				do
				{

					cout << "\nenter the starting word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(startWordIndex,
						0, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				do
				{

					cout << "\nenter the ending word index number: ";

					if (userInput.validateFromConsole_UnsignedInt_Limited(endWordIndex,
						startWordIndex, true, (wordGroupLength - 1)))
						break;

					else
						cout << "\ninvalid entry, try again (not a valid integer, or out of range)";

				} while (true);

				//complete the operation
				editRange_InvertSentinelBoundedSubRanges(searchTerm, get_NodeAddress(startWordIndex),
					get_NodeAddress(endWordIndex), sentinelLeads, includeSentinel);

				cout << "\noperation completed, return to main menu";

			}

			else //any other entry
			{

				if (userInput == "E" || userInput == "e")
					break;

				else
					cout << "\ninvalid selection, return to main menu";

			}

		} while (true);

	}

};

class FileValidation_WordGroup : public WordGroup
{

public:

	//default constructor
	FileValidation_WordGroup() {}

	//default destructor
	~FileValidation_WordGroup() {}



};

class JSON_Node
{

private:

	unsigned char type; //0 means object, 1 array, 2 string, 3 number, 4 bool, 5 null
	Word name; //identifier
	Word value; //only used to store value (represented as a Word) for type 2 - 5 nodes

public:

	//default constructor
	JSON_Node() : type() {}

	//modified constructor used to set type
	JSON_Node(const unsigned char aType) : type(aType) {}

	//default destructor
	~JSON_Node() {}

	//cuts and pastes aName into name, assumes name is unpopulated
	void set_Name(Word &aName)
	{
		
		if (aName.test_ListPopulated())
			name.add_CutWholeList(aName);

	}

	//cuts and pastes aValue into value, assumes values is unpopulated
	void set_Value(Word &aValue)
	{

		if (aValue.test_ListPopulated())
			value.add_CutWholeList(aValue);

	}

	//getter
	unsigned char get_Type()
	{

		return type;

	}

	//getter
	Word &get_Name()
	{

		return name;

	}

	//getter
	Word &get_Value()
	{

		return value;

	}

};

class JSON_Object
{

private:

	BranchingList<JSON_Node> structure;

public:

	//default constructor
	JSON_Object() {}

	//default destructor
	~JSON_Object() {}

	//loads initializes this object from fileName or returns an error code along w/
	//the errorCharIndex reference parameter to indicate where the error occurred;
	//assumes all whitespace not embedded in strings has been removed
	unsigned char load_FromFile(Word &fileName, unsigned long long &errorCharIndex)
	{

		ifstream inputFile;

		//check for valide file (and open file)
		if (!open_InputFile(inputFile, fileName))
			return 1;

		//initial setup
		char nextChar = inputFile.get();
		errorCharIndex = 0;

		Word nameWord;
		Word valueWord;
		BranchingNode<JSON_Node> *currentParent(NULL);
		BranchingNode<JSON_Node> *currentChild;
		unsigned char count;
		bool nameExpected(true);
		bool closerExpected;

		//check for first '{'
		if (nextChar != '{')
			return 2;

		nextChar = inputFile.get();
		errorCharIndex++;

		//main loop
		do
		{

			//name section 
			if (nameExpected)
			{

				//check for first '"'
				if (nextChar != '"')
					return 3;

				nextChar = inputFile.get();
				errorCharIndex++;

				do
				{

					//check for end of string
					if (nextChar == '"')
					{

						nextChar = inputFile.get();
						errorCharIndex++;

						break;

					}

					//check for valid use of reverse solidus '\'
					else if (nextChar == '\\')
					{

						nameWord.add_NewNode(nextChar);

						nextChar = inputFile.get();
						errorCharIndex++;

						if (nextChar == '"' || nextChar == '\\' || nextChar == '/'
							|| nextChar == 'b' || nextChar == 'f' || nextChar == 'n'
							|| nextChar == 'r' || nextChar == 't')
							nameWord.add_NewNode(nextChar);

						else if (nextChar == 'u')
						{

							nameWord.add_NewNode(nextChar);

							for (count = 0; count < 4; count++)
							{

								nextChar = inputFile.get();
								errorCharIndex++;

								if ((nextChar >= '0' & nextChar <= '9') || (nextChar >= 'A' && nextChar <= 'F'))
									nameWord.add_NewNode(nextChar);

								else
									return 4;

							}

						}

						else
							return 4;

						nextChar = inputFile.get();
						errorCharIndex++;

					}

					//check for incomplete file
					else if (inputFile.eof())
						return 5;

					else
					{

						nameWord.add_NewNode(nextChar);

						nextChar = inputFile.get();
						errorCharIndex++;

					}

				} while (true);

				//check for ':'
				if (nextChar != ':')
					return 6;

				nextChar = inputFile.get();
				errorCharIndex++;

			}

			//value section

			//object
			if (nextChar == '{')
			{

				currentParent = structure.add_NewNode(JSON_Node(0), currentParent);
				currentParent->content.set_Name(nameWord);
				closerExpected = false;
				nameExpected = true;

				nextChar = inputFile.get();
				errorCharIndex++;

			}
			
			//array
			else if (nextChar == '[')
			{

				currentParent = structure.add_NewNode(JSON_Node(1), currentParent);
				currentParent->content.set_Name(nameWord);
				closerExpected = false;
				nameExpected = false;

				nextChar = inputFile.get();
				errorCharIndex++;

			}

			//string loop to capture string value
			else if (nextChar == '"')
			{

				nextChar = inputFile.get();
				errorCharIndex++;

				do
				{

					//check for end of string
					if (nextChar == '"')
					{

						currentChild = structure.add_NewNode(JSON_Node(2), currentParent);
						currentChild->content.set_Name(nameWord);
						currentChild->content.set_Value(valueWord);

						closerExpected = true;

						nextChar = inputFile.get();
						errorCharIndex++;

						break;

					}

					//check for valid use of reverse solidus '\'
					else if (nextChar == '\\')
					{

						valueWord.add_NewNode(nextChar);

						nextChar = inputFile.get();
						errorCharIndex++;

						if (nextChar == '"' || nextChar == '\\' || nextChar == '/'
							|| nextChar == 'b' || nextChar == 'f' || nextChar == 'n'
							|| nextChar == 'r' || nextChar == 't')
							valueWord.add_NewNode(nextChar);

						else if (nextChar == 'u')
						{

							valueWord.add_NewNode(nextChar);

							for (count = 0; count < 4; count++)
							{

								nextChar = inputFile.get();
								errorCharIndex++;

								if ((nextChar >= '0' & nextChar <= '9') || (nextChar >= 'A' && nextChar <= 'F'))
									valueWord.add_NewNode(nextChar);

								else
									return 7;

							}

						}

						else
							return 7;

						nextChar = inputFile.get();
						errorCharIndex++;

					}

					//check for incomplete file
					else if (inputFile.eof())
						return 5;

					else
					{

						valueWord.add_NewNode(nextChar);

						nextChar = inputFile.get();
						errorCharIndex++;

					}

				} while (true);

			}

			//number
			else if ((nextChar >= '0' && nextChar <= '9') || nextChar == '-' || nextChar == '.')
			{

				do
				{
				
					valueWord.add_NewNode(nextChar);

					nextChar = inputFile.get();
					errorCharIndex++;
				
				} while ((nextChar >= '0' && nextChar <= '9') || nextChar == '-' || nextChar == '.');

				//check for incomplete file
				if (inputFile.eof())
					return 5;

				//check for valid floating point syntax
				if (valueWord.validate_FloatingPoint())
				{

					currentChild = structure.add_NewNode(JSON_Node(3), currentParent);
					currentChild->content.set_Name(nameWord);
					currentChild->content.set_Value(valueWord);

					closerExpected = true;

				}

				else
					return 8;

			}

			//bool - true
			else if (nextChar == 't')
			{

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'r')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'u')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'e')
					return 9;

				valueWord.add_NewNode(nextChar);

				currentChild = structure.add_NewNode(JSON_Node(4), currentParent);
				currentChild->content.set_Name(nameWord);
				currentChild->content.set_Value(valueWord);

				closerExpected = true;

				nextChar = inputFile.get();
				errorCharIndex++;

			}

			//bool - false
			else if (nextChar == 'f')
			{

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'a')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'l')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 's')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'e')
					return 9;

				valueWord.add_NewNode(nextChar);

				currentChild = structure.add_NewNode(JSON_Node(4), currentParent);
				currentChild->content.set_Name(nameWord);
				currentChild->content.set_Value(valueWord);

				closerExpected = true;

				nextChar = inputFile.get();
				errorCharIndex++;

			}

			//null
			else if (nextChar == 'n')
			{

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'u')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'l')
					return 9;

				valueWord.add_NewNode(nextChar);

				nextChar = inputFile.get();
				errorCharIndex++;

				if (nextChar != 'l')
					return 9;

				valueWord.add_NewNode(nextChar);

				currentChild = structure.add_NewNode(JSON_Node(5), currentParent);
				currentChild->content.set_Name(nameWord);
				currentChild->content.set_Value(valueWord);

				closerExpected = true;

				nextChar = inputFile.get();
				errorCharIndex++;

			}

			//invalid value
			else
				return 9;

			while (closerExpected)
			{
				
				if (nextChar == ',')
				{

					//object context
					if (!currentParent || currentParent->content.get_Type() == 0)
						nameExpected = true;

					//array context
					else
						nameExpected = false;


					closerExpected = false;

					nextChar = inputFile.get();
					errorCharIndex++;

				}

				else if (nextChar == '}')
				{

					//outermost object context
					if (!currentParent)
					{

						nextChar = inputFile.get();
						errorCharIndex++;

						//file has valid syntax and JSON object initialized successfully
						//close file and return
						if (inputFile.eof())
						{

							inputFile.close();
							return 0;

						}

						//longer than expected file
						else
							return 10;

					}

					//inner object context
					else if (currentParent->content.get_Type() == 0)
					{

						currentParent = currentParent->get_Parent();

						nextChar = inputFile.get();
						errorCharIndex++;

					}

					//array context - invalid syntax
					else
						return 11;

				}

				else if (nextChar == ']')
				{

					//outermost object context - invalid syntax
					if (!currentParent)
						return 12;

					//array context
					else if (currentParent->content.get_Type() == 1)
					{

						currentParent = currentParent->get_Parent();

						nextChar = inputFile.get();
						errorCharIndex++;

					}

					//inner object context - invalid syntax
					else
						return 12;

				}

				//invalid character where closer expected
				else
					return 13;

			}

		} while (true);

	}

	void output_ToConsole()
	{

		BranchingNode<JSON_Node> *currentParent(NULL);
		BranchingNode<JSON_Node> *currentNode(structure.get_FirstElder());
		unsigned char count;
		unsigned char levelCount(1);

		cout << "\n\n{";

		if (!currentNode)
		{

			cout << "}";
			return;

		}

		do
		{

			//this sibling exists and has children, dive down one level
			if (currentNode->get_FirstChild())
			{

				if (currentNode->content.get_Name().test_ListPopulated())
				{

					cout << "\n";

					for (count = 0; count < levelCount; count++)
						cout << "\t";

					cout << "\"";
					currentNode->content.get_Name().output_Console();
					cout << "\":";

				}

				if (!currentNode->content.get_Type())
					cout << "{";

				else
					cout << "[";
				
				levelCount++;

				currentParent = currentNode;
				currentNode = currentNode->get_FirstChild();

			}

			//this sibling exists but has no children,
			//move laterally by one node
			else
			{

				cout << "\n";

				for (count = 0; count < levelCount; count++)
					cout << "\t";

				if (currentNode->content.get_Name().test_ListPopulated())
				{

					cout << "\"";
					currentNode->content.get_Name().output_Console();
					cout << "\":";

				}

				if (currentNode->content.get_Type() == 2)
				{

					cout << "\"";
					currentNode->content.get_Value().output_Console();
					cout << "\"";

				}

				else
					currentNode->content.get_Value().output_Console();
					
				currentNode = currentNode->get_NextSibling();

				//more sibling(s) exist
				if (currentNode)
					cout << ",";

				//no more siblings exist at this level, move up level(s) and to
				//the parents' next sibling
				else
				{

					do
					{

						//elder nodes are exhausted, exit
						if (!currentParent)
						{

							cout << "\n}";
							return;

						}

						else
						{

							levelCount--;

							cout << "\n";

							for (count = 0; count < levelCount; count++)
								cout << "\t";

							if (!currentParent->content.get_Type())
								cout << "}";

							else
								cout << "]";

							currentNode = currentParent->get_NextSibling();
							currentParent = currentParent->get_Parent();

							if (currentNode)
								cout << ",";

						}

					} while (!currentNode);

				}

			}

		} while (true);

	}

	//***validation functions used in external class initialization functions***

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type or if nodeToTest has no 'next relative'
	//when iterate and ensureNonTerminal are true, returns true otherwise
	bool validateJSON_Node_Object(BranchingNode<JSON_Node> *&nodeToTest,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type())
			return false;

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, wrong name, or if nodeToTest has no 'next relative'
	//when iterate and ensureNonTerminal are true, returns true otherwise
	bool validateJSON_Node_Object(BranchingNode<JSON_Node> *&nodeToTest,
		const char expectedName[], const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() || nodeToTest->content.get_Name() != expectedName)
			return false;

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type or if nodeToTest has no 'next relative'
	//when iterate and ensureNonTerminal are true, returns true otherwise
	bool validateJSON_Node_Array(BranchingNode<JSON_Node> *&nodeToTest,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 1)
			return false;

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, wrong name, or if nodeToTest has no 'next relative'
	//when iterate and ensureNonTerminal are true, returns true otherwise
	bool validateJSON_Node_Array(BranchingNode<JSON_Node> *&nodeToTest,
		const char expectedName[], const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 1 || nodeToTest->content.get_Name() != expectedName)
			return false;

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type or if nodeToTest has no 'next relative'
	//when iterate and ensureNonTerminal are true, returns true otherwise; captures
	//nodeToTest's value in returnWord
	bool validateJSON_Node_String(BranchingNode<JSON_Node> *&nodeToTest,
		Word &returnWord, const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 2)
			return false;

		returnWord = nodeToTest->content.get_Value();

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, wrong name, or if nodeToTest has no 'next relative'
	//when iterate and ensureNonTerminal are true, returns true otherwise; captures
	//nodeToTest's value in returnWord
	bool validateJSON_Node_String(BranchingNode<JSON_Node> *&nodeToTest,
		Word &returnWord, const char expectedName[], const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 2 || nodeToTest->content.get_Name() != expectedName)
			return false;

		returnWord = nodeToTest->content.get_Value();

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, if nonZero is true and interpreted
	//value is zero, if minLimitActive is true and interpreted value is < minLimit,
	//if maxLimitActive is true and interpreted value is > maxLimit, or if nodeToTest has
	//no 'next relative' when iterate and ensureNonTerminal are true, returns true otherwise;
	//captures nodeToTest's interpreted value in returnDouble
	bool validateJSON_Node_FloatingPoint_Limited(BranchingNode<JSON_Node> *&nodeToTest,
		double &returnDouble, const bool nonZero = false,
		const bool minLimitActive = false, const double minLimit = 0.0,
		const bool maxLimitActive = false, const double maxLimit = 0.0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 3)
			return false;

		//note - floating point syntax is validated for any type 2 nodes in
		//JSON_Object::load_FromFile() so that syntax validation need not be repeated here
		double testValue = nodeToTest->content.get_Value().interpret_Double();

		if (nonZero)
		{

			if (testValue == 0.0)
				return false;

		}

		if (minLimitActive)
		{

			if (testValue < minLimit)
				return false;

		}

		if (maxLimitActive)
		{

			if (testValue > maxLimit)
				return false;

		}

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		returnDouble = testValue;
		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, wrong name, if nonZero is true and interpreted
	//value is zero, if minLimitActive is true and interpreted value is < minLimit,
	//if maxLimitActive is true and interpreted value is > maxLimit, or if nodeToTest has
	//no 'next relative' when iterate and ensureNonTerminal are true, returns true otherwise;
	//captures nodeToTest's interpreted value in returnDouble
	bool validateJSON_Node_FloatingPoint_Limited(BranchingNode<JSON_Node> *&nodeToTest,
		double &returnDouble, const char expectedName[], const bool nonZero = false,
		const bool minLimitActive = false, const double minLimit = 0.0,
		const bool maxLimitActive = false, const double maxLimit = 0.0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 3 || nodeToTest->content.get_Name() != expectedName)
			return false;

		//note - floating point syntax is validated for any type 2 nodes in
		//JSON_Object::load_FromFile() so that syntax validation need not be repeated here
		double testValue = nodeToTest->content.get_Value().interpret_Double();

		if (nonZero)
		{

			if (testValue == 0.0)
				return false;

		}

		if (minLimitActive)
		{

			if (testValue < minLimit)
				return false;

		}

		if (maxLimitActive)
		{

			if (testValue > maxLimit)
				return false;

		}

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		returnDouble = testValue;
		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type or if nodeToTest has
	//no 'next relative' when iterate and ensureNonTerminal are true, returns true otherwise;
	//captures nodeToTest's interpreted value in returnDouble
	bool validateJSON_Node_FloatingPoint_Unlimited(BranchingNode<JSON_Node> *&nodeToTest,
		double &returnDouble, const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 3)
			return false;

		//note - floating point syntax is validated for any type 2 nodes in
		//JSON_Object::load_FromFile() so that syntax validation need not be repeated here
		returnDouble = nodeToTest->content.get_Value().interpret_Double();

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, wrong name, or if nodeToTest has
	//no 'next relative' when iterate and ensureNonTerminal are true, returns true otherwise;
	//captures nodeToTest's interpreted value in returnDouble
	bool validateJSON_Node_FloatingPoint_Unlimited(BranchingNode<JSON_Node> *&nodeToTest,
		double &returnDouble, const char expectedName[], const bool iterate = true,
		const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 3 || nodeToTest->content.get_Name() != expectedName)
			return false;

		//note - floating point syntax is validated for any type 2 nodes in
		//JSON_Object::load_FromFile() so that syntax validation need not be repeated here
		returnDouble = nodeToTest->content.get_Value().interpret_Double();

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type, invalid unsigned int syntax, if interpreted
	//value is < minLimit, if maxLimitActive is true and interpreted value is > maxLimit, or if nodeToTest has
	//no 'next relative' when iterate and ensureNonTerminal are true, returns true otherwise;
	//captures nodeToTest's interpreted value in returnUnsignedChar
	bool validateJSON_Node_UnsignedInt_Limited(BranchingNode<JSON_Node> *&nodeToTest,
		unsigned char &returnUnsignedChar, const unsigned char minLimit = 0,
		const bool maxLimitActive = false, const unsigned char maxLimit = 0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 3 || !nodeToTest->content.get_Value().validate_UnsignedInt())
			return false;

		unsigned char testValue = nodeToTest->content.get_Value().interpret_UnsignedChar();

		if (testValue < minLimit)
			return false;

		if (maxLimitActive)
		{

			if (testValue > maxLimit)
				return false;

		}

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		returnUnsignedChar = testValue;
		return true;

	}

	//assumes nodeToTest != NULL; iterates nodeToTest to 'next relative' if iterate
	//is true; returns false for wrong type,, wrong name, invalid unsigned int syntax, if interpreted
	//value is < minLimit, if maxLimitActive is true and interpreted value is > maxLimit, or if nodeToTest has
	//no 'next relative' when iterate and ensureNonTerminal are true, returns true otherwise;
	//captures nodeToTest's interpreted value in returnUnsignedChar
	bool validateJSON_Node_UnsignedInt_Limited(BranchingNode<JSON_Node> *&nodeToTest,
		unsigned char &returnUnsignedChar, const char expectedName[], const unsigned char minLimit = 0,
		const bool maxLimitActive = false, const unsigned char maxLimit = 0,
		const bool iterate = true, const bool ensureNonTerminal = true)
	{

		if (nodeToTest->content.get_Type() != 3 || nodeToTest->content.get_Name() != expectedName
			|| !nodeToTest->content.get_Value().validate_UnsignedInt())
			return false;

		unsigned char testValue = nodeToTest->content.get_Value().interpret_UnsignedChar();

		if (testValue < minLimit)
			return false;

		if (maxLimitActive)
		{

			if (testValue > maxLimit)
				return false;

		}

		if (iterate)
			nodeToTest = nodeToTest->get_NextRelative();

		if (ensureNonTerminal)
		{

			if (!nodeToTest)
				return false;

		}

		returnUnsignedChar = testValue;
		return true;

	}

	//***WORK FROM HERE TO CREATE MORE UNSIGNED INT LIMITED AND UNLIMITED, BOOL, AND NULL VALIDATION FUNCTIONS***

};

#endif // STRINGTOOLS_H_INCLUDED