// Define the pin to which the water flow sensor is connected
const int sensorPin = 12; // Example pin number, change according to your setup

// Variables to store the flow rate and total volume
float flowRate;
float totalVolume = 0;
unsigned long prevMillis = 0;
const unsigned long interval = 1000; // Read interval in milliseconds

//pH
int pH = A0;

int buf[10];

float ph (float voltage) { 
  return 7 + ((2.5 - voltage) / 0.10);
  }


void setup() {

   pinMode(sensorPin, INPUT); // Set sensor pin as input
   pinMode (pH, INPUT);
   
   Serial.begin(9600);
   delay(1000);
}

void loop()
{
  //Water Flow
  unsigned long currentMillis = millis();
  
  // Read sensor data every 'interval' milliseconds
  if (currentMillis - prevMillis >= interval) {
    // Calculate flow rate
    float sensorValue = pulseIn(sensorPin, HIGH); // Read the pulse width
    float frequency = 1000.0 / (sensorValue / interval); // Frequency in Hz
    flowRate = frequency / 7.5; // Convert frequency to flow rate in L/min
    
   
    // Print flow rate and total volume
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
    
    
    // Reset timer
    prevMillis = currentMillis;
  }

  //pH
  for(int i=0;i<10;i++) {
    buf[i]=analogRead(pH); 
    delay(10);
  }

  float avgValue=0;
  for(int i=0;i<10;i++)
  avgValue+=buf[i];
  
  float pHVol=(float)avgValue*5.0/1024/10; 
  float phValue = -5.70*pHVol+21.34;

  
  Serial.print("pH cal ");
  Serial.println(phValue);
  delay(1000);

}
