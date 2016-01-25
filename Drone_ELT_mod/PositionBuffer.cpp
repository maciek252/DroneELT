/*
 * PositionBuffer.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: maciek
 */

#include "PositionBuffer.hpp"
#include "TinyGPS.h"

PositionBuffer::PositionBuffer(){
	oneSecondTimer = millis();
	writePos = 0;
	resetAvgPosition();
}





int PositionBuffer::nextForward(int arg){
	if(arg == BUFFERSIZE)
		return 0;
	return arg + 1;
}

int PositionBuffer::nextBackward(int arg){
	if(arg == 0)
		return BUFFERSIZE;
	return arg - 1;
}

int PositionBuffer::nextForward(int arg, int offset){
	int o = arg;
	for(int i = 0; i < offset; i++)
		o = nextForward(o);
	return o;
}

int PositionBuffer::nextBackward(int arg, int offset){
	int o = arg;
	for(int i = 0; i < offset; i++)
		o = nextBackward(o);
	return o;
}

void  PositionBuffer::addGPSPosition(Position position){
	positions[writePos] = position;
	writePos = nextForward(writePos);
}



Position PositionBuffer::averageFromRange(int previousBegin, int previousEnd){
	double latitudeAvg = 0.0;
	double longitudeAvg = 0.0;

	int readPos = nextBackward(writePos, previousEnd);
	for(int i = 0; i < (previousBegin - previousEnd + 1); i++){
		latitudeAvg += positions[readPos].getLatitude();
		longitudeAvg += positions[readPos].getLongitude();
		readPos = nextBackward(readPos);
	}

	latitudeAvg = latitudeAvg / (double)(previousBegin - previousEnd + 1);
	longitudeAvg = longitudeAvg / (double)(previousBegin - previousEnd + 1);
	Position result(latitudeAvg, longitudeAvg);
	return result;

}



Position PositionBuffer::testWawer5(){
	addPositionToBeTriggered(positionValidPKPWawer, 0, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 1, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 2, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 3, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 4, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 5, 1);

	return averageFromRange(2,1);
}

double PositionBuffer::testDistanceWawerWodynska1(){
	Position wawer = positionValidPKPWawer[0];
	Position wodynska = positionValidWodynska[0];
	return TinyGPS::distance_between(wawer.getLatitude(), wawer.getLongitude(), wodynska.getLatitude(), wodynska.getLongitude());
}

void PositionBuffer::tick(){

	if(millis() - oneSecondTimer > 1000){
		oneSecondTimer = millis();
		oneSecondTick();
	}
}

void PositionBuffer::oneSecondTick(){

	resetAvgPosition();
	int i = millis() % 10;
	if(positionsForTesting.size() > 10)
		positionsForTesting.pop();
	Position nowa;
	nowa.fired = false;
	nowa.numOfSats = i;
	positionsForTesting.add(0, nowa);
}

void PositionBuffer::resetAvgPosition(){
	avgLatForSecond = 0.0;
	avgLongForSecond = 0.0;
	numOfSamplesInSecond = 0;
		avgNumOfSatsForSecond = 0;
		avgHdopForSecond = 0;
}

void PositionBuffer::addPositionToBeTriggered(Position *array, int index, double time){

}

