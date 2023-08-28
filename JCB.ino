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
int fx_ribbon_4 = A16;
int fx_ribbon_5 = A18;
int fx_ribbon_6 = A17;

int pot_1 = A12;
int pot_2 = A13;
int pot_3 = A14;
int pot_4 = A15;

int latch_1 = 1;
int latch_2 = 2;
int latch_3 = 3;

int latches[3];

//arrays for holding low pass filtered osc ribbon sensors (500mm)
int filtered_osc_ribbons_vals[3];
int firstOrderOscVals[3];
int secondOrderOscVals[3];
int thirdOrderOscVals[3];
int fourthOrderOscVals[3];
int fifthOrderOscVals[3];

//arrays for holding low pass filtered fx ribbon sensors (200mm)
int filtered_fx_ribbons_vals[6];
int firstOrderFXVals[6];
int secondOrderFXVals[6];
int thirdOrderFXVals[6];
int fourthOrderFXVals[6];
int fifthOrderFXVals[6];

//arrays for holding low pass filtered pot (knob) sensors
int firstOrderKnobVals[4];
int secondOrderKnobVals[4];
int thirdOrderKnobVals[4];
int fourthOrderKnobVals[4];
int fifthOrderKnobVals[4];

//variables for transfering finalized MIDI vals (per pass)
int tempKnobVal = 0;
int tempRibbonVal = 0;
int tempOscVal = 0;

//holds FSR value, latched and unlatched
int latched_Val_FSR[3] = {0, 0, 0};

//holds previous FSR value
int old_Latched_1_Val_FSR = 0;
int old_Latched_2_Val_FSR = 0;
int old_Latched_3_Val_FSR = 0;

//flags for latching logic
bool latched_1_Flag = false;
bool latched_2_Flag = false;
bool latched_3_Flag = false;

//analog inputs for grabbing sensor values (pots and fsrs)
int potSensors[16] = {osc_ribbon_1, osc_ribbon_2, osc_ribbon_3, fsr_ribbon_1, fsr_ribbon_2, fsr_ribbon_3,
                      fx_ribbon_1, fx_ribbon_2, fx_ribbon_3, fx_ribbon_4, fx_ribbon_5, fx_ribbon_6, pot_1,
                      pot_2, pot_3, pot_4};

//measursed values to set an appropriate MIDI range
int potSensorsMin[16] = {155, 149, 157, 1, 1, 1, 212, 210, 216, 200, 214, 207, 1, 1, 1, 1};
int potSensorsMax[16] = {285, 286, 283, 925, 925, 925, 403, 403, 404, 410, 401, 406, 1021, 1021, 1021, 1021};

//initial MIDI mapping from analog and digital reads
int processedPotSensors[16];

//MIDI channel/note selection
int midiChannel = 1;
int midiNotes[3] = {26, 29, 33};

//variables for sending note on/off logic
bool noteOnFlag = false;
bool activeRibbon = false;
int fsrTracker = 0;

//array for detecting changes in previous MIDI values; logic for sending CC messages
int oldProcessedPotSensor[4] = {0, 0, 0, 0};
int oldProcessedFXSensors[6] = {0, 0, 0, 0, 0, 0};
int oldProcessedRibbonSensors[3] = {0, 0, 0};

//counter for sending MIDI messages
int sendMIDIMessages = 0;

void setup() 
{
  Serial.begin(9600);

  pinMode(latch_1, INPUT);
  pinMode(latch_2, INPUT);
  pinMode(latch_3, INPUT);
}

void loop()
{
  cleanIncomingPotVals();
  mapIncomingPotVals();

  latchVals();
  cleanFSRLatchVals();
  maintainRibbonVals();
  
  filterRibbons();
  filterKnobs();

  if (sendMIDIMessages == 8)
  {
    sendMIDIVals();
    sendMIDIMessages = 0;    
  }

  sendMIDIMessages++;

  //monitorSensorValues();
}

/**********************************
 * Handles MIDI sends
 **********************************/
