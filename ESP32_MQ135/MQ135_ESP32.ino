#include <MQUnifiedsensor.h>

#define MQ135PIN 34
#define R0_PRECISION 100
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  

//Declare Sensor
MQUnifiedsensor MQ135("ESP32", 5, 12, MQ135PIN, "MQ-135");

void setup() {
  Serial.begin(9600);
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.init();

  /*
    //If the RL value is different from 10K please assign your RL value with the following method:
    MQ135.setRL(10);
  */

  /*****************************  MQ CAlibration ********************************************/
  // Explanation:
  // In this routine the sensor will measure the resistance of the sensor supposedly before being pre-heated
  // and on clean air (Calibration conditions), setting up R0 value.
  // We recomend executing this routine only on setup in laboratory conditions.
  // This routine does not need to be executed on each restart, you can load your R0 value from eeprom.

  Serial.print("Calibrating please wait ...");
  float calcR0 = 0;
  for (int i = 1; i <= R0_PRECISION; i ++)
  {
    MQ135.update(); // Update data, read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / R0_PRECISION);
  Serial.println("  done!");

  if (isinf(calcR0)) {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1);
  }
  if (calcR0 == 0) {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1);
  }
  
  /*****************************  MQ CAlibration ********************************************/
  Serial.println("** Values from MQ-135 ****");
  Serial.println("|    CO   |  Alcohol |   CO2  |  Toluen  |  NH4  |  Aceton  |");
}

void loop() {
  MQ135.update(); // Update data, read the voltage from the analog pin

  MQ135.setA(605.18); MQ135.setB(-3.937); // Configure the equation to calculate CO concentration value
  float CO = MQ135.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

  MQ135.setA(77.255); MQ135.setB(-3.18); //Configure the equation to calculate Alcohol concentration value
  float Alcohol = MQ135.readSensor(); // SSensor will read PPM concentration using the model, a and b values set previously or from the setup

  MQ135.setA(110.47); MQ135.setB(-2.862); // Configure the equation to calculate CO2 concentration value
  float CO2 = MQ135.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

  MQ135.setA(44.947); MQ135.setB(-3.445); // Configure the equation to calculate Toluen concentration value
  float Toluen = MQ135.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

  MQ135.setA(102.2 ); MQ135.setB(-2.473); // Configure the equation to calculate NH4 concentration value
  float NH4 = MQ135.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

  MQ135.setA(34.668); MQ135.setB(-3.369); // Configure the equation to calculate Aceton concentration value
  float Aceton = MQ135.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  Serial.print("|   "); Serial.print(CO);
  Serial.print("   |   "); Serial.print(Alcohol);
  
  Serial.print("   |   "); Serial.print(CO2 + 400); // 400ppm offset due to current pollution
  Serial.print("   |   "); Serial.print(Toluen);
  Serial.print("   |   "); Serial.print(NH4);
  Serial.print("   |   "); Serial.print(Aceton);
  Serial.println("   |");
  
  /*
    Exponential regression:
    GAS      | a      | b
    CO       | 605.18 | -3.937
    Alcohol  | 77.255 | -3.18
    CO2      | 110.47 | -2.862
    Toluen  | 44.947 | -3.445
    NH4      | 102.2  | -2.473
    Aceton  | 34.668 | -3.369
  */

  delay(500); //Sampling frequency
}
