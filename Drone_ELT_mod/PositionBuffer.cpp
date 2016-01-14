/*
 * PositionBuffer.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: maciek
 */

#include "PositionBuffer.hpp"


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

void PositionBuffer::addPosition(Position *array, int index, double time){

}

void PositionBuffer::addPosition(Position *array, int index){
	addGPSPosition(array[index]);
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
	addPosition(positionValidPKPWawer, 0);
	addPosition(positionValidPKPWawer, 1);
	addPosition(positionValidPKPWawer, 2);
	addPosition(positionValidPKPWawer, 3);
	addPosition(positionValidPKPWawer, 4);
	addPosition(positionValidPKPWawer, 5);

	return averageFromRange(2,1);
}