void sendMIDIVals()
{ 
  for (int i = 0; i < 3; i++)
  {
    if (latched_Val_FSR[i] > 5)
    {
      activeRibbon = true;
      break;
    }
    else
    {
      activeRibbon = false;
    }
  }
  
  //send note Ons/Offs
  for (int i = 0; i < 3; i++)
  {
    if (processedPotSensors[i+3] > 5 && noteOnFlag == false)
    {
      usbMIDI.sendNoteOn(midiNotes[i], 127, midiChannel);
      noteOnFlag = true;

      fsrTracker = i;
    }

    if (processedPotSensors[fsrTracker + 3] == 0 && noteOnFlag == true && latches[fsrTracker] == false && activeRibbon == false)
    {
      usbMIDI.sendNoteOff(midiNotes[fsrTracker], 0, midiChannel);
      noteOnFlag = false;
    }
  }

  //send FSRs: CCs 99, 100, 101
  if (latched_Val_FSR[0] != old_Latched_1_Val_FSR)
  {
    usbMIDI.sendControlChange(99, latched_Val_FSR[0], midiChannel);
    old_Latched_1_Val_FSR = latched_Val_FSR[0];
  }

  if (latched_Val_FSR[1] != old_Latched_2_Val_FSR)
  {
    usbMIDI.sendControlChange(100, latched_Val_FSR[1], midiChannel);
    old_Latched_2_Val_FSR = latched_Val_FSR[1];
  }

  if (latched_Val_FSR[2] != old_Latched_3_Val_FSR)
  {
    usbMIDI.sendControlChange(101, latched_Val_FSR[2], midiChannel);
    old_Latched_3_Val_FSR = latched_Val_FSR[2];
  }
  
  //send Softpots: CCs 102, 103, 104
  for (int i = 0; i < 3; i++)
  {
    if (filtered_osc_ribbons_vals[i] != oldProcessedRibbonSensors[i])
    {
      usbMIDI.sendControlChange(i + 102, filtered_osc_ribbons_vals[i], midiChannel);
    }

    oldProcessedRibbonSensors[i] = filtered_osc_ribbons_vals[i];
  }

  //send FX Ribbons: CCs 105, 106, 107, 108, 109, 110
  for (int i = 0; i < 6; i++)
  {
    if (filtered_fx_ribbons_vals[i] != oldProcessedFXSensors[i])
    {
      usbMIDI.sendControlChange(i + 105, filtered_fx_ribbons_vals[i], midiChannel);
    }

    oldProcessedFXSensors[i] = filtered_fx_ribbons_vals[i];
  }

  //send Pots: CCs 111, 112, 113, 114
  for (int i = 0; i < 4; i++)
  {
    if (processedPotSensors[i + 12] != oldProcessedPotSensor[i])
    {
      usbMIDI.sendControlChange(i + 111, processedPotSensors[i + 12], midiChannel);
    }

    oldProcessedPotSensor[i] = processedPotSensors[i + 12];
  }
}

/*****************************************************
 * OSC ribbon processing, LP
 *****************************************************/
void filterRibbons()
{
  for (int i = 0; i < 3; i++)
  {    
    if (processedPotSensors[i] != 0)
    {
      tempOscVal = (processedPotSensors[i] + firstOrderOscVals[i] + secondOrderOscVals[i] +
                  thirdOrderOscVals[i] + fourthOrderOscVals[i] + fifthOrderOscVals[i]) / 6;
                     
      fifthOrderOscVals[i] = fourthOrderOscVals[i];
      fourthOrderOscVals[i] = thirdOrderOscVals[i];
      thirdOrderOscVals[i] = secondOrderOscVals[i];
      secondOrderOscVals[i] = firstOrderOscVals[i];
      firstOrderOscVals[i] = tempOscVal;

      filtered_osc_ribbons_vals[i] = map(tempOscVal, 0, 122, 0, 127);    
    }
  }
}


/**********************************
 * FX ribbon processing, LP
 **********************************/
void maintainRibbonVals()
{
  for (int i = 6; i < 12; i++)
  {
    if (processedPotSensors[i] != 0)
    {
      int j = i - 6;
      
      tempRibbonVal = (processedPotSensors[i] + firstOrderFXVals[j] + 
        secondOrderFXVals[j] + thirdOrderFXVals[j] + fourthOrderFXVals[j] + fifthOrderFXVals[j])/6;

      fifthOrderFXVals[j] = fourthOrderFXVals[j];
      fourthOrderFXVals[j] = thirdOrderFXVals[j];
      thirdOrderFXVals[j] = secondOrderFXVals[j];
      secondOrderFXVals[j] = firstOrderFXVals[j];
      firstOrderFXVals[j] = tempRibbonVal;

      filtered_fx_ribbons_vals[j] = map(tempRibbonVal, 1, 122, 0, 127);
    }
  }
}

/*****************************************************
 * Knob processing, LP
 *****************************************************/
void filterKnobs()
{
  for (int i = 0; i < 4; i++)
  {    
    tempKnobVal = (processedPotSensors[i + 12] + firstOrderKnobVals[i] + secondOrderKnobVals[i] +
                  thirdOrderKnobVals[i] + fourthOrderKnobVals[i] + fifthOrderKnobVals[i]) / 6;
                     
    fifthOrderKnobVals[i] = fourthOrderKnobVals[i];
    fourthOrderKnobVals[i] = thirdOrderKnobVals[i];
    thirdOrderKnobVals[i] = secondOrderKnobVals[i];
    secondOrderKnobVals[i] = firstOrderKnobVals[i];
    firstOrderKnobVals[i] = tempKnobVal;

    processedPotSensors[i + 12] = map(tempKnobVal, 0, 122, 0, 127);
  }
}

/**********************************
 * Function handles latching of FSR
 * values upon utilizing switches
 **********************************/
