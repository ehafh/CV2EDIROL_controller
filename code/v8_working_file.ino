#include <SoftwareSerial.h>
SoftwareSerial Serial1(4, 3); // RX, TX

#define SELPIN 12 //Selection Pin 
#define DATAOUT 11//MOSI 
#define DATAIN  10//MISO 
#define SPICLOCK  9//Clock 
#define butt1  7// 
#define butt2  6// 


int readvalue;
int ledC = 13;
int potVal[16];
int OLDpotVal[16];

//address 0 contains nothing!
int midiCCselect[16] = {0,3,4,5,7,8,9,10,12,11,1,2,17,0,18,19}; // select the midi Controller Number for each input 


void setup(){ 

 //  Set MIDI baud rate:
 Serial1.begin(31250); // MIDI OUT
 //set pin modes 
 pinMode(SELPIN, OUTPUT); 
 pinMode(DATAOUT, OUTPUT); 
 pinMode(DATAIN, INPUT); 
 pinMode(SPICLOCK, OUTPUT); 

 pinMode(ledC, OUTPUT);

 pinMode(butt1, INPUT);  
 pinMode(butt2, INPUT);  
 
 //disable device to start with 
 digitalWrite(SELPIN,HIGH); 
 digitalWrite(DATAOUT,LOW); 
 digitalWrite(SPICLOCK,LOW); 

 Serial.begin(9600); 
 delay(10);
 Serial.println("V4/V8 Test Program");
 delay(10);

 //read and update knobs so that we dont burst send all paramiters at starup
  readAllPots();
  mapPots();
  readButts();
 copyA(potVal, OLDpotVal, 16);
} 

void loop() { 
  readAllPots();
  mapPots();
  readButts();
  checkForValChange();

  
  for (int i = 0; i <= 15; i++) {
  Serial.print(OLDpotVal[i]);
  Serial.print(" ");
  }
  Serial.println(" ");
  //delay(600);
  
  copyA(potVal, OLDpotVal, 16); //copy new vals into old vals
} 

void copyA(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}

void readAllPots() {

for (int i = 1; i <= 8; i++) {
  potVal[i] = read_adc(i); 
}
  potVal[9] = analogRead(A0); 
  potVal[10] = analogRead(A1); 
  potVal[11] = analogRead(A2); 
  potVal[12] = analogRead(A3); 
  potVal[13] = analogRead(A4); 

}

void readButts() {
  potVal[14] = digitalRead(butt1)*127;
  potVal[15] = digitalRead(butt2)*127;

  
}

void mapPots() {

  for (int i = 0; i <= 8; i++) {
  potVal[i] = map(potVal[i], 0, 4096, 0, 127 ); 
}

  for (int i = 9; i <= 13; i++) {
  potVal[i] = map(potVal[i], 0, 1024, 0, 127 ); 
}

}

void checkForValChange(){
  for (int i = 0; i <= 15; i++) {
  if((OLDpotVal[i] != potVal[i])){//if the new value is different from the old value 
    midiCC(0xB0, midiCCselect[i], potVal[i]);
    //Serial.print("OLD VAL ");
    //Serial.print(OLDpotVal[i]);
    //Serial.print(" ");
    //Serial.print("NEW VAL ");
    //Serial.println(potVal[i]);
    
    }
  }
  
}

int read_adc(int channel){
  int adcvalue = 0;
  byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

  //allow channel selection
  commandbits|=((channel-1)<<3);

  digitalWrite(SELPIN,LOW); //Select adc
  // setup bits to be written
  for (int i=7; i>=3; i--){
    digitalWrite(DATAOUT,commandbits&1<<i);
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);    
  }

  digitalWrite(SPICLOCK,HIGH);    //ignores 2 null bits
  digitalWrite(SPICLOCK,LOW);
  digitalWrite(SPICLOCK,HIGH);  
  digitalWrite(SPICLOCK,LOW);

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue+=digitalRead(DATAIN)<<i;
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);
  }
  digitalWrite(SELPIN, HIGH); //turn off device
  return adcvalue;
}

void midiCC(int CC_data, int c_num, int c_val){
  digitalWrite(ledC, !digitalRead(ledC));
  Serial1.write(CC_data);
  Serial1.write(c_num);
  Serial1.write(c_val);
  //Serial.println("SEND MIDI");
  digitalWrite(ledC, !digitalRead(ledC));
}


