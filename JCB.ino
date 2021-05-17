 //Global Sensor Inputs
int osc_ribbon_1 = A0;
int osc_ribbon_2 = A2;
int osc_ribbon_3 = A4;

int fsr_ribbon_1 = A1;
int fsr_ribbon_2 = A3;
int fsr_ribbon_3 = A5;

int fx_ribbon_1 = A6;
int fx_ribbon_2 = A7;
int fx_ribbon_3 = A8;
int fx_ribbon_4 = A9;
int fx_ribbon_5 = A10;
int fx_ribbon_6 = A11;

int pot_1 = A12;
int pot_2 = A13;
int pot_3 = A14;
int pot_4 = A15;

int latch_1 = 31;
int latch_2 = 33;
int latch_3 = 35;

void setup() {
  Serial.begin(9600);

  pinMode(latch_1, INPUT);
  pinMode(latch_2, INPUT);
  pinMode(latch_3, INPUT);
}

void loop()
{
  //monitorSensorValues();
  writeSensorValues();
}

/******************************************
 * Function handles monitoring of received 
 * sensor values
 ******************************************/
void monitorSensorValues()
{
  Serial.print("OSC Ribbon 1: ");
  Serial.println(analogRead(osc_ribbon_1));
  Serial.print("FSR 1: "); 
  Serial.println(analogRead(fsr_ribbon_1));

  Serial.print("OSC Ribbon 2: ");
  Serial.println(analogRead(osc_ribbon_2));
  Serial.print("FSR 2: "); 
  Serial.println(analogRead(fsr_ribbon_2));

  Serial.print("OSC Ribbon 3: ");
  Serial.println(analogRead(osc_ribbon_3));
  Serial.print("FSR 3: "); 
  Serial.println(analogRead(fsr_ribbon_3));

  Serial.print("FX Ribbon 1: ");
  Serial.println(analogRead(fx_ribbon_1));
  
  Serial.print("FX Ribbon 2: ");
  Serial.println(analogRead(fx_ribbon_2));

  Serial.print("FX Ribbon 3: ");
  Serial.println(analogRead(fx_ribbon_3));

  Serial.print("FX Ribbon 4: ");
  Serial.println(analogRead(fx_ribbon_4));

  Serial.print("FX Ribbon 5: ");
  Serial.println(analogRead(fx_ribbon_5));

  Serial.print("FX Ribbon 6: ");
  Serial.println(analogRead(fx_ribbon_6));

  Serial.print("Pot 1: ");
  Serial.println(analogRead(pot_1));
  
  Serial.print("Pot 2: ");
  Serial.println(analogRead(pot_2));
  
  Serial.print("Pot 3: ");
  Serial.println(analogRead(pot_3));
  
  Serial.print("Pot 4: ");
  Serial.println(analogRead(pot_4));

  Serial.print("Latch 1: ");
  Serial.println(digitalRead(latch_1));
  
  Serial.print("Latch 2: ");
  Serial.println(digitalRead(latch_2));
  
  Serial.print("Latch 3: ");
  Serial.println(digitalRead(latch_3));
  
  delay(2000);
}

void writeSensorValues()
{
  Serial.print(analogRead(osc_ribbon_1));
  Serial.print(" "); 
  Serial.print(analogRead(fsr_ribbon_1));
  Serial.print(" "); 

  Serial.print(analogRead(osc_ribbon_2));
  Serial.print(" "); 
  Serial.print(analogRead(fsr_ribbon_2));
  Serial.print(" "); 

  Serial.print(analogRead(osc_ribbon_3));
  Serial.print(" "); 
  Serial.print(analogRead(fsr_ribbon_3));
  Serial.print(" "); 

  Serial.print(analogRead(fx_ribbon_1));
  Serial.print(" ");   
  Serial.print(analogRead(fx_ribbon_2));
  Serial.print(" "); 
  Serial.print(analogRead(fx_ribbon_3));
  Serial.print(" "); 

  Serial.print(analogRead(fx_ribbon_4));
  Serial.print(" "); 
  Serial.print(analogRead(fx_ribbon_5));
  Serial.print(" "); 
  Serial.print(analogRead(fx_ribbon_6));
  Serial.print(" "); 
 
  Serial.print(analogRead(pot_1));
  Serial.print(" "); 
  Serial.print(analogRead(pot_2));
  Serial.print(" "); 
  Serial.print(analogRead(pot_3));
  Serial.print(" "); 
  Serial.print(analogRead(pot_4));
  Serial.print(" "); 
 
  Serial.print(digitalRead(latch_1));
  Serial.print(" "); 
  Serial.print(digitalRead(latch_2));
  Serial.print(" "); 
  Serial.println(digitalRead(latch_3));
}
