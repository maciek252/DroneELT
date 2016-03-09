/*
 * PositionBuffer.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: maciek
 */

#include "PositionBuffer.h"
#include <FastSerial.h>
#include <Arduino.h>
//#include "TinyGPS2.h"
//#include "Utils.h"
#include "MemoryFree.h"

void Position::update(Position position) {
	if(true){
	//if (position.numOfSats >= 3) {
		latitude += position.latitude;
		longitude += position.longitude;
		numOfSats += position.numOfSats;
		hdop += position.hdop;
		numOfSamples++;
	}
}

void Position::writeStatToSerial() {
	//printDouble( 1.2925254, 5);
	Serial.print("lat=");
	//printDouble(latitude, 5);
	Serial.print("long=");
	//printDouble(longitude, 5);
	Serial.print("ns=");
	Serial.print(numOfSats, DEC);
	//Serial.write(numOfSats);
	Serial.print("nSmpl=");
	Serial.println(numOfSamples, DEC);
	//Serial.write(numOfSamples);

	//Serial.write('p');
	//Serial.write(numOfSats);
	//Serial.write('\n');
	//Serial.flush();
}

void Position::computeAverage() {
	latitude = latitude / numOfSamples;
	longitude = longitude / numOfSamples;
}

void Position::addPositionToAverage(Position &pos) {

}

PositionBuffer::PositionBuffer() {
	oneSecondTimer = millis();
	writePos = 0;
}

int PositionBuffer::nextForward(int arg) {
	if (arg == BUFFERSIZE)
		return 0;
	return arg + 1;
}

int PositionBuffer::nextBackward(int arg) {
	if (arg == 0)
		return BUFFERSIZE;
	return arg - 1;
}

int PositionBuffer::nextForward(int arg, int offset) {
	int o = arg;
	for (int i = 0; i < offset; i++)
		o = nextForward(o);
	return o;
}

int PositionBuffer::nextBackward(int arg, int offset) {
	int o = arg;
	for (int i = 0; i < offset; i++)
		o = nextBackward(o);
	return o;
}

void PositionBuffer::addGPSPositionToOneSecondBuffers(Position position) {

	if (positionsOneSecondIntervals.size() > NUM_OF_SAMPLES_IN_BUFFER)
		positionsOneSecondIntervals.pop();
	positionsOneSecondIntervals.add(0, position);
	//positions[writePos] = position;
	//writePos = nextForward(writePos);
}

void PositionBuffer::updateCurrentSecondPosition(Position position) {
	currentSecondPosition.update(position);
}

Position PositionBuffer::averageFromRange(int previousBegin, int previousEnd) {
	double latitudeAvg = 0.0;
	double longitudeAvg = 0.0;

	int readPos = nextBackward(writePos, previousEnd);
	for (int i = 0; i < (previousBegin - previousEnd + 1); i++) {
		latitudeAvg += positions[readPos].getLatitude();
		longitudeAvg += positions[readPos].getLongitude();
		readPos = nextBackward(readPos);
	}

	latitudeAvg = latitudeAvg / (double) (previousBegin - previousEnd + 1);
	longitudeAvg = longitudeAvg / (double) (previousBegin - previousEnd + 1);
	Position result(latitudeAvg, longitudeAvg);
	return result;

}

Position PositionBuffer::testWawer5() {
	addPositionToBeTriggered(positionValidPKPWawer, 0, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 1, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 2, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 3, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 4, 1);
	addPositionToBeTriggered(positionValidPKPWawer, 5, 1);

	return averageFromRange(2, 1);
}

void PositionBuffer::testWawer4() {
	addPositionToBeTriggered(positionValidPKPWawer, 0, 3);
	addPositionToBeTriggered(positionValidPKPWawer, 1, 6);

}

#if 0
double PositionBuffer::testDistanceWawerWodynska1() {
	Position wawer = positionValidPKPWawer[0];
	Position wodynska = positionValidWodynska[0];
	return TinyGPS::distance_between(wawer.getLatitude(), wawer.getLongitude(), wodynska.getLatitude(), wodynska.getLongitude());
}
#endif

void PositionBuffer::tick() {

	if (millis() - oneSecondTimer > 1000) {
		oneSecondTimer = millis();
		//Serial.write('t');
		oneSecondTick();
	}
	if (millis() - fifthSecondTimer > 1000) {
		fifthSecondTimer = millis();
		//Serial.write('t');
		fifthSecondTick();
	}

}

void PositionBuffer::fifthSecondTick() {

	int i = 0;
	while (true) {
		Position p = positionsForTesting.get(i);
		if (!p.fired && p.triggerTime * 1000.0 > millis()) {
			p.fired = true;
			Serial.println("adding test position");
			//printDouble(p.latitude, 5);
			//addGPSPositionToOneSecondBuffers(p);
			updateCurrentSecondPosition(p);
			positionsForTesting.remove(i);
			i = 0;
			//positionsForTesting.set(i, p);
		}
		i++;
		if (i == positionsForTesting.size() - 1)
			break;
	}

	fifthSecondTimer = 0;
}

void PositionBuffer::oneSecondTick() {

	//addPositionToBeTriggered(positionValidPKPWawer, 0, 3);

	int i = millis() % 10;

//	Position nowa;
//	nowa.fired = false;
//	nowa.numOfSats = i;
	//positionsForTesting.add(0, nowa);
	addGPSPositionToOneSecondBuffers(currentSecondPosition);
	currentSecondPosition.resetPosition();

	printStatsToSerial();
}

void PositionBuffer::printStatsToSerial() {

	//Serial.write("----free memory =" + freeMemory2());
	Serial.println("1 second tick");
	//Serial.write('1');
	for (int i = 0; i < positionsOneSecondIntervals.size(); i++) {
		positionsOneSecondIntervals.get(i).writeStatToSerial();
	}
}

void PositionBuffer::addPositionToBeTriggered(Position *array, int index,
		float time) {

	//Position p(21.1, 21.2);

	Position &p = array[index];
	//printDouble(p.latitude, 5);
	p.triggerTime = time;
	positionsForTesting.add(p);
}

void addTestSamplesIfTriggeredByTime() {

}
