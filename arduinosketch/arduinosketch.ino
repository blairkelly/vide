#include <math.h>         //loads the more advanced math functions
 
//com
String sBuffer = "";
String usbInstructionDataString = "";
int usbCommandVal = 0;
boolean USBcommandExecuted = true;
String usbCommand = "";

//timing
int thermistor_read_delay = 500;
unsigned long last_thermistor_readtime = 0;

void setup() {            //This function gets called when the Arduino starts
    pinMode(8, OUTPUT);
    digitalWrite(8, HIGH); //off
    Serial.begin(57600);   //This code sets up the Serial port at 115200 baud rate
}
 
double Thermister(int RawADC) {  //Function to perform the fancy math of the Steinhart-Hart equation
    double Temp;
    Temp = log(((10240000/RawADC) - 10000));
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
    Temp = Temp - 273.15;              // Convert Kelvin to Celsius
    //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
    Temp = (Temp * 9.0)/ 5.0 - 5; // Celsius to Fahrenheit - comment out this line if you need Celsius
    return Temp;
}

/*functions*/
void printsbuffer () {
    //print sBuffer
    if(sBuffer != "") {
        Serial.println(sBuffer);
        sBuffer = "";
    }
}
void addtosbuffer (String param, String value) {
    if(sBuffer == "") {
        sBuffer = "t=" + (String)millis() + "&" + param + "=" + value;
    } else {
        sBuffer = sBuffer + "&" + param + "=" + value;
    }
}

void delegate(String cmd, int cmdval) {
    if (cmd.equals("p")) {
        if (cmdval == 0) {
            digitalWrite(8, HIGH); //off
            addtosbuffer("powerswitchtail", "0");
        } else if (cmdval == 1) {
            digitalWrite(8, LOW); //ON
            addtosbuffer("powerswitchtail", "1");
        }
    }

    if (cmd.equals("d")) {
        thermistor_read_delay = cmdval;
        if (thermistor_read_delay < 80) {
            thermistor_read_delay = 80;
        } else if (thermistor_read_delay > 5000) {
            thermistor_read_delay = 5000;
        }

        addtosbuffer("trd", String(thermistor_read_delay));
    }
}

void serialListen()
{
    char arduinoSerialData; //FOR CONVERTING BYTE TO CHAR. here is stored information coming from the arduino.
    String currentChar = "";
    if(Serial.available() > 0) {
        arduinoSerialData = char(Serial.read());   //BYTE TO CHAR.
        currentChar = (String)arduinoSerialData; //incoming data equated to c.
        if(!currentChar.equals("1") && !currentChar.equals("2") && !currentChar.equals("3") && !currentChar.equals("4") && !currentChar.equals("5") && !currentChar.equals("6") && !currentChar.equals("7") && !currentChar.equals("8") && !currentChar.equals("9") && !currentChar.equals("0") && !currentChar.equals(".")) { 
            //the character is not a number, not a value to go along with a command,
            //so it is probably a command.
            if(!usbInstructionDataString.equals("")) {
                //usbCommandVal = Integer.parseInt(usbInstructionDataString);
                char charBuf[30];
                usbInstructionDataString.toCharArray(charBuf, 30);
                usbCommandVal = atoi(charBuf);

            }
            if((USBcommandExecuted == false) && (arduinoSerialData == 13)) {
            
                delegate(usbCommand, usbCommandVal);
                USBcommandExecuted = true;
            
            }
            if((arduinoSerialData != 13) && (arduinoSerialData != 10)) {
                usbCommand = currentChar;
            }
            usbInstructionDataString = "";
        } else {
            //in this case, we're probably receiving a command value.
            //store it
            usbInstructionDataString = usbInstructionDataString + currentChar;
            USBcommandExecuted = false;
        }
    }
}

void readThermistors()
{
    if ( (millis() - thermistor_read_delay) > last_thermistor_readtime ) {
        addtosbuffer("t0", String(analogRead(0)));
        addtosbuffer("t1", String(analogRead(1)));
        last_thermistor_readtime = millis();
    }
    
}

void loop() {             //This function loops while the arduino is powered
    serialListen();
    readThermistors();
    printsbuffer();
}