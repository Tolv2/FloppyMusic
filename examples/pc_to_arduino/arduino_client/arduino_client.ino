// Recieve on and off commands over serial
// Message length: 1 byte
// Message structure:
// 0bSPPPPPPP; S = status high/low (1 - high, 0 - low); P = pin number
#define MESSAGE_STATUS(x) ((x & 0b10000000) >> 7)
#define MESSAGE_PIN_NUMBER(x) ((x) & 0b01111111)

#define NUMBER_OF_FLOPPIES 2

#define SERIAL_BAUDRATE 115200

const int usedPins[] = {3, 4, 5, 6, 7, 8};
int message;

void setup(){
    Serial.begin(SERIAL_BAUDRATE);
    for(int i=0; i<(3 * NUMBER_OF_FLOPPIES); i++){
        pinMode(usedPins[i], OUTPUT);
        digitalWrite(usedPins[i], LOW);
    }
}

void loop(){
    if(Serial.available() > 0){
        message = Serial.read();
        digitalWrite(MESSAGE_PIN_NUMBER(message), MESSAGE_STATUS(message) ? HIGH : LOW ); // The second argument may look a bit silly, but certain boards might not all have the same bindings, so it's probably safest to use the Arduino macros.
    }
}
