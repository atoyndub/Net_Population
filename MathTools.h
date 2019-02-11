#ifndef MATHTOOLS_H_INCLUDED
#define MATHTOOLS_H_INCLUDED

#include "MemoryTools.h"
#include "StringTools.h"

//general math functions

//returns valueToRound rounded to the nearest long long integer value
long long roundToInteger(const double valueToRound)
{

	long long roundedValue(valueToRound);

	if (valueToRound - roundedValue >= 0.5)
		roundedValue++;

	return roundedValue;

}

double squareRoot(double squaredValue, const unsigned char iterationCount)
{

	double tempDouble = 1.0;

	for (unsigned char count = 0; count < iterationCount; count++)
		tempDouble = 0.5 * (tempDouble + (squaredValue / tempDouble));

	return tempDouble;

}

//simple components

class SimpleComponent
{

public:

	//default constructor
	SimpleComponent() {}

	//default destructor
	~SimpleComponent() {}

	//defined in derived classes to return an unsigned char
	//corresponding to type (0 means constant, 1 means variable, 2 means temporary,
	//100 means base class which should never be called)
	virtual unsigned char get_ComponentType()
	{

		return 100;

	}


	//getter function not to be called from this base class
	virtual double get_Value()
	{

		return 0.0;

	}

	//setter function not to be called from this base class
	virtual void set_Value(const double newValue) {}

};

class Constant_Double : public SimpleComponent
{

private:

	double value;

public:

	//default constructor
	Constant_Double() : SimpleComponent(), value(1.0) {}

	//modified constructor
	Constant_Double(const double aValue) : SimpleComponent(), value(aValue) {}

	//default destructor
	~Constant_Double() {}

	//getter for identifying type from base class pointer
	unsigned char get_ComponentType()
	{

		return 0;

	}

	//getter
	double get_Value()
	{

		return value;

	}

	//setter (should never be used for a constant)
	void set_Value(const double newValue) {}

};

//stores external variable for use in calculations
class Variable_Double : public SimpleComponent
{

private:

	Word name;

	//switch to track whether this variable is
	//used in an expression (gets turn on in initialize function
	//but must be turned off manually in the scope and control of
	//variablesList
	bool utilized;

	double *value;
	double safetyValue;

public:

	//default constructor
	Variable_Double() : SimpleComponent(), name("x"), utilized(false),
		value(&safetyValue), safetyValue(1.0) {}

	//modified constructor - assumes aValue != NULL
	Variable_Double(Word &aName, double *aValue) : SimpleComponent(),
		name(aName), utilized(false), value(aValue) {}

	//default destructor
	~Variable_Double() {}

	//copy constructor
	Variable_Double(const Variable_Double& otherVariable)
	{

		name = otherVariable.name;
		utilized = otherVariable.utilized;

		if (otherVariable.value != &(otherVariable.safetyValue))
			value = otherVariable.value;

		else
			value = &safetyValue;

	}

	//copy assignment constructor
	Variable_Double& operator=(Variable_Double const & otherVariable)
	{

		name = otherVariable.name;
		utilized = otherVariable.utilized;

		if (otherVariable.value != &(otherVariable.safetyValue))
			value = otherVariable.value;

		else
			value = &safetyValue;

		return *this;

	}

	//getter for identifying type from base class pointer
	unsigned char get_ComponentType()
	{

		return 1;

	}

	//getter, assumes name will not be modified
	Word &get_Name()
	{

		return name;

	}

	//getter - for use in distinguishing which variables
	//in an initialization list are being utilized; intended
	//to facilitate creation of a short list of pointers to
	//utilized variables to allow for faster updating when
	//needed through the reassign() function
	bool get_Utilized() const
	{

		return utilized;

	}

	//marks this variable as utilized, called in expression
	//initialization
	void set_Utilized(const bool newUtilizedValue)
	{

		utilized = newUtilizedValue;

	}

	//getter
	double get_Value()
	{

		return *value;

	}

	//setter (should never be used for a variable)
	void set_Value(const double newValue) {}

	//setter - for reasigning the variable to a new external double
	//assumes externalValue != NULL
	void reassign_Value(double *newExternalValue)
	{

		value = newExternalValue;

	}

};

//stores temporary values used in sequential calculations
class Temporary_Double : public SimpleComponent
{

private:

	double value;

public:

	//default constructor
	Temporary_Double() : SimpleComponent(), value(1.0) {}

	//default destructor
	~Temporary_Double() {}

	//getter for identifying type from base class pointer
	unsigned char get_ComponentType()
	{

		return 2;

	}

	//getter
	double get_Value()
	{

		return value;

	}

	//setter
	void set_Value(const double newValue)
	{

		value = newValue;

	}

};

//operators

class Operator
{

public:

	//default constructor
	Operator() {}

	//default destructor
	~Operator() {}

	//defined in derived classes to return an unsigned char
	//corresponding to type (0 means assignment, 1 addition,
	//2 subtraction, 3 multiplication, 4 division, 5 exponent,
	//6 root, 7 negative, and 100 means base class which should never be called)
	virtual unsigned char get_OperationType()
	{

		return 100;

	}

	//main operating function - overwritten in all derived classes
	//and virtual here so that derived class overwrites can be called from
	//base class pointers
	virtual void calculate() {}

};

//design notes:
//storageOperand stores a pointer to the expression's keyValues
//element (Temporary_Double) which serves both as the first operand
//used in the operation, as well as the object where this operation's
//final calculated value will be stored

//actionOperand stores the pointer to the SimpleComponent
//which serves as the second operand (in binary operations);
//this may be a Constant_Double or Variable_Double for simple components,
//or a Temporary_Double for complex components corresponding in that case
//the element at the complex component's keyValueIndex within the
//expression's keyValues array

//binary operators
class Assign : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	Assign() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL
	Assign(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~Assign() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 0;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function
	void calculate()
	{

		storageOperand->set_Value(actionOperand->get_Value());

	}

};

class Add : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	Add() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL
	Add(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~Add() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 1;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function
	void calculate()
	{

		double storageValue(storageOperand->get_Value());

		storageOperand->set_Value(storageValue + actionOperand->get_Value());

	}

};

class Subtract : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	Subtract() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL
	Subtract(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~Subtract() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 2;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function
	void calculate()
	{

		double storageValue(storageOperand->get_Value());

		storageOperand->set_Value(storageValue - actionOperand->get_Value());

	}

};

class Multiply : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	Multiply() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL
	Multiply(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~Multiply() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 3;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function
	void calculate()
	{

		double storageValue(storageOperand->get_Value());

		storageOperand->set_Value(storageValue * actionOperand->get_Value());

	}

};

class Divide : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	Divide() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL
	Divide(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~Divide() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 4;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function, assumes *actionOperand != 0
	void calculate()
	{

		double storageValue(storageOperand->get_Value());

		storageOperand->set_Value(storageValue / actionOperand->get_Value());

	}

};

