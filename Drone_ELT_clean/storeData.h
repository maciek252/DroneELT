#ifndef _STORE_DATA_H_
#define _STORE_DATA_H_

class DataRecord{
 public:
  float latitude;
  float longitude;
  int  eph;
  int numOfSats;
  
  DataRecord(){
     eph = -1; 
  }
  
  bool isValid(){
     if(eph == -1)
       return false;
     return true;  
  }
  
};

static const int numOfRecordsStored = 5;
static int index = 0;
static DataRecord recordsStored[numOfRecordsStored];



static void addRecord(DataRecord r){
  if(index == numOfRecordsStored -1)
    index = 0;
  else
    index++;
    
  recordsStored[index] = r;  
    
}

static DataRecord getPreviousRecord(){
 
   DataRecord result = recordsStored[index];
   index--;
   if(index < 0)
     index = numOfRecordsStored -1;
   return result;  
}

#endif

