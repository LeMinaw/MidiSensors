/*------------------------- GENERAL INFORMATION ------------------------*
 * MidiSensors v1.0                                                     *
 * Released 08/08/16 by LeMinaw                                         *
 * Arduino Button Library v1.0 is requiered                             * 
 *------------------------------- LICENSE ------------------------------*
 * This work is licensed under the Creative Commons Attribution         *
 * NonCommercial 4.0 International License. To view a copy of this      *
 * license, visit http://creativecommons.org/licenses/by-nc/4.0/.       *
 *----------------------------------------------------------------------*/
 
/*--------------------------------------------MIDI PROTOCOL REMINDER--------------------------------*
 *               MESSAGE                 |            DATA1              |         DATA2            *
 * 1000 cccc = note off => 128(10)       | 0xxx xxxx: note pitch         | 0xxx xxxx: velocity      *
 * 1001 cccc = note on => 129(10)        | 0xxx xxxx: note pitch         | 0xxx xxxx: velocity     *
 * 1110 cccc = pitch bend => 224(10)     | 0000 0000: code               | 0xxx xxxx: speed         *
 * 1011 cccc = control change => 176(10) | 0xxx xxxx: number             | 0xxx xxxx: value         *
 *--------------------------------------------------------------------------------------------------*/

#include "Button.h"

// PINS CONFIGURATION
const int sensPins[4]   = {0, 1, 2, 3};
const int ledActPins[4] = {22, 23, 24, 25};
const int ledSelPins[4] = {8, 9, 10, 11};
const int butSelPin     = 5;
const int butMinPin     = 6;
const int butMaxPin     = 7;
// CC CONFIGURATION
const int sensCCs[4] = {75, 76, 77, 78};
// END CONFIGURATION

int selectedSens = -1;
int sensValues[4] = {0, 0, 0, 0};
int sensCalibs[8] = {0, 1023, 0, 1023, 0, 1023, 0, 1023};
int newValue;0

Button butSel(butSelPin, true, true, 10); // Pullup, inversion and 20ms debounce enabled
Button butMin(butMinPin, true, true, 10);
Button butMax(butMaxPin, true, true, 10);

void setup() {
    // Serial.begin(57600); // For serial logs
    Serial1.begin(31250);
    for (int i; i < 4; i++) pinMode(sensPins[i],   INPUT );
    for (int i; i < 2; i++) pinMode(ledSelPins[i], OUTPUT);
    for (int i; i < 4; i++) pinMode(ledActPins[i], OUTPUT);
    selectSens();
}

void loop() {
    butSel.read();
    butMin.read();
    butMax.read();
    
    // Serial.println(analogRead(sensPins[1])); // For serial logs
    
    if(butSel.wasReleased()) selectSens();
    if(butMin.wasReleased()) minCalib(selectedSens);
    if(butMax.wasReleased()) maxCalib(selectedSens);
    
    for (int i; i < 4; i++) {
        newValue = map(analogRead(sensPins[i]), sensCalibs[i*2], sensCalibs[i*2+1], 0, 127);
        if (newValue != sensValues[i]) {
            midiTx(176, sensCCs[i], newValue);
            sensValues[i] = newValue;
            digitalWrite(ledActPins[i], HIGH);
            delay(2);
            digitalWrite(ledActPins[i], LOW);
        }
    }
    // delay(1); // Can improve stability on some cards
}

void selectSens() {
    selectedSens++;
    if (selectedSens > 3) selectedSens = 0;
    for (int i; i < 4; i++) digitalWrite(ledSelPins[i], LOW);
    digitalWrite(ledSelPins[selectedSens], HIGH);
}

void minCalib(int sens) {
    sensCalibs[sens*2] = analogRead(sensPins[sens]);
}
void maxCalib(int sens) {
    sensCalibs[sens*2+1] = analogRead(sensPins[sens]);
}

void midiTx(unsigned char msg, unsigned char data1, unsigned char data2) {
    //Serial.write(msg);   // For serial logs
    //Serial.write(data1); // For serial logs
    //Serial.write(data2); // For serial logs
    Serial1.write(msg);
    Serial1.write(data1);
    Serial1.write(data2);
}

//EOF