class IntegerExponent : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	IntegerExponent() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL, aStorageOperand is
	//the base, aActionOperand is the power
	IntegerExponent(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~IntegerExponent() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 5;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function; rounds actionOperand to the nearest integer
	//before performing calculation; assumes storageOperand and aActionOperand
	//are not simulatneously both 0 (because that situation is mathematically
	//undefined, even if defined here)
	void calculate()
	{

		long long power(roundToInteger(actionOperand->get_Value()));

		if (power == 0)
			storageOperand->set_Value(1.0);

		else
		{

			double baseValue(storageOperand->get_Value());
			double calculatedValue(baseValue);

			//positive exponent
			if (power > 0)
			{

				while (power > 1)
				{

					calculatedValue *= baseValue;
					power--;

				}

				storageOperand->set_Value(calculatedValue);

			}

			//negative exponent
			else
			{

				while (power < -1)
				{

					calculatedValue *= baseValue;
					power++;

				}

				storageOperand->set_Value(1.0 / calculatedValue);

			}

		}

	}

};

class IntegerRoot : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;
	SimpleComponent *actionOperand;

public:

	//default constructor
	IntegerRoot() : Operator(), storageOperand(&safetyComponent),
		actionOperand(&safetyComponent) {}

	//modified constructor - assumes parameters are non-NULL, aStorageOperand is
	//the base, aActionOperand is the root power
	IntegerRoot(SimpleComponent *aStorageOperand, SimpleComponent *aActionOperand)
		: Operator(), storageOperand(aStorageOperand), actionOperand(aActionOperand) {}

	//default destructor
	~IntegerRoot() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 6;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//for identifying actionOperand's type
	unsigned char get_ActionOperandType()
	{

		return actionOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_ActionOperand()
	{

		return actionOperand;

	}

	//main operating function; rounds actionOperand to the nearest integer
	//before performing calculation; actionOperand determines the power of the
	//root (e.g. 2 means square root, 3 cube root, etc);
	//assumes actionOperand is not 0 (because that situation is mathematically
	//undefined, even if defined here)
	void calculate()
	{

		long long rootPower(roundToInteger(storageOperand->get_Value()));

		if (rootPower == 0)
			storageOperand->set_Value(1.0);

		else
		{

			bool negative;
			unsigned long long count;
			unsigned char charCount;

			double baseValue(actionOperand->get_Value());
			double Xn(1.0);
			double Xn_BaseValue;
			double Xn_to_the_pMinus1;

			//positive root
			if (rootPower > 0)
				negative = false;

			//negative root
			else
			{

				negative = true;
				rootPower *= (-1);

			}

			//the newtonian method: X(n+1) = Xn - (f(Xn) / f'(Xn-1))
			//and power rule for derivatives: d/dx(X^n) = n(x^(n-1)) 
			//lead to the following derivation for finding the pth root
			//(where p is rootPower, a is known value of the root function?):
			//Xn+1 = Xn - (Xn/p) + a/(p * Xn^(p-1))

			//number of iterations set to 20 (arbitrary)
			for (charCount = 0; charCount < 20; charCount++)
			{

				//calculate value of Xn^(p-1)
				Xn_BaseValue = Xn;
				Xn_to_the_pMinus1 = Xn;

				for (count = 2; count < rootPower; count++)
					Xn_to_the_pMinus1 *= Xn_BaseValue;

				Xn = Xn - (Xn / rootPower) + (baseValue / (rootPower * Xn_to_the_pMinus1));

			}

			//complete the calculation
			if (negative)
				storageOperand->set_Value(1.0 / Xn);

			else
				storageOperand->set_Value(Xn);

		}

	}

};

//unary operator
class Negative : public Operator
{

private:

	Constant_Double safetyComponent;

	SimpleComponent *storageOperand;

public:

	//default constructor
	Negative() : Operator(), storageOperand(&safetyComponent) {}

	//modified constructor - assumes parameter is non-NULL
	Negative(SimpleComponent *aStorageOperand) : Operator(), storageOperand(aStorageOperand) {}

	//default destructor
	~Negative() {}

	//getter for identifying type from base class pointer
	unsigned char get_OperationType()
	{

		return 7;

	}

	//for identifying storageOperand's type
	unsigned char get_StorageOperandType()
	{

		return storageOperand->get_ComponentType();

	}

	//used in copy construction and assignment copy construction
	SimpleComponent *get_StorageOperand()
	{

		return storageOperand;

	}

	//main operating function
	void calculate()
	{

		storageOperand->set_Value(-storageOperand->get_Value());

	}

};

class ComponentNode
{

private:

	//the type of component this is
	//simple components (0 constant, 1 variable)
	//complex multi-child components (2 distributive sequence, 3 associative parenthetical)
	//complex single-child components (4 negative, 5 exponential, 6 root)
	unsigned char type;

	//the type of binary operation preceeding this component in the
	//expression; 0 means assignment, 1 addition, 2 subtraction, 3 multiplication,
	//4 division, 5 exponent, 6 root
	unsigned char operationType;

	//pointer to the associated constant or variable (for simple components only),
	//housed within the parent expression; NULL if component is complex (type > 1)
	SimpleComponent *associatedSimpleComponent;

	//keyValues is an array of doubles in the expression
	//which are used to store temporary calculated values after each
	//operation. This property stores the index number of the
	//keyValues element which will store the calculated value of any
	//operation in which this component is the actionOperand or
	//actionOperand (binary) or the storageOperand (unary)
	//not initialized in construction but later in a WrittenExpression
	//sub_Initialize function
	unsigned char keyValueIndex;

public:

	//default constructor - creates an associative parenthetical
	ComponentNode() : type(3), operationType(0),
		associatedSimpleComponent(NULL) {}

	//modified constructor - for constants and variables, assumes aAssociatedSimpleComponent != NULL
	//and assumes aType is either 0 (constant) or 1 (variable) corresponding to the type of derived
	//object to which aAssociatedSimpleComponent actually points.
	ComponentNode(const unsigned char aType, const unsigned char aOperationType,
		SimpleComponent *aAssociatedSimpleComponent) : type(aType), operationType(aOperationType),
		associatedSimpleComponent(aAssociatedSimpleComponent) {}

	//modified constructor - for complex components; assumes aType is one of the following:
	//2 (distributive sequence), 3 (associative parenthetical), 4 (negative),
	//5 (exponential), or 6 (root) corresponding to the type of component this node represents
	ComponentNode(const unsigned char aType, const unsigned char aOperationType) : type(aType),
		operationType(aOperationType), associatedSimpleComponent(NULL) {}

	//getter
	unsigned char get_Type() const
	{

		return type;

	}

	//getter
	unsigned char get_OperationType() const
	{

		return operationType;

	}

	//getter, returns the address of the associated simple component
	SimpleComponent *get_AssociatedSimpleComponent()
	{
	
		return associatedSimpleComponent;
	
	}

	//setter
	void set_OperationType(const unsigned char newOperationType)
	{

		operationType = newOperationType;

	}