void latchVals()
{
  latches[0] = digitalRead(latch_1);
  latches[1] = digitalRead(latch_2);
  latches[2] = digitalRead(latch_3);

  if (latches[0] == 1 && latched_1_Flag == false)
  { 
    latched_Val_FSR[0] = processedPotSensors[3];
    latched_1_Flag = true;
  }

  if (latches[0] == 0)
  {
    latched_Val_FSR[0] = processedPotSensors[3];
    latched_1_Flag = false;
  }

  if (latches[1] == 1 && latched_2_Flag == false)
  { 
    latched_Val_FSR[1] = processedPotSensors[4]; 
    latched_2_Flag = true;
  }

  if (latches[1] == 0)
  {
    latched_Val_FSR[1] = processedPotSensors[4];
    latched_2_Flag = false;
  }

  if (latches[2] == 1 && latched_3_Flag == false)
  { 
    latched_Val_FSR[2] = processedPotSensors[5];
    latched_3_Flag = true;
  }

  if (latches[2] == 0)
  {
    latched_Val_FSR[2] = processedPotSensors[5];
    latched_3_Flag = false;
  }
}

/*****************************************************
 * Handle low noise, eliminate low values
 *****************************************************/
void cleanFSRLatchVals()
{
  if (latched_Val_FSR[0]< 4)
  {
    latched_Val_FSR[0] = 0;
  }
  
  if (latched_Val_FSR[1] < 4)
  {
    latched_Val_FSR[1] = 0;
  }
  
  if (latched_Val_FSR[2] < 4)
  {
    latched_Val_FSR[2] = 0;
  }
}

/************************************
   Function handles mapping of received
   sensor values to MIDI range
 ************************************/
void mapIncomingPotVals()
{
  for (int i = 0; i < 16; i++)
  {
    //map values to MIDI range
    processedPotSensors[i] = map(analogRead(potSensors[i]), potSensorsMin[i], potSensorsMax[i], 0, 127);

    //Reverse value direction (0 to 127) in accordance to physical interaction
    if (i == 0 || i == 1 || i == 2 || i == 6 || i == 7 || i == 8 || i == 9 || i == 10 || i == 11 || i == 12)
    {
      processedPotSensors[i] = invertRibbonVal(processedPotSensors[i]);

      //handle values outside of MIDI range
      if (processedPotSensors[i] < 0)
      {
        processedPotSensors[i] = 0;
      }

      if (processedPotSensors[i] > 127 && processedPotSensors[i] < 130)
      {
        processedPotSensors[i] = 127;
      }

      if (processedPotSensors[i] > 130)
      {
        processedPotSensors[i] = 0;
      }
    }

    if (processedPotSensors[i] > 127)
    {
      processedPotSensors[i] = 127;
    }
  }
}

/****************************************
  Function handles received noise values 
 ****************************************/
void cleanIncomingPotVals()
{
  for (int i = 0; i < 16; i++)
  {
    if (analogRead(potSensors[i]) < 10)
    {
      processedPotSensors[i] = 0;
    }
  }
}

/******************************************
  Function inverts recieved values 
  in accordance to MIDI range
 ******************************************/
int invertRibbonVal(int val)
{
  val = (val - 127) * -1;

  return val;
}

/******************************************
   Function handles monitoring of received
   sensor values
 ******************************************/
void monitorSensorValues()
{
  Serial.print("OSC Ribbon 1: ");
  Serial.println(processedPotSensors[0]);
  Serial.print("FSR 1: ");
  Serial.println(latched_Val_FSR[0]);

  Serial.print("OSC Ribbon 2: ");
  Serial.println(processedPotSensors[1]);
  Serial.print("FSR 2: ");
  Serial.println(latched_Val_FSR[1]);

  Serial.print("OSC Ribbon 3: ");
  Serial.println(processedPotSensors[3]);
  Serial.print("FSR 3: ");
  Serial.println(latched_Val_FSR[2]);

  Serial.print("FX Ribbon 1: ");
  Serial.println(filtered_fx_ribbons_vals[0]);

  Serial.print("FX Ribbon 2: ");
  Serial.println(filtered_fx_ribbons_vals[1]);

  Serial.print("FX Ribbon 3: ");
  Serial.println(filtered_fx_ribbons_vals[2]);

  Serial.print("FX Ribbon 4: ");
  Serial.println(filtered_fx_ribbons_vals[3]);

  Serial.print("FX Ribbon 5: ");
  Serial.println(filtered_fx_ribbons_vals[4]);

  Serial.print("FX Ribbon 6: ");
  Serial.println(filtered_fx_ribbons_vals[5]);

  Serial.print("Pot 1: ");
  Serial.println(processedPotSensors[12]);

  Serial.print("Pot 2: ");
  Serial.println(processedPotSensors[13]);

  Serial.print("Pot 3: ");
  Serial.println(processedPotSensors[14]);

  Serial.print("Pot 4: ");
  Serial.println(processedPotSensors[15]);

  Serial.print("Latch 1: ");
  Serial.println(digitalRead(latch_1));

  Serial.print("Latch 2: ");
  Serial.println(digitalRead(latch_2));

  Serial.print("Latch 3: ");
  Serial.println(digitalRead(latch_3));
}
