#define     SERIES_RESISTOR    470000  // Series resistor value in ohms.
#define     USE_FAHRENHEIT     true   // True to use Fahrenheit, false to
#define     ADC_SAMPLES        5      // Number of ADC samples to average
                                      // when taking a reading.

// Temperature unit conversion functions and state.
typedef float (*TempConversion)(float);
TempConversion ToKelvin; 
TempConversion FromKelvin;
char* TempUnit;

 // Steinhart-Hart coefficients.
float A = 0.006559355258;
float B = -0.000233063483;
float C = -0.000000000979;

//com
String sBuffer = "";
String usbInstructionDataString = "";
int usbCommandVal = 0;
boolean USBcommandExecuted = true;
String usbCommand = "";

//timing
int thermistor_read_delay = 200;
unsigned long last_thermistor_readtime = 0;

void setup() {            //This function gets called when the Arduino starts
    pinMode(8, OUTPUT);
    digitalWrite(8, HIGH); //off
    Serial.begin(57600);   //This code sets up the Serial port at 115200 baud rate

    if (USE_FAHRENHEIT) {
        ToKelvin = &fahrenheitToKelvin;
        FromKelvin = &kelvinToFahrenheit;
        TempUnit = "Fahrenheit";
    }
    else {
        ToKelvin = &celsiusToKelvin;
        FromKelvin = &kelvinToCelsius;
        TempUnit = "Celsius";
    }
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
        if (thermistor_read_delay < 50) {
            thermistor_read_delay = 50;
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



double readResistance(int thermistor_pin) {
    float reading = 0;
    for (int i = 0; i < ADC_SAMPLES; ++i) {
        reading += analogRead(thermistor_pin);
    }
    reading /= (float)ADC_SAMPLES;
    reading = (1023 / reading) - 1;
    return SERIES_RESISTOR / reading;
}

float kelvinToFahrenheit(float kelvin) {
    return kelvin*(9.0/5.0) - 459.67;
}

float fahrenheitToKelvin(float fahrenheit) {
    return (fahrenheit + 459.67)*(5.0/9.0);
}

float kelvinToCelsius(float kelvin) {
    return kelvin - 273.15;
}

float celsiusToKelvin(float celsius) {
    return celsius + 273.15; 
}

float readTemp(int thermistor_pin) {
    float R = readResistance(thermistor_pin);
    float kelvin = 1.0/(A + B*log(R) + C*pow(log(R), 3.0));
    return kelvin;
}

void readThermistors()
{
    if ( (millis() - thermistor_read_delay) > last_thermistor_readtime ) {
        float first_thermistor_temp = FromKelvin(readTemp(0));
        addtosbuffer("t0", String( first_thermistor_temp ));
        last_thermistor_readtime = millis();
    }
    
}

void loop() {             //This function loops while the arduino is powered
    serialListen();
    readThermistors();
    printsbuffer();
}