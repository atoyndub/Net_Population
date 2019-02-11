#ifndef STIMULUSTRACKER_H_INCLUDED
#define STIMULUSTRACKER_H_INCLUDED

//contains cell-associated properties used in stimulus tracking

class StimulusTracker
{

private:

	//updateable stimulus tracking properties
	double stimulus; //stores the unmodified stimulus a cell receives (positive, negative, or 0.0)
	unsigned short lastRoundStimulusChanged; //stores the count (starting from 1) of the stimulus round in which this cell last was stimulated or stimulus was decayed (0 if never stimulated)
	unsigned short roundRefractionComplete; //stores the count (starting from 1) of the stimulus round in which this cell will have passed it's refractory period after activation (0 if never activated)
	unsigned short activationCount; //tracks number of activations per stimulate() call

public:

	//default constructor (no other constructor needed)
	StimulusTracker() {}

	//default destructor
	~StimulusTracker() {}

	//resets all tracking variables to null state
	void reset()
	{

		stimulus = 0.0;
		lastRoundStimulusChanged = 0;
		roundRefractionComplete = 0;
		activationCount = 0;

	}

	//getter
	double get_Stimulus() const
	{

		return stimulus;

	}

	//getter
	unsigned short get_LastRoundStimulusChanged() const
	{

		return lastRoundStimulusChanged;

	}

	//getter
	unsigned short get_RoundRefractionComplete() const
	{

		return roundRefractionComplete;

	}

	//getter
	unsigned short get_ActivationCount() const
	{

		return activationCount;

	}

	//calculates the value of this tracker's stimulus (without any actual modification)
	//accounting for any stimulus decay based on decayRate and the number of stimulus
	//rounds passed since lastRoundStimulusChanged (if any); assumes decayRate
	//is that of the cell in index position corresponding to the index position
	//of this tracker and currentStimulusRound >= lastRoundStimulusChanged
	double calculate_DecayedStimulus(const double decayRate,
		const unsigned short currentStimulusRound) const
	{

		unsigned short roundCount(lastRoundStimulusChanged);
		double returnValue(stimulus);

		while (roundCount < currentStimulusRound)
		{

			returnValue *= decayRate;
			roundCount++;

		}

		return returnValue;

	}

	//modifies the value of this tracker's stimulus and lastRoundStimulusChanged
	//accounting for any stimulus decay based on decayRate and the number of stimulus
	//rounds passed since lastRoundStimulusChanged (if any); assumes decayRate
	//is that of the cell in index position corresponding to the index position
	//of this tracker and currentStimulusRound >= lastRoundStimulusChanged
	void decay_Stimulus(const double decayRate,
		const unsigned short currentStimulusRound)
	{

		while (lastRoundStimulusChanged < currentStimulusRound)
		{

			stimulus *= decayRate;
			lastRoundStimulusChanged++;

		}

	}

	//sets the value of this tracker's stimulus; assumes this
	//tracker corresponds to an input cell and all parameters
	//passed correspond to that the cell and the external input
	//stimulus it receives in the first stimulus round;
	//assumes lastRoundStimulusChanged will be updated in conjunction with this function
	void set_Stimulus_FirstRound(const double externalStimulus,
		const double inputCellInputDiffusalCoeff)
	{

		stimulus = externalStimulus * inputCellInputDiffusalCoeff;

	}

	//modifies the value of this tracker's stimulus; assumes this
	//tracker corresponds to a post cell of an activated pre cell
	//and all parameters passed correspond to that cell connection per their names
	//assumes lastRoundStimulusChanged will be updated in conjunction with this function
	void add_Stimulus_LaterRounds(const double preCellBroadcastCoeff,
		const double postCellInputDiffusalCoeff, const double linkCoeff)
	{

		stimulus += preCellBroadcastCoeff * postCellInputDiffusalCoeff * linkCoeff;

	}

	//called when a the cell associated with this tracker has been activated
	//sets stimulus to zero, increments activationCount, and sets
	//roundRefractionComplete to currentStimulusRound + refractoryPeriod
	void activate(const unsigned short currentStimulusRound,
		const unsigned char refractoryPeriod)
	{

		stimulus = 0.0;
		activationCount++;
		roundRefractionComplete = currentStimulusRound + refractoryPeriod;

	}

	//sets lastRoundStimulusChanged to currentStimulusRound
	void update_LastRoundStimulusChanged(const unsigned short currentStimulusRound)
	{

		lastRoundStimulusChanged = currentStimulusRound;

	}

	//sets roundRefractionComplete to currentStimulusRound + refractoryPeriod
	//void update_RoundRefractionComplete(const unsigned short currentStimulusRound,
		//const unsigned char refractoryPeriod)
	//{

		//roundRefractionComplete = currentStimulusRound + refractoryPeriod;

	//}

};

#endif // STIMULUSTRACKER_H_INCLUDED