	//getter
	unsigned char get_KeyValueIndex() const
	{

		return keyValueIndex;

	}

	//setter
	void set_KeyValueIndex(const unsigned char newKeyValueIndex)
	{

		keyValueIndex = newKeyValueIndex;

	}

};

/*

class conceptual design:

each component stores an index number referencing which element of keyValues
will be updated when the value associated with that node is in the second
operand position in a calculation.

keyValues is an ExtensibleArray<SimpleComponent> member of Expression_Double;
the size of keyValues is calculated in Expression_Double::initialize() based
on the structure of the BranchingList components derived from the Word writtenForm;
keyValues has one element per key component branch (a branch of components which
can calculated serially using a one Temporary_Double to store each successive calculation result;
a new key component branch (and corresponding assigned keyValues element)
starts with any complex component (parenthetical, distributive, power, root) which is not
itself a first child and extends both vertically to descendants level after level until if
and when new key component branches are encountered (again beginning with a complex component
which is not a first child); the keyValueIndex of each component is assigned from top to bottom
of the components structure; the last operation in the expression's operations list always
assignes the calculated value to the [0] element of keyValues

*/

class Expression_Double
{

private:

	//Word which represents the expression
	Word writtenForm;

	//internal storage array for all constant values
	//(variables are stored externally)
	UniformList<Constant_Double> constants;

	//stored operations (both unary and binary)
	UniformList<Negative> negatives;
	UniformList<Assign> assignments;
	UniformList<Add> additions;
	UniformList<Subtract> subtractions;
	UniformList<Multiply> multiplications;
	UniformList<Divide> divisions;
	UniformList<IntegerExponent> exponents;
	UniformList<IntegerRoot> roots;

	//sequenced pointers to each operation in the lists above 
	ExtensibleArray<Operator*> operations;

	//used to store values of each successive calculation in operations
	ExtensibleArray<Temporary_Double> keyValues;

public:

	//default constructor
	Expression_Double() : writtenForm("0"), operations(1), keyValues(1)
	{

		constants.add_NewNode(Constant_Double(0));

		*(operations.get_ElementAddress(0)) = &(assignments.add_NewNode(Assign(
			keyValues.get_ElementAddress(0), &(constants.get_FirstNode()->content)))->content);

	}

	//modified constructor
	Expression_Double(Word &aWrittenForm,
		UniformList<Variable_Double> &initializedVariables)
	{

		initialize(aWrittenForm, initializedVariables);

	}

