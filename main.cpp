
#include "Input.h"
#include "MemoryTools.h"
#include "Population.h"
#include "Output.h"
#include "StringTools.h"
#include "Time.h"

using namespace std;

/*

int main()
{

	//initial setup
	ConsoleValidation_Word userInput;
	Population myNetPopulation;
	
	//seed rand()
	srand(time(NULL));

	cout << "\t\t\t*** WELCOME TO ANTHONY'S NEURAL NET V 2.0 ***";

	do
	{

		cout << "\n\nplease select from the following options:";
		cout << "\n\n\t1) load/initialize a population from file";
		cout << "\n\t2) evolve the population via asexual reproduction";
		cout << "\n\t3) evolve the population via sexual reproduction";
		cout << "\n\t4) record to file a single full read";
		cout << "\n\t5) save the current population to file(s)";
		cout << "\n\t6) edit a file using the text editor";
		cout << "\n\t7) compare two files to find diferring values";
		cout << "\n\n\tE) exit the program";

		cout << "\n\nyour selection: ";
		userInput.getInput_Console(true);

		//population initialization
		if (userInput == "1")
		{

			if (myNetPopulation.initialize())
				cout << "\npopulation initialization successfull";

			else
				cout << "\npopulation initialization failed";

		}

		//evolution using asexual reproduction
		else if (userInput == "2")
		{

			unsigned int evolutionCount;
			unsigned short mutationsPerNet;
			double mutationAmplitude;

			cout << "\nenter the number of evolutionary cycles: ";
			while (!userInput.getValid_UnsignedInt_Unlimited(evolutionCount))
				cout << "\ninvalid entry try again: ";

			cout << "\nenter the number of mutations per net each cycle: ";
			while (!userInput.getValid_UnsignedInt_Unlimited(mutationsPerNet))
				cout << "\ninvalid entry try again: ";

			cout << "\nenter the mutation amplitude modifier (double): ";
			while (!userInput.getValid_FloatingPoint_Limited(mutationAmplitude, true, 0.0))
				cout << "\ninvalid entry try again: ";

			myNetPopulation.evolve_Asexual(evolutionCount, mutationsPerNet, mutationAmplitude);

		}

		//evolution using sexual reproduction
		else if (userInput == "3")
		{

			unsigned int evolutionCount;
			unsigned short mutationsPerNet;
			double mutationAmplitude;
			unsigned short maxSpliceLength;

			cout << "\nenter the number of evolutionary cycles: ";
			while (!userInput.getValid_UnsignedInt_Unlimited(evolutionCount))
				cout << "\ninvalid entry try again: ";

			cout << "\nenter the number of mutations per net each cycle: ";
			while (!userInput.getValid_UnsignedInt_Unlimited(mutationsPerNet))
				cout << "\ninvalid entry try again: ";

			cout << "\nenter the mutation amplitude modifier (double): ";
			while (!userInput.getValid_FloatingPoint_Limited(mutationAmplitude, true, 0.0))
				cout << "\ninvalid entry try again: ";

			cout << "\nenter the maximum splice length for meiotic reproduction: ";
			while (!userInput.getValid_UnsignedInt_Limited(maxSpliceLength, 1))
				cout << "\ninvalid entry try again: ";

			myNetPopulation.evolve_Sexual(evolutionCount, mutationsPerNet,
				mutationAmplitude, maxSpliceLength);

		}

		//record a single full read
		else if (userInput == "4")
		{

			unsigned short netsIncluded;
			unsigned char recordingType;

			do
			{

				cout << "\nselect the type or recording to perform: ";
				cout << "\n\t1) XML\n\t2) JSON";
				cout << "\n\tE) exit the menu";
				cout << "\n\nyour selection: ";
				
				userInput.getInput_Console(true);

				if (userInput == "1")
				{

					cout << "\nenter file name for saving the recording report (contents will be overwritten): ";
					userInput.getInput_Console(true);

					if (myNetPopulation.recordedRead(userInput, false))
						cout << "\nXML recorded read successfull";

					else
					{

						cout << "\ninvalid file name, recorded read aborted";
						break;

					}

				}

				else if (userInput == "2")
				{

					cout << "\nenter file name for saving the recording report (contents will be overwritten): ";
					userInput.getInput_Console(true);

					if (myNetPopulation.recordedRead(userInput, true))
						cout << "\nJSON recorded read successfull";

					else
					{

						cout << "\ninvalid file name, recorded read aborted";
						break;

					}

				}

				else if (userInput == "e" || userInput == "E")
					break;

				else
					cout << "\ninvalid entry try again: ";

			} while (true);

		}

		//output Population contents to file(s)
		else if (userInput == "5")
		{

			bool formatType;

			cout << "\nwould you like to save the report in JSON format (no saves as XML, Y/N): ";
			while(!userInput.getValid_YesNo(formatType))
				cout << "invalid entry, try again";

			cout << "\nenter file name for saving the population report (contents will be overwritten): ";
			userInput.getInput_Console(true);

			if (formatType)
				myNetPopulation.output_ToFile_JSON(userInput, true);

			else
				myNetPopulation.output_ToFile_XML(userInput, true);

			cout << "\npopulation report saved to file successfully";

		}

		//file editing
		else if (userInput == "6")
		{

			FileValidation_WordGroup fileContent;
			fileContent.edit_FromConsole();

		}

		//file comparison
		else if (userInput == "7")
		{

			FileValidation_WordGroup firstFile;
			FileValidation_WordGroup secondFile;

			//get file inputs
			do
			{

				cout << "\nenter the name, including extension, of the first file for comparison: ";
				
				userInput.getInput_Console(true);
				firstFile.getInput_File(userInput, true);

				if (firstFile.test_ListPopulated())
					break;

				else
					cout << "invalid entry, try again";

			} while (true);

			do
			{

				cout << "\nenter the name, including extension, of the second file for comparison: ";

				userInput.getInput_Console(true);
				secondFile.getInput_File(userInput);

				if (secondFile.test_ListPopulated())
					break;

				else
					cout << "invalid entry, try again";

			} while (true);

			//delete whitespace
			firstFile.editRange_DeleteWhitespace(firstFile.get_FirstNode(),
				firstFile.get_LastNode(), false);
			secondFile.editRange_DeleteWhitespace(secondFile.get_FirstNode(),
				secondFile.get_LastNode(), false);

			//initial editing/validation
			UniformList<ValidationLetter_ConstructCode> tempConstructCodes;
			tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('<'));
			tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode(">", 0, true, 1, 0));
			tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode('>'));
			ValidationWord universalTag(tempConstructCodes);

			//insert word divisions before and after any angled brace tags
			firstFile.editRange_Split(universalTag, firstFile.get_FirstNode(),
				firstFile.get_LastNode(), true, true);
			secondFile.editRange_Split(universalTag, secondFile.get_FirstNode(),
				secondFile.get_LastNode(), true, true);

			UniformNode<Word> *firstFileWordNode(firstFile.get_FirstNode());
			UniformNode<Word> *secondFileWordNode(secondFile.get_FirstNode());
			double firstValue;
			double secondValue;
			unsigned char count(0);

			cout << "\nfirst file\t\tsecond file (discrepant values)";

			do
			{

				if (firstFileWordNode->content != secondFileWordNode->content)
				{

					firstValue = firstFileWordNode->content.interpret_Double();
					secondValue = secondFileWordNode->content.interpret_Double();

					if (firstValue != secondValue)
					{

						firstFileWordNode->content.output_Console();

						cout << "\t";
						secondFileWordNode->content.output_Console();

						count++;

					}

				}

				if (count > 49)
				{

					cout << "\npress 'e' to exit or any other key to continue: ";
					userInput.getInput_Console(true);

					if (userInput == "E" || userInput == "e")
						break;

					count = 0;
					cout << "\nfirst file\t\tsecond file (discrepant values)";

				}

				firstFileWordNode = firstFileWordNode->get_NextNode();
				secondFileWordNode = secondFileWordNode->get_NextNode();

			} while (firstFileWordNode && secondFileWordNode);
			
			cout << "\nfile comparison completed";

		}

		//exit
		else if (userInput == "E" || userInput == "e")
			break;

		else
			cout << "invalid entry, try again";

	} while (true);

	return 0;

}

*/

int main()
{

	ConsoleValidation_Word userInput;
	JSON_Object JSONStructure;
	unsigned long long errorIndex;

	cout << "\t\t\t*** WELCOME TO ANTHONY'S JSON Validator ***";

	do
	{

		cout << "\n\nenter JSON file name (including extension) for loading: ";
		userInput.getInput_Console(true, true);

		if (!JSONStructure.load_FromFile(userInput, errorIndex))
		{

			cout << "\n\nJSON file loaded successfully, see structure report below:";
			JSONStructure.output_ToConsole();

		}

		else
			cout << "error on character index " << errorIndex << ", invalid file";

	} while (true);

	return 0;

}