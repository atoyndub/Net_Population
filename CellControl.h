#ifndef CELLCONTROL_H_INCLUDED
#define CELLCONTROL_H_INCLUDED

//contains the cell properties which remain consistent across each Net for cells in the same index position

class CellControl
{

private:

	//cell mutation control properties
	//double internalCenter; //starting value of internalCoeff if a new net is being designed rather than loaded from file (> 0.0)
	//double broadcastCenter; //starting value of broadcastCoeff if a new net is being designed rather than loaded from file (any value)
	double linksCoeffCenter; //starting value of all this cell's postLinks linkCoeffs (> 0.0)
	//double decayRateCenter; //starting value of this cell's decayRate ( >= 0.0 (no stimulus loss per round), <= 1.0 (total stimulus loss per round)
	//unsigned char refractoryPeriodCenter; //starting value of this cell's refractoryPeriod (number of rounds till cell can be reactivated (>= 1) 

	double internalSpread; //controls rate of internalCoeff spread by mutation (> 0.0, value of 1.0 means an unamplified mutation changes internalCoeff by up to +-0.5)
	double broadcastSpread; //controls rate of broadcastCoeff spread by mutation (> 0.0, value of 1.0 means an unamplified mutation changes broadcastCoeff by up to +-0.5)
	double linksCoeffSpread; //controls rate of links coefficients spreads (> 0.0, value of 1.0 means an unamplified mutation changes a linksCoeff by up to +-0.5)
	double decayRateSpread; //controls rate of decayRate spread (> 0.0, value of 1.0 means an unamplified mutation changes a decayRate by up to +-0.05)
	unsigned char refractoryPeriodSpread; //sets the maximum refractoryPeriod change per mutation (>= 1) 
	
	double broadcastSwitchSignFreq; //controls frequency at which broadcastCoeff switches signs (between 0 *never switches* and 1 *always switches*)
	double linksSwitchSignFreq; //controls frequency at which linkCoeffs switch signs (between 0 *never switches* and 1 *always switches*)

public:

	//default constructor - broadly safe in this and higher-level contexts
	CellControl() : linksCoeffCenter(1.0), internalSpread(1.0),
		broadcastSpread(1.0), linksCoeffSpread(1.0), decayRateSpread(1.0),
		refractoryPeriodSpread(2), broadcastSwitchSignFreq(0.01), linksSwitchSignFreq(0.01) {}

	//modified constructor; assumes all parameters are in valid ranges
	CellControl(const double aLinksCoeffCenter, const double aInternalSpread,
		const double aBroadcastSpread, const double aLinksCoeffSpread,
		const double aDecayRateSpread, const double aRefractoryPeriodSpread,
		const double aBroadcastSwitchSignFreq, const double aLinksSwitchSignFreq)
		: linksCoeffCenter(aLinksCoeffCenter), internalSpread(aInternalSpread),
		broadcastSpread(aBroadcastSpread), linksCoeffSpread(aLinksCoeffSpread),
		decayRateSpread(aDecayRateSpread),
		refractoryPeriodSpread(aRefractoryPeriodSpread),
		broadcastSwitchSignFreq(aBroadcastSwitchSignFreq),
		linksSwitchSignFreq(aLinksSwitchSignFreq) {}

	//assumes that outputFile is already open
	void output_ToOpenFile_XML(ofstream &outputFile)
	{

		outputFile << "\n\t\t\t<groupLength>1</groupLength>";

		outputFile << "\n\t\t\t<linksCoeffCenter>";
		outputDouble_ToOpenFile(linksCoeffCenter, outputFile, 15);
		outputFile << "</linksCoeffCenter>";
		
		outputFile << "\n\t\t\t<internalSpread>";
		outputDouble_ToOpenFile(internalSpread, outputFile, 15);
		outputFile << "</internalSpread>";

		outputFile << "\n\t\t\t<broadcastSpread>";
		outputDouble_ToOpenFile(broadcastSpread, outputFile, 15);
		outputFile << "</broadcastSpread>";

		outputFile << "\n\t\t\t<linksCoeffSpread>";
		outputDouble_ToOpenFile(linksCoeffSpread, outputFile, 15);
		outputFile << "</linksCoeffSpread>";

		outputFile << "\n\t\t\t<decayRateSpread>";
		outputDouble_ToOpenFile(decayRateSpread, outputFile, 15);
		outputFile << "</decayRateSpread>";

		outputFile << "\n\t\t\t<refractoryPeriodSpread>" << static_cast<unsigned short>(refractoryPeriodSpread) << "</refractoryPeriodSpread>";

		outputFile << "\n\t\t\t<broadcastSwitchSignFreq>";
		outputDouble_ToOpenFile(broadcastSwitchSignFreq, outputFile, 15);
		outputFile << "</broadcastSwitchSignFreq>";

		outputFile << "\n\t\t\t<linksSwitchSignFreq>";
		outputDouble_ToOpenFile(linksSwitchSignFreq, outputFile, 15);
		outputFile << "</linksSwitchSignFreq>";

	}

	//assumes that outputFile is already open
	void output_ToOpenFile_JSON(ofstream &outputFile)
	{

		outputFile << "\"linksCoeffCenter\":";
		outputDouble_ToOpenFile(linksCoeffCenter, outputFile, 15);
		outputFile << ",";

		outputFile << "\"internalSpread\":";
		outputDouble_ToOpenFile(internalSpread, outputFile, 15);
		outputFile << ",";

		outputFile << "\"broadcastSpread\":";
		outputDouble_ToOpenFile(broadcastSpread, outputFile, 15);
		outputFile << ",";

		outputFile << "\"linksCoeffSpread\":";
		outputDouble_ToOpenFile(linksCoeffSpread, outputFile, 15);
		outputFile << ",";

		outputFile << "\"decayRateSpread\":";
		outputDouble_ToOpenFile(decayRateSpread, outputFile, 15);
		outputFile << ",";

		outputFile << "\"refractoryPeriodSpread\":" << static_cast<unsigned short>(refractoryPeriodSpread) << ",";

		outputFile << "\"broadcastSwitchSignFreq\":";
		outputDouble_ToOpenFile(broadcastSwitchSignFreq, outputFile, 15);
		outputFile << ",";

		outputFile << "\"linksSwitchSignFreq\":";
		outputDouble_ToOpenFile(linksSwitchSignFreq, outputFile, 15);

	}

	friend class Cell;

};

#endif // CELLCONTROL_H_INCLUDED