	//copy constructor
	Expression_Double(const Expression_Double& otherExpression)
	{

		writtenForm = otherExpression.writtenForm;

		constants = otherExpression.constants;

		unsigned long long operationsLength(otherExpression.operations.get_ArrayLength());
		unsigned long long keyValuesLength(otherExpression.keyValues.get_ArrayLength());

		operations.resize_NoCopy(operationsLength);
		keyValues.resize_NoCopy(keyValuesLength);

		unsigned char type;
		unsigned long long opperationCount;
		unsigned long long keyValueCount;
		Operator* currentOperation;
		UniformNode<Constant_Double> *otherConstant;
		UniformNode<Constant_Double> *thisConstant;
		SimpleComponent *actionOperand;
		SimpleComponent *storageOperand;

		UniformNode<Assign> *assignmentNode;
		UniformNode<Add> *additionNode;
		UniformNode<Subtract> *subtractionNode;
		UniformNode<Multiply> *multiplicationNode;
		UniformNode<Divide> *divisionNode;
		UniformNode<IntegerExponent> *exponentNode;
		UniformNode<IntegerRoot> *rootNode;
		UniformNode<Negative> *negativeNode;

		for (opperationCount = 0; opperationCount < operationsLength; opperationCount++)
		{

			currentOperation = otherExpression.operations.get_Element(opperationCount);
			type = currentOperation->get_OperationType();
			
			//assignment
			if (type == 0)
			{

				assignmentNode = otherExpression.assignments.get_FirstNode();

				do
				{

					if (currentOperation == &assignmentNode->content)
						break;

					assignmentNode = assignmentNode->get_NextNode();

				} while (true);

				//find action operand
				type = assignmentNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (assignmentNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (assignmentNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from assignmentNode because variables are stored externally;
				else
					actionOperand = assignmentNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (assignmentNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(assignments.add_NewNode(
					Assign(storageOperand, actionOperand))->content);

			}

			//addition
			else if (type == 1)
			{

				additionNode = otherExpression.additions.get_FirstNode();

				do
				{

					if (currentOperation == &additionNode->content)
						break;

					additionNode = additionNode->get_NextNode();

				} while (true);

				//find action operand
				type = additionNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (additionNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (additionNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from additionNode because variables are stored externally;
				else
					actionOperand = additionNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (additionNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(additions.add_NewNode(
					Add(storageOperand, actionOperand))->content);

			}

			//subtraction
			else if (type == 2)
			{

				subtractionNode = otherExpression.subtractions.get_FirstNode();

				do
				{

					if (currentOperation == &subtractionNode->content)
						break;

					subtractionNode = subtractionNode->get_NextNode();

				} while (true);

				//find action operand
				type = subtractionNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (subtractionNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (subtractionNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from subtractionNode because variables are stored externally;
				else
					actionOperand = subtractionNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (subtractionNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(subtractions.add_NewNode(
					Subtract(storageOperand, actionOperand))->content);

			}

			//multiplication
			else if (type == 3)
			{

				multiplicationNode = otherExpression.multiplications.get_FirstNode();

				do
				{

					if (currentOperation == &multiplicationNode->content)
						break;

					multiplicationNode = multiplicationNode->get_NextNode();

				} while (true);

				//find action operand
				type = multiplicationNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (multiplicationNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (multiplicationNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from multiplicationNode because variables are stored externally;
				else
					actionOperand = multiplicationNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (multiplicationNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(multiplications.add_NewNode(
					Multiply(storageOperand, actionOperand))->content);

			}

			//division
			else if (type == 4)
			{

				divisionNode = otherExpression.divisions.get_FirstNode();

				do
				{

					if (currentOperation == &divisionNode->content)
						break;

					divisionNode = divisionNode->get_NextNode();

				} while (true);

				//find action operand
				type = divisionNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (divisionNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (divisionNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from divsionNode because variables are stored externally;
				else
					actionOperand = divisionNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (divisionNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(divisions.add_NewNode(
					Divide(storageOperand, actionOperand))->content);

			}

			//exponent
			else if (type == 5)
			{

				exponentNode = otherExpression.exponents.get_FirstNode();

				do
				{

					if (currentOperation == &exponentNode->content)
						break;

					exponentNode = exponentNode->get_NextNode();

				} while (true);

				//find action operand
				type = exponentNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (exponentNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (exponentNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from divsionNode because variables are stored externally;
				else
					actionOperand = exponentNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (exponentNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(exponents.add_NewNode(
					IntegerExponent(storageOperand, actionOperand))->content);

			}

			//root
			else if (type == 6)
			{

				rootNode = otherExpression.roots.get_FirstNode();

				do
				{

					if (currentOperation == &rootNode->content)
						break;

					rootNode = rootNode->get_NextNode();

				} while (true);

				//find action operand
				type = rootNode->content.get_ActionOperandType();

				//actionOperand points to a keyValue
				if (type == 2)
				{

					keyValueCount = 0;

					do
					{

						if (rootNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					actionOperand = keyValues.get_ElementAddress(keyValueCount);

				}

				//actionOperand points to a constant
				else if (type == 0)
				{

					otherConstant = otherExpression.constants.get_FirstNode();
					thisConstant = constants.get_FirstNode();

					do
					{

						if (rootNode->content.get_ActionOperand() == &(otherConstant->content))
							break;

						otherConstant = otherConstant->get_NextNode();
						thisConstant = thisConstant->get_NextNode();

					} while (true);

					actionOperand = &(thisConstant->content);

				}

				//actionOperand points to a variable which can be taken directly
				//from divsionNode because variables are stored externally;
				else
					actionOperand = rootNode->content.get_ActionOperand();

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (rootNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(roots.add_NewNode(
					IntegerRoot(storageOperand, actionOperand))->content);

			}

			//negative
			else if (type == 7)
			{

				negativeNode = otherExpression.negatives.get_FirstNode();

				do
				{

					if (currentOperation == &negativeNode->content)
						break;

					negativeNode = negativeNode->get_NextNode();

				} while (true);

				//find storage operand (always points to a keyValue)
				keyValueCount = 0;

				do
				{

					if (negativeNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
						break;

					keyValueCount++;

				} while (true);

				storageOperand = keyValues.get_ElementAddress(keyValueCount);

				//add create the new operation
				*(operations.get_ElementAddress(opperationCount)) = &(negatives.add_NewNode(
					Negative(storageOperand))->content);

			}

		}

	}

	//copy assignement operator
	Expression_Double& operator=(Expression_Double const& otherExpression)
	{

		if (&otherExpression != this)
		{

			writtenForm = otherExpression.writtenForm;

			constants = otherExpression.constants;

			unsigned long long operationsLength(otherExpression.operations.get_ArrayLength());
			unsigned long long keyValuesLength(otherExpression.keyValues.get_ArrayLength());

			operations.resize_NoCopy(operationsLength);
			keyValues.resize_NoCopy(keyValuesLength);

			assignments.delete_WholeList();
			additions.delete_WholeList();
			subtractions.delete_WholeList();
			multiplications.delete_WholeList();
			divisions.delete_WholeList();
			exponents.delete_WholeList();
			roots.delete_WholeList();
			negatives.delete_WholeList();

			unsigned char type;
			unsigned long long opperationCount;
			unsigned long long keyValueCount;
			Operator* currentOperation;
			UniformNode<Constant_Double> *otherConstant;
			UniformNode<Constant_Double> *thisConstant;
			SimpleComponent *actionOperand;
			SimpleComponent *storageOperand;

			UniformNode<Assign> *assignmentNode;
			UniformNode<Add> *additionNode;
			UniformNode<Subtract> *subtractionNode;
			UniformNode<Multiply> *multiplicationNode;
			UniformNode<Divide> *divisionNode;
			UniformNode<IntegerExponent> *exponentNode;
			UniformNode<IntegerRoot> *rootNode;
			UniformNode<Negative> *negativeNode;

			for (opperationCount = 0; opperationCount < operationsLength; opperationCount++)
			{

				currentOperation = otherExpression.operations.get_Element(opperationCount);
				type = currentOperation->get_OperationType();

				//assignment
				if (type == 0)
				{

					assignmentNode = otherExpression.assignments.get_FirstNode();

					do
					{

						if (currentOperation == &assignmentNode->content)
							break;

						assignmentNode = assignmentNode->get_NextNode();

					} while (true);

					//find action operand
					type = assignmentNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (assignmentNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (assignmentNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from assignmentNode because variables are stored externally;
					else
						actionOperand = assignmentNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (assignmentNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(assignments.add_NewNode(
						Assign(storageOperand, actionOperand))->content);

				}

				//addition
				else if (type == 1)
				{

					additionNode = otherExpression.additions.get_FirstNode();

					do
					{

						if (currentOperation == &additionNode->content)
							break;

						additionNode = additionNode->get_NextNode();

					} while (true);

					//find action operand
					type = additionNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (additionNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (additionNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from additionNode because variables are stored externally;
					else
						actionOperand = additionNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (additionNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(additions.add_NewNode(
						Add(storageOperand, actionOperand))->content);

				}

				//subtraction
				else if (type == 2)
				{

					subtractionNode = otherExpression.subtractions.get_FirstNode();

					do
					{

						if (currentOperation == &subtractionNode->content)
							break;

						subtractionNode = subtractionNode->get_NextNode();

					} while (true);

					//find action operand
					type = subtractionNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (subtractionNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (subtractionNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from subtractionNode because variables are stored externally;
					else
						actionOperand = subtractionNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (subtractionNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(subtractions.add_NewNode(
						Subtract(storageOperand, actionOperand))->content);

				}

				//multiplication
				else if (type == 3)
				{

					multiplicationNode = otherExpression.multiplications.get_FirstNode();

					do
					{

						if (currentOperation == &multiplicationNode->content)
							break;

						multiplicationNode = multiplicationNode->get_NextNode();

					} while (true);

					//find action operand
					type = multiplicationNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (multiplicationNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (multiplicationNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from multiplicationNode because variables are stored externally;
					else
						actionOperand = multiplicationNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (multiplicationNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(multiplications.add_NewNode(
						Multiply(storageOperand, actionOperand))->content);

				}

				//division
				else if (type == 4)
				{

					divisionNode = otherExpression.divisions.get_FirstNode();

					do
					{

						if (currentOperation == &divisionNode->content)
							break;

						divisionNode = divisionNode->get_NextNode();

					} while (true);

					//find action operand
					type = divisionNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (divisionNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (divisionNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from divsionNode because variables are stored externally;
					else
						actionOperand = divisionNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (divisionNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(divisions.add_NewNode(
						Divide(storageOperand, actionOperand))->content);

				}

				//exponent
				else if (type == 5)
				{

					exponentNode = otherExpression.exponents.get_FirstNode();

					do
					{

						if (currentOperation == &exponentNode->content)
							break;

						exponentNode = exponentNode->get_NextNode();

					} while (true);

					//find action operand
					type = exponentNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (exponentNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (exponentNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from divsionNode because variables are stored externally;
					else
						actionOperand = exponentNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (exponentNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(exponents.add_NewNode(
						IntegerExponent(storageOperand, actionOperand))->content);

				}

				//root
				else if (type == 6)
				{

					rootNode = otherExpression.roots.get_FirstNode();

					do
					{

						if (currentOperation == &rootNode->content)
							break;

						rootNode = rootNode->get_NextNode();

					} while (true);

					//find action operand
					type = rootNode->content.get_ActionOperandType();

					//actionOperand points to a keyValue
					if (type == 2)
					{

						keyValueCount = 0;

						do
						{

							if (rootNode->content.get_ActionOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
								break;

							keyValueCount++;

						} while (true);

						actionOperand = keyValues.get_ElementAddress(keyValueCount);

					}

					//actionOperand points to a constant
					else if (type == 0)
					{

						otherConstant = otherExpression.constants.get_FirstNode();
						thisConstant = constants.get_FirstNode();

						do
						{

							if (rootNode->content.get_ActionOperand() == &(otherConstant->content))
								break;

							otherConstant = otherConstant->get_NextNode();
							thisConstant = thisConstant->get_NextNode();

						} while (true);

						actionOperand = &(thisConstant->content);

					}

					//actionOperand points to a variable which can be taken directly
					//from divsionNode because variables are stored externally;
					else
						actionOperand = rootNode->content.get_ActionOperand();

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (rootNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(roots.add_NewNode(
						IntegerRoot(storageOperand, actionOperand))->content);

				}

				//negative
				else if (type == 7)
				{

					negativeNode = otherExpression.negatives.get_FirstNode();

					do
					{

						if (currentOperation == &negativeNode->content)
							break;

						negativeNode = negativeNode->get_NextNode();

					} while (true);

					//find storage operand (always points to a keyValue)
					keyValueCount = 0;

					do
					{

						if (negativeNode->content.get_StorageOperand() == otherExpression.keyValues.get_ElementAddress(keyValueCount))
							break;

						keyValueCount++;

					} while (true);

					storageOperand = keyValues.get_ElementAddress(keyValueCount);

					//add create the new operation
					*(operations.get_ElementAddress(opperationCount)) = &(negatives.add_NewNode(
						Negative(storageOperand))->content);

				}

			}

		}

		return *this;

	}

	//validates syntax of aWrittenForm, and constructs a corresponding
	//expression if valid and if any/all variables represented in
	//aWrittenForm are contained within initializedVariables (identified
	//by name); assumes initializedVariables (and the doubles it references)
	//will remain in scope and more or less intact w/ any current members
	//(at least those referred to by this expression) for the lifetime of
	//this expression; returns true if syntax is valid and initialization successfull,
	//returns false and sets this expression to a null state otherwise
	bool initialize(Word &aWrittenForm, UniformList<Variable_Double> &initializedVariables)
	{

		//structured list of all sub-components, simple and complex,
		//contained which will be contained within this expresion
		BranchingList<ComponentNode> components;

		//phase 1 - validate syntax while populating components
		if(!sub_Initialize_ValidateSyntax(aWrittenForm, components, initializedVariables))
		{

			constants.delete_WholeList();

			negatives.delete_WholeList();
			assignments.delete_WholeList();
			additions.delete_WholeList();
			subtractions.delete_WholeList();
			multiplications.delete_WholeList();
			divisions.delete_WholeList();
			exponents.delete_WholeList();
			roots.delete_WholeList();

			writtenForm = "0";
			operations.resize_NoCopy(1);
			keyValues.resize_NoCopy(1);

			constants.add_NewNode(Constant_Double(0));

			*(operations.get_ElementAddress(0)) = &(assignments.add_NewNode(Assign(
				keyValues.get_ElementAddress(0), &(constants.get_FirstNode()->content)))->content);

			return false;

		}

		//phase 2 - syntax has been validated, now sequence operations
		sub_Initialize_SequenceOperations(components, initializedVariables);

		//copy written form
		writtenForm = aWrittenForm;

		//***TESTING***

		/*

		BranchingNode<ComponentNode> *currentNode = components.get_FirstElder();
		BranchingNode<ComponentNode> *currentParentNode = NULL;
		unsigned short levelCount = 0;
		unsigned short count;
		unsigned char testChar;

		cout << "\n\ncomponents structure report:";
		cout << "firstElder = " << components.get_FirstElder();
		cout << ", lastElder = " << components.get_LastElder();
		cout << ", elderNodeCount = " << components.get_ElderNodeCount();
		cout << ", totalNodeCount = " << components.get_TotalNodeCount();

		do
		{

			//there are no more siblings at this level
			if (!currentNode)
			{

				//the proceedure is not at the top level, move up one level
				//and laterally by one node
				if (currentParentNode)
				{

					levelCount--;

					currentNode = currentParentNode->get_NextSibling();
					currentParentNode = currentParentNode->get_Parent();

				}

				//the proceedure is concluded
				else
					break;

			}

			//this sibling exists and has children, dive down one level
			else if (currentNode->get_FirstChild())
			{

				//output information about currentNode
				cout << "\n\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "node (this = " << currentNode;
				cout << ", parent = " << currentNode->get_Parent();
				cout << ", priorSibling = " << currentNode->get_PriorSibling();
				cout << ", nextSibling = " << currentNode->get_NextSibling() << ",\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "totalChildren = " << currentNode->get_ChildCount();
				cout << ", firstChild = " << currentNode->get_FirstChild();
				cout << ", lastChild = " << currentNode->get_LastChild() << ",\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "type = ";
				testChar = currentNode->content.get_Type();
				if (testChar == 0)
					cout << "CONSTANT";
				else if (testChar == 1)
					cout << "VARIABLE";
				else if (testChar == 2)
					cout << "DISTRIBUTIVE";
				else if (testChar == 3)
					cout << "PARENTHETICAL";
				else if (testChar == 4)
					cout << "NEGATIVE";
				else if (testChar == 5)
					cout << "EXPONENTIAL";
				else if (testChar == 6)
					cout << "ROOT";
				testChar = currentNode->content.get_OperationType();
				cout << ", operationType = ";
				if (testChar == 0)
					cout << "ASSIGNMENT";
				else if (testChar == 1)
					cout << "ADDITION";
				else if (testChar == 2)
					cout << "SUBTRACTION";
				else if (testChar == 3)
					cout << "MULTIPLICATION";
				else if (testChar == 4)
					cout << "DIVISION";
				else if (testChar == 5)
					cout << "EXPONENT";
				else if (testChar == 6)
					cout << "ROOT";
				cout << ", keyValueIndex = " << static_cast<int>(currentNode->content.get_KeyValueIndex()) << ")";

				levelCount++;

				currentParentNode = currentNode;
				currentNode = currentNode->get_FirstChild();

			}

			//this sibling exists but has no children,
			//move laterally by one node
			else
			{

				//output information about currentNode
				cout << "\n\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "node (this = " << currentNode;
				cout << ", parent = " << currentNode->get_Parent();
				cout << ", priorSibling = " << currentNode->get_PriorSibling();
				cout << ", nextSibling = " << currentNode->get_NextSibling() << ",\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "totalChildren = " << currentNode->get_ChildCount();
				cout << ", firstChild = " << currentNode->get_FirstChild();
				cout << ", lastChild = " << currentNode->get_LastChild() << ",\n";
				for (count = 0; count < levelCount; count++)
					cout << "\t";
				cout << "type = ";
				testChar = currentNode->content.get_Type();
				if (testChar == 0)
					cout << "CONSTANT";
				else if (testChar == 1)
					cout << "VARIABLE";
				else if (testChar == 2)
					cout << "DISTRIBUTIVE";
				else if (testChar == 3)
					cout << "PARENTHETICAL";
				else if (testChar == 4)
					cout << "NEGATIVE";
				else if (testChar == 5)
					cout << "EXPONENTIAL";
				else if (testChar == 6)
					cout << "ROOT";
				testChar = currentNode->content.get_OperationType();
				cout << ", operationType = ";
				if (testChar == 0)
					cout << "ASSIGNMENT";
				else if (testChar == 1)
					cout << "ADDITION";
				else if (testChar == 2)
					cout << "SUBTRACTION";
				else if (testChar == 3)
					cout << "MULTIPLICATION";
				else if (testChar == 4)
					cout << "DIVISION";
				else if (testChar == 5)
					cout << "EXPONENT";
				else if (testChar == 6)
					cout << "ROOT";
				cout << ", keyValueIndex = " << static_cast<int>(currentNode->content.get_KeyValueIndex()) << ")";

				currentNode = currentNode->get_NextSibling();

			}

		} while (true);

		*/

		//***TESTING***

		return true;

	}

	//validates syntax of workingCopy, and populates components if valid and
	//if any/all variables represented in workingCopy are contained within
	//initializedVariables (identified by name); returns the number of required
	//key values (> 0) if syntax is valid, returns 0 otherwise
	bool sub_Initialize_ValidateSyntax(Word &aWrittenForm,
		BranchingList<ComponentNode> &components, UniformList<Variable_Double> &initializedVariables)
	{

		if (!aWrittenForm.test_ListPopulated())
			return false;

		//delete any content of the current storage lists
		//(in case this expression has been previously populated)
		constants.delete_WholeList();

		negatives.delete_WholeList();
		additions.delete_WholeList();
		subtractions.delete_WholeList();
		multiplications.delete_WholeList();
		divisions.delete_WholeList();
		exponents.delete_WholeList();
		roots.delete_WholeList();

		//syntax validation setup
		UniformList<ValidationLetter_ConstructCode> tempConstructCodes;
		tempConstructCodes.add_NewNode(ValidationLetter_ConstructCode("()+*-/^#", 0));
		ValidationWord nonVariableTerm(tempConstructCodes);

		FileValidation_WordGroup workingCopy;
		UniformNode<Word> *wordNode = workingCopy.add_NewNode();
		wordNode->content = aWrittenForm;

		//delete whitespace and parse before and after all parentheses and operators
		workingCopy.editRange_DeleteWhitespace(wordNode, wordNode, false);
		workingCopy.editRange_Split(nonVariableTerm, wordNode, wordNode, true, true);

		wordNode = workingCopy.get_FirstNode();

		Word leftParenthesis("(");
		Word rightParenthesis(")");
		Word plus("+");
		Word minus("-");
		Word multiply("*");
		Word divide("/");
		Word exponent("^");
		Word root("#");

		unsigned char unmatchedParentheses = 0; //the current number of unpaired left parentheses
		bool operationRequired = false; //false means operand required, true, a binary operation
		unsigned char lastOperation = 0; //code of the most recent binary operation (0 means assignment, 1 addition, 2 subtraction, 3 multiplication, 4 division, 5 exponent, 6 root)

		double constantValue;
		UniformNode<Variable_Double> *variableNode;

		BranchingNode<ComponentNode> *currentParent = NULL;
		BranchingNode<ComponentNode> *currentChild;

		//used to track variables referred to by this expression
		//and mark them as utilized after syntax has been validated
		UniformList<Variable_Double*> tempVariablesList;

		//test for valid syntax of aWrittenForm and build
		//components branching list structure simultaneously
		do
		{

			//a constant, variable, or parenthetical is syntactically required
			if (!operationRequired)
			{

				//test for constant
				if (workingCopy.validateWordNode_FloatingPoint_Unlimited(
					wordNode, constantValue, true, false))
				{

					//add a new constant to constants and a corresponding
					//new component to the structured components list
					components.add_NewNode(ComponentNode(0, lastOperation, &(constants.add_NewNode(
						Constant_Double(constantValue))->content)), currentParent);

					//if and while the current context is exponential, root,
					//or negative, move up level(s)
					while (currentParent && currentParent->content.get_Type() > 3)
						currentParent = currentParent->get_Parent();

					operationRequired = true;

				}

				//test for variable
				else if (workingCopy.validateWordNode_AlphaNumeric(wordNode, false, false)
					|| workingCopy.validateWordNode_AlphaNumericIndexed(wordNode, false, false))
				{

					//test whether the found variable is in the initializedVariables
					//list
					variableNode = initializedVariables.get_FirstNode();

					while (variableNode)
					{

						if (variableNode->content.get_Name() == wordNode->content)
							break;

						variableNode = variableNode->get_NextNode();

					}

					//a variable in the expression was not found in the initialized
					//variables list, reset to null state
					if (!variableNode)
						return 0;


					//add a new component to the structured components list
					//which refers to the external variable in initializedVariables
					components.add_NewNode(ComponentNode(1, lastOperation,
						&(variableNode->content)), currentParent);

					//add this variable to temp list for later marking as utilized
					tempVariablesList.add_NewNode(&(variableNode->content));

					//if and while the current context is exponential, root,
					//or negative, move up level(s)
					while (currentParent && currentParent->content.get_Type() > 3)
						currentParent = currentParent->get_Parent();

					operationRequired = true;

					wordNode = wordNode->get_NextNode();

				}

				//test for parenthetical start
				else if (workingCopy.validateWordNode_MatchesWord(
					leftParenthesis, wordNode))
				{

					currentParent = components.add_NewNode(ComponentNode(3,
						lastOperation), currentParent);

					unmatchedParentheses++;
					lastOperation = 0;

				}

				//test for any preceeding negative unary operand
				else if (workingCopy.validateWordNode_MatchesWord(
					minus, wordNode))
				{

					currentParent = components.add_NewNode(ComponentNode(4,
						lastOperation), currentParent);

					lastOperation = 0;

				}
				
				//syntax error
				else
					return false;

			}

			//a binary operation is syntactically required
			else
			{

				//test for plus
				if (workingCopy.validateWordNode_MatchesWord(plus, wordNode))
				{

					//if currentParent is a distributive sequence, move up one level
					if (currentParent && currentParent->content.get_Type() == 2)
						currentParent = currentParent->get_Parent();

					lastOperation = 1;
					operationRequired = false;

				}

				//test for minus
				else if (workingCopy.validateWordNode_MatchesWord(minus, wordNode))
				{

					//if currentParent is a distributive sequence, move up one level
					if (currentParent && currentParent->content.get_Type() == 2)
						currentParent = currentParent->get_Parent();

					lastOperation = 2;
					operationRequired = false;

				}

				//test for multiply
				else if (workingCopy.validateWordNode_MatchesWord(multiply, wordNode))
				{

					if (currentParent)
					{

						//currentParent is a lower-level associative parenthetical; create a new distributive
						//sequence and move the most recent preceding component (and any of its contents) to
						//new position as the first child of the new distributive sequence
						if (currentParent->content.get_Type() == 3)
						{

							//the most recent preceeding component to be moved
							//(should never be NULL by this assignment)
							currentChild = currentParent->get_LastChild();

							//create the new distributive sequence
							currentParent = components.add_NewNode(ComponentNode(2,
								currentChild->content.get_OperationType()), currentParent);

							//reset currentChild's binary operation type to assignment and 
							//relocate it to be the firstChild of the new distributive sequence
							currentChild->content.set_OperationType(0);
							components.move_Branch(currentChild, currentParent);

						}

					}

					//currentParent is the outermost pseudo parenthetical
					else
					{

						//the most recent preceeding component to be moved
						//(should never be NULL by this assignment)
						currentChild = components.get_LastElder();

						//create the new distributive sequence
						currentParent = components.add_NewNode(ComponentNode(2,
							currentChild->content.get_OperationType()));

						//reset currentChild's binary operation type to assignment and 
						//relocate it to be the firstChild of the new distributive sequence
						currentChild->content.set_OperationType(0);
						components.move_Branch(currentChild, currentParent);

					}

					lastOperation = 3;
					operationRequired = false;

				}

				//test for divide
				else if (workingCopy.validateWordNode_MatchesWord(divide, wordNode))
				{

					if (currentParent)
					{

						//currentParent is a lower-level associative parenthetical; create a new distributive
						//sequence and move the most recent preceding component (and any of its contents) to
						//new position as the first child of the new distributive sequence
						if (currentParent->content.get_Type() == 3)
						{

							//the most recent preceeding component to be moved
							//(should never be NULL by this assignment)
							currentChild = currentParent->get_LastChild();

							//create the new distributive sequence
							currentParent = components.add_NewNode(ComponentNode(2,
								currentChild->content.get_OperationType()), currentParent);

							//reset currentChild's binary operation type to assignment and 
							//relocate it to be the firstChild of the new distributive sequence
							currentChild->content.set_OperationType(0);
							components.move_Branch(currentChild, currentParent);

						}

					}

					//currentParent is the outermost pseudo parenthetical
					else
					{

						//the most recent preceeding component to be moved
						//(should never be NULL by this assignment)
						currentChild = components.get_LastElder();

						//create the new distributive sequence
						currentParent = components.add_NewNode(ComponentNode(2,
							currentChild->content.get_OperationType()));

						//reset currentChild's binary operation type to assignment and 
						//relocate it to be the firstChild of the new distributive sequence
						currentChild->content.set_OperationType(0);
						components.move_Branch(currentChild, currentParent);

					}

					lastOperation = 4;
					operationRequired = false;

				}

				//test for exponent (and apply to most recent node added)
				else if (workingCopy.validateWordNode_MatchesWord(
					exponent, wordNode))
				{

					//create a new exponential; if last sibling is a simple component,
					//parenthetical, or negative, move it (and any of its contents) to
					//new position as the first child of the new exponent; if lastChild
					//is distributive sequence, exponential, or root, dive down levels
					//until a last child of one of the other types if found
					if (currentParent)
						currentChild = currentParent->get_LastChild();

					else
						currentChild = components.get_LastElder();

					//dive down levels if needed
					while (currentChild->content.get_Type() == 2
						|| currentChild->content.get_Type() > 4)
					{

						currentParent = currentChild;
						currentChild = currentParent->get_LastChild();

					}

					//create the new exponent
					currentParent = components.add_NewNode(ComponentNode(5,
						currentChild->content.get_OperationType()), currentParent);

					//reset currentChild's binary operation type to assignment and 
					//relocate it to be the firstChild of the new root
					currentChild->content.set_OperationType(0);
					components.move_Branch(currentChild, currentParent);

					lastOperation = 5;
					operationRequired = false;

				}

				//test for root (and apply to most recent node added)
				else if (workingCopy.validateWordNode_MatchesWord(
					root, wordNode))
				{

					//create a new root; if last sibling is a simple component,
					//parenthetical, or negative, move it (and any of its contents) to
					//new position as the first child of the new root; if lastChild
					//is distributive sequence, exponential, or root, dive down levels
					//until a last child of one of the other types if found
					if (currentParent)
						currentChild = currentParent->get_LastChild();

					else
						currentChild = components.get_LastElder();

					//dive down levels if needed
					while (currentChild->content.get_Type() == 2
						|| currentChild->content.get_Type() > 4)
					{

						currentParent = currentChild;
						currentChild = currentParent->get_LastChild();

					}

					//create the new root
					currentParent = components.add_NewNode(ComponentNode(6,
						currentChild->content.get_OperationType()), currentParent);

					//reset currentChild's binary operation type to assignment and 
					//relocate it to be the firstChild of the new root
					currentChild->content.set_OperationType(0);
					components.move_Branch(currentChild, currentParent);

					lastOperation = 6;
					operationRequired = false;

				}

				//test for parenthetical end
				else if (workingCopy.validateWordNode_MatchesWord(
					rightParenthesis, wordNode, true, false) && unmatchedParentheses > 0)
				{

					//current context is distributive sequence, move up one level
					if (currentParent->content.get_Type() == 2)
						currentParent = currentParent->get_Parent();

					//move up one level out of the current parenthetical
					currentParent = currentParent->get_Parent();

					//if and while the current context is exponential,
					//root, or negative move up level(s)
					while (currentParent && currentParent->content.get_Type() > 3)
						currentParent = currentParent->get_Parent();

					unmatchedParentheses--;

				}

				//syntax error
				else
					return false;

			}

		} while (wordNode);

		if (unmatchedParentheses)
			return false;

		if (!operationRequired)
			return false;

		//mark any variables in the expression as utilized 
		UniformNode<Variable_Double*> *tempVariableNode(tempVariablesList.get_FirstNode());

		while (tempVariableNode)
		{

			tempVariableNode->content->set_Utilized(true);

			tempVariableNode = tempVariableNode->get_NextNode();

		}

		return true;

	}

	//assumes components is initialized with at least one member and structure
	//corresponds to a syntactically valid written expression
	void sub_Initialize_SequenceOperations(BranchingList<ComponentNode> &components,
		UniformList<Variable_Double> &initializedVariables)
	{

		//initial setup
		unsigned char keyValueIndex(0);
		unsigned char keyValueMax(0);
		BranchingNode<ComponentNode> *currentNode(components.get_FirstElder());
		BranchingNode<ComponentNode> *currentParentNode(NULL);
		bool concluded(false);

		//assign keyValue indices to components and resize keyValues array
		do
		{

			//this sibling exists and has children, dive down one level
			if (currentNode->get_FirstChild())
			{

				//set the keyValue index
				currentNode->content.set_KeyValueIndex(keyValueIndex);

				//test whether a new key value branch is required
				if (currentNode->get_PriorSibling())
				{
				
					keyValueIndex++;
				
					if (keyValueIndex > keyValueMax)
						keyValueMax = keyValueIndex;
				
				}
					
				//dive down to next level
				currentParentNode = currentNode;
				currentNode = currentNode->get_FirstChild();

			}

			//this sibling exists but has no children (this should
			//only be true of simple components)
			else
			{

				//set the keyValue index
				currentNode->content.set_KeyValueIndex(keyValueIndex);
				
				//move laterally by one node
				currentNode = currentNode->get_NextSibling();


				//there are no more siblings at this level
				if (!currentNode)
				{

					do
					{

						//the proceedure is not at the top level, move up one level
						//and laterally by one node
						if (currentParentNode)
						{

							keyValueIndex = currentParentNode->content.get_KeyValueIndex();

							currentNode = currentParentNode->get_NextSibling();
							currentParentNode = currentParentNode->get_Parent();

						}

						//the proceedure is concluded
						else
						{

							concluded = true;
							break;
						}

					} while (!currentNode);

				}

			}

		} while (!concluded);

		keyValues.resize_NoCopy(keyValueMax + 1);

		//additional initial setup for initializing operations
		UniformList<Operator*> tempOperations;
		unsigned char typeCode;
		SimpleComponent *storageComponent;
		SimpleComponent *actionComponent(NULL); //initialized only to make compiler happy
		bool frozenActionValue(false);

		currentNode = components.get_FirstElder();
		currentParentNode = NULL;

		//initialize operations
		do
		{

			//there are no more siblings at this level
			if (!currentNode)
			{

				//the proceedure is not at the top level, (this should only be true
				//where currentParentNode is a populated complex component)
				if (currentParentNode)
				{
					
					//set actionComponent to parentNode's last child's keyValue if actionComponent
					//is not already frozen (last child should never be NULL)
					if(!frozenActionValue)
						actionComponent = keyValues.get_ElementAddress(
							currentParentNode->get_LastChild()->content.get_KeyValueIndex());

					//set storageComponent to parentNode's keyValue
					storageComponent = keyValues.get_ElementAddress(
						currentParentNode->content.get_KeyValueIndex());

					typeCode = currentParentNode->content.get_OperationType();

					//test for negative
					if (currentParentNode->content.get_Type() == 4)
						tempOperations.add_NewNode(&(negatives.add_NewNode(
							Negative(actionComponent))->content));

					//currentParentNode is a first child so assignment
					//can be skipped so long as the the next non-assignment operation's
					//actionComponent refers to the last child's storageComponent; //freeze
					//actionComponent for later use by the next mandatory operation;
					if (typeCode == 0)
						frozenActionValue = true;

					else
					{

						frozenActionValue = false;

						//addition
						if (typeCode == 1)
							tempOperations.add_NewNode(&(additions.add_NewNode(
								Add(storageComponent, actionComponent))->content));

						//subtraction
						else if (typeCode == 2)
							tempOperations.add_NewNode(&(subtractions.add_NewNode(
								Subtract(storageComponent, actionComponent))->content));

						//multiplication
						else if (typeCode == 3)
							tempOperations.add_NewNode(&(multiplications.add_NewNode(
								Multiply(storageComponent, actionComponent))->content));

						//division
						else if (typeCode == 4)
							tempOperations.add_NewNode(&(divisions.add_NewNode(
								Divide(storageComponent, actionComponent))->content));

						//exponent
						else if (typeCode == 5)
							tempOperations.add_NewNode(&(exponents.add_NewNode(
								IntegerExponent(storageComponent, actionComponent))->content));

						//root
						else //(typeCode == 6)
							tempOperations.add_NewNode(&(roots.add_NewNode(
								IntegerRoot(storageComponent, actionComponent))->content));

					}

					//move up one level and laterally by one node
					currentNode = currentParentNode->get_NextSibling();
					currentParentNode = currentParentNode->get_Parent();

				}

				//the proceedure is concluded; copy operations and return
				else
				{

					//add a final assignment of the froze actionComponent to keyValues[0]
					//to set up the expression for evaluate()
					if (frozenActionValue)
						tempOperations.add_NewNode(&(assignments.add_NewNode(
							Assign(keyValues.get_ElementAddress(0), actionComponent))->content));

					operations = tempOperations;
					return;

				}
					
			}

			//this sibling exists and has children, dive down one level
			else if (currentNode->get_FirstChild())
			{

				//dive down to next level
				currentParentNode = currentNode;
				currentNode = currentNode->get_FirstChild();

			}

			//this sibling exists but has no children (this should only be true
			//of simple components); create an operation corresponding to this node
			//and move laterally by one node
			else
			{

				frozenActionValue = false;

				storageComponent = keyValues.get_ElementAddress(currentNode->content.get_KeyValueIndex());

				//this should work for either constants or variables
				actionComponent = currentNode->content.get_AssociatedSimpleComponent();

				typeCode = currentNode->content.get_OperationType();

				//assignment
				if (typeCode == 0)
					tempOperations.add_NewNode(&(assignments.add_NewNode(
						Assign(storageComponent, actionComponent))->content));

				//addition
				else if (typeCode == 1)
					tempOperations.add_NewNode(&(additions.add_NewNode(
						Add(storageComponent, actionComponent))->content));

				//subtraction
				else if (typeCode == 2)
					tempOperations.add_NewNode(&(subtractions.add_NewNode(
						Subtract(storageComponent, actionComponent))->content));

				//multiplication
				else if (typeCode == 3)
					tempOperations.add_NewNode(&(multiplications.add_NewNode(
						Multiply(storageComponent, actionComponent))->content));

				//division
				else if (typeCode == 4)
					tempOperations.add_NewNode(&(divisions.add_NewNode(
						Divide(storageComponent, actionComponent))->content));

				//exponent
				else if (typeCode == 5)
					tempOperations.add_NewNode(&(exponents.add_NewNode(
						IntegerExponent(storageComponent, actionComponent))->content));

				//root
				else //(typeCode == 6)
					tempOperations.add_NewNode(&(roots.add_NewNode(
						IntegerRoot(storageComponent, actionComponent))->content));

				//move laterally by one node
				currentNode = currentNode->get_NextSibling();

			}

		} while (true);

	}

	//returns the calculated value of the expression
	double evaluate()
	{

		unsigned char operationsLength(operations.get_ArrayLength());
		unsigned char count(0);

		do
		{

			operations.get_Element(count)->calculate();

			count++;

		} while (count < operationsLength);

		return keyValues.get_ElementAddress(0)->get_Value();

	}

	//assumes writtenForm will not be modified
	Word &get_WrittenForm()
	{

		return writtenForm;

	}

};

#endif // MATHTOOLS_H_INCLUDED
