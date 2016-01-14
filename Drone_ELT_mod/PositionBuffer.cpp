/*
 * PositionBuffer.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: maciek
 */

#include "PositionBuffer.hpp"


int PositionBuffer::nextForward(){
	if(writePos == BUFFERSIZE)
		return 0;
	return writePos + 1;
}

int PositionBuffer::nextBackward(){
	if(writePos == 0)
		return BUFFERSIZE;
	return writePos - 1;
}

int PositionBuffer::nextForward(int offset){
	int o = writePos;
	for(int i = 0; i < offset; i++)
		o = nextForward();
	return o;
}

int PositionBuffer::nextBackward(int offset){
	int o = writePos;
	for(int i = 0; i < offset; i++)
		o = nextBackward();
	return o;
}

void  PositionBuffer::addGPSPosition(Position position){

}

Position PositionBuffer::testWawer5(){

}



