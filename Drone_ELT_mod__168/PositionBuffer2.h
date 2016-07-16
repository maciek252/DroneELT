/*
 * PositionBuffer.hpp
 *
 *  Created on: Jan 14, 2016
 *      Author: maciek
 */

#ifndef DRONE_ELT_MOD_POSITIONBUFFER_2_H_
#define DRONE_ELT_MOD_POSITIONBUFFER_2_H_

#include "LinkedList.h"


#if 1
class Position2 {
public:
	Position2() {
		resetPosition();
	}

	void resetPosition() {
		latitude = 0.0;
		longitude = 0.0;
		hdop = 0.0;
		numOfSats = 0.0;
		triggerTime = 0.0;
		fired = false;
		numOfSamples = 0;
	}



	Position2(double latitude, double longitude, int numOfSats, double hdop);
	Position2(double latitude, double longitude, int numOfSats){

		// unfortunately the syntax: : Position(lat, long) does not compile...
		 this->latitude = latitude;
		 this->longitude = longitude;
		 this->numOfSats = numOfSats;

	}
	Position2(double latitude, double longitude) {
		this->latitude = latitude;
		this->longitude = longitude;
	}

	void update(Position2 position);

	void addPositionToAverage(Position2 &pos);
	void computeAverage();

	bool fired;

	bool isValid(){
		if(numOfSats < 3)
			return false;
		if(latitude == 0.0 && longitude == 0.0)
			return false;
		return true;
	}

	double getLatitude() {
		return latitude;
	}
	double getLongitude() {
		return longitude;
	}

	void writeStatToSerial();

public:

	double latitude, longitude;
	int numOfSats, numOfSamples;
	double hdop;
	float triggerTime;
};
#endif

///////////////////////////////////////////////////////////////////////////////////

#if 1
class PositionBuffer {
public:
	PositionBuffer();

	void addGPSPositionToOneSecondBuffers(Position2 position);
	void updateCurrentSecondPosition(Position2 position);

	Position2 getPreviousPosition(int index);

	// calculates average from (previousBegin, previousEnd) range - indices of samples added to the buffer before the last one
	// previousBegin > previousEnd, takes (previousBegin-previousEnd+1) samples
	// example: buffer of size 20, added 24 samples, (7,3) will take samples: 17, 18, 19, 20, 21
	Position2 averageFromRange(int previousBegin, int previousEnd);

	Position2 averageFromCurrentSecPositionsAndCleanBuffer();

	void addToHistory(Position2);

	bool stopAlarmCriterionMet();


	Position2 testWawer5();
	void testWawer4();
	double testDistanceWawerWodynska1();

	void tick();

	//Position2 currentSecondPosition;
	Position2 lastValidPosition;


	bool startCriterionMetFlag;
	bool alarmCriterionMetFlag;
	bool addPositionFarAwayFlag;

private:

	bool startCriterionMet();
	bool alarmCriterionMet();
	void addPositionFarAway();

	//LinkedList<Position2> positionsForTesting;
	LinkedList<Position2> positionsInCurrentSecond;
	LinkedList<Position2> historyPositions;

	static const int TICK_INTERVAL = 3000;
	// free mem for local vars  ~574
	static const int NUM_OF_SAMPLES_IN_BUFFER = 5; // this is OK, works well
	//static const int NUM_OF_SAMPLES_IN_BUFFER = 3; // for 3 and mavlink - free mem = 338 const OK
	//static const int NUM_OF_SAMPLES_IN_BUFFER = 4; // for 4 and mavlink - free mem = 313 const OK
	static const int NUM_OF_SAMPLES_IN_CURRENT_SEC_BUFFER = 5; // OK
	//static const int NUM_OF_SAMPLES_IN_CURRENT_SEC_BUFFER = 4; // works, but maybe 5 better?
	//static const int BUFFERSIZE = 10;
	//Position2 positions[BUFFERSIZE];


	void addTestSamplesIfTriggeredByTime();
	void printStatsToSerialBefore();
	void printStatsToSerialAfter();

	bool startCriterionSatisfied();
	bool alarmCriterionSatisfied();

	Position2 giveLastValidAvgPos();
	Position2 giveLastValidSample();

	void addPositionToBeTriggered(Position2 *array, int index, float time);

	int nextForward(int index, int offset);
	int nextBackward(int index, int offset);

	int nextForward(int index);
	int nextBackward(int index);

	void oneSecondTick();
	void fifthSecondTick();
	int writePos;
	int currentPosIndex;


	long oneSecondTimer;
	long fifthSecondTimer;

};
#endif


#endif /* DRONE_ELT_MOD_POSITIONBUFFER_2_H_ */
