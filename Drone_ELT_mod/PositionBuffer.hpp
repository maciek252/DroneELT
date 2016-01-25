/*
 * PositionBuffer.hpp
 *
 *  Created on: Jan 14, 2016
 *      Author: maciek
 */

#ifndef DRONE_ELT_MOD_POSITIONBUFFER_HPP_
#define DRONE_ELT_MOD_POSITIONBUFFER_HPP_

#include "LinkedList.h"

class Position {
public:
	Position(){
		latitude = 0.0;
		longitude = 0.0;
		hdop = 0.0;
		numOfSats = 0.0;
		fired = false;
	};
	Position(double latitude, double longitude, int numOfSats, double hdop);
	Position(double latitude, double longitude) {
		this->latitude = latitude;
		this->longitude = longitude;
	}


	bool fired;

	double getLatitude() {
		return latitude;
	}
	double getLongitude() {
		return longitude;
	}
public:

	double latitude, longitude;
	int numOfSats;
	double hdop;
};

static Position positionValidWodynska[1] = { Position(52.207850, 21.185813) };

static Position positionValidZyczynska[5] = { Position(52.208367, 21.185661),
		Position(52.208370, 21.185648), Position(52.208360, 21.185664),
		Position(52.208371, 21.185662), Position(52.208360, 21.185648) };

/*
 static Position positionValidXXX[5] = {
 Position(),
 Position(),
 Position(),
 Position(),
 Position()};
 */

static Position positionValidPKPWawer[6] = { Position(52.223407, 21.149524),
		Position(52.223397, 21.149543), Position(52.223406, 21.149556),
		Position(52.223390, 21.149541), Position(52.223421, 21.149552),
		Position(52.223413, 21.149540) };

static Position positionValidOstrobramska[7] = { Position(52.234116, 21.126134),
		Position(52.234104, 21.126128), Position(52.234110, 21.126135),
		Position(52.234098, 21.126119), Position(52.234110, 21.126116),
		Position(52.234103, 21.126154), Position(52.234117, 21.126123) };

static Position positionValidSiedlce[1] = { Position(52.165221, 22.282902) };

static Position positionValidMoscow[1] = { Position(55.757202, 37.621489) };

static Position positionValidCracow[1] = { Position(50.061073, 19.944103) };

/*
 * W-wa Miedzylesie Wodynska 52.207850, 21.185813
 * W-wa Miedzylesie Zyczynska:
 *
 *
 *
 *
 *
 * W-wa PKP Wawer
 * W-wa Ostrobramska
 * Siedlce:
 *
 * Moscow:
 *
 * Cracow:
 *
 *
 */

class PositionBuffer {
public:
	PositionBuffer();

	void addGPSPosition(Position position);

	Position getPreviousPosition(int index);

	// calculates average from (previousBegin, previousEnd) range - indices of samples added to the buffer before the last one
	// previousBegin > previousEnd, takes (previousBegin-previousEnd+1) samples
	// example: buffer of size 20, added 24 samples, (7,3) will take samples: 17, 18, 19, 20, 21
	Position averageFromRange(int previousBegin, int previousEnd);

	bool startCriterionMet();
	bool alarmCriterionMet();
	bool stopAlarmCriterionMet();

	Position testWawer5();
	double testDistanceWawerWodynska1();

	void tick();

private:

	LinkedList<Position> positionsForTesting;

	double avgLatForSecond;
	double avgLongForSecond;
	long numOfSamplesInSecond;
	double avgNumOfSatsForSecond;
	double avgHdopForSecond;


	static const int BUFFERSIZE = 20;
	Position positions[BUFFERSIZE];


	void resetAvgPosition();

	void addPositionToBeTriggered(Position *array, int index, double time);

	int nextForward(int index, int offset);
	int nextBackward(int index, int offset);

	int nextForward(int index);
	int nextBackward(int index);

	void oneSecondTick();
	int writePos;

	long oneSecondTimer;

};

#endif /* DRONE_ELT_MOD_POSITIONBUFFER_HPP_ */
