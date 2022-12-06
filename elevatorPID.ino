#include <Arduino.h>
#include <PID_v1.h>
// #include <Ultrasonic.h>
#include <HCSR04.h>

#define pino_trigger 2
#define pino_echo 4
#define motorPin3 3 // Pin  7 of L293
#define motorPin4 6 // Pin  2 of L293

const int bterreo = 9;
const int bandar1 = 10;
const int bandar2 = 11;
const int bandar3 = 12;

int vSpeed = 200;

int terreo = 0;
int andar1 = 0;
int andar2 = 0;
int andar3 = 0;
int andarAnt = 5;

// Define Variables we'll be connecting to
double Setpoint, Input, Output, aux, teste = 0;
const int inputPin = 0, outputPin = 13; //

// Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 40, 5.16906, 9.47875, DIRECT);

// Inicializa o sensor nos pinos definidos acima
UltraSonicDistanceSensor ultrasonic(pino_trigger, pino_echo);
unsigned long serialTime; // this will help us know when to talk with processing

void setup()
{
  // initialize the serial link with processing
  Serial.begin(9600);

  // ultrasonic pins
  pinMode(pino_trigger, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(pino_echo, INPUT);     // Sets the echoPin as an INPUT

  // Motor pins control
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  // floor buttons pins
  pinMode(bterreo, INPUT_PULLUP);
  pinMode(bandar1, INPUT_PULLUP);
  pinMode(bandar2, INPUT_PULLUP);
  pinMode(bandar3, INPUT_PULLUP);

  // initialize the variables we're linked to
  Input = analogRead(inputPin);

  Input = ultrasonic.measureDistanceCm();
  Input *= 20;

  Setpoint = 600;

  aux = 0;

  Serial.println(Input);

  // turn the PID on
  // myPID.SetMode(AUTOMATIC);
}

void loop()
{

  Input = Get_position();

  // pid-related code
  // Input = analogRead(inputPin);
  // myPID.Compute();
  analogWrite(outputPin, Output);

  // Serial.print("INPUT ");
  // Serial.println(Input);

  // send-receive with processing if it's time
  if (millis() > serialTime)
  {
    SerialReceive();
    SerialSend();
    serialTime += 300;
  }

  // Serial.println(terreo);
  // Serial.println(andar1);
  // // Serial.println(andar2);
  // Serial.println(andar3);

  terreo = digitalRead(bterreo);
  if (terreo == LOW)
  {
    Setpoint = 630;
    Serial.print("Botao T\n");
  }

  andar1 = digitalRead(bandar1);
  if (andar1 == LOW)
  {
    Setpoint = 420;
    Serial.print("Botao 1\n");
  }

  andar2 = digitalRead(bandar2);
  if (andar2 == LOW)
  {
    Setpoint = 220;
    Serial.print("Botao 2\n");
  }

  andar3 = digitalRead(bandar3);
  if (andar3 == LOW)
  {
    Setpoint = 43;
    Serial.print("Botao 3\n");
  }

  if (Setpoint == 630)
  { // 1st floor
    Dummy_Control(620, 640);
  }
  else if (Setpoint == 420)
  { // 2nd floor
    Dummy_Control(400, 440);
  }
  else if (Setpoint == 220)
  { // 3rd floor
    Dummy_Control(200, 240);
  }
  else if(Setpoint == 43)
  {  
    Dummy_Control(40, 45);
  }

}

double Get_position()
{
  Input = ultrasonic.measureDistanceCm();
  Input *= 20;
  
  if (aux > 1000)
  {             // the first sensor output value is too high
    Input = 50; //
  }

  return Input;
}

void Dummy_Control(double min_val, double max_val)
{
  Output = 255;
  while ((Input < min_val) || (Input > max_val))
  {
    Input = Get_position();

    if (Input > max_val)
    {

      Serial.print("DOWN - max:");
      Serial.print(max_val);
      Serial.print(" min: ");
      Serial.print(min_val);
      Serial.print(" input: ");
      Serial.println(Input);
      digitalWrite(motorPin3, HIGH); // DESCE
      digitalWrite(motorPin4, LOW);
    }
    else if (Input < min_val)
    {
      Serial.print("UP - max:");
      Serial.print(max_val);
      Serial.print(" min: ");
      Serial.print(min_val);
      Serial.print(" input: ");
      Serial.println(Input);
      digitalWrite(motorPin3, LOW); // SOBE
      digitalWrite(motorPin4, HIGH);
    }
  }

  digitalWrite(motorPin3, LOW); // PARA
  digitalWrite(motorPin4, LOW);
}


union
{                   // This Data structure lets
  byte asBytes[24]; // us take the byte array
  float asFloat[6]; // sent from processing and
} 
foo; 
void SerialReceive()
{

  // read the bytes sent from Processing
  int index = 0;
  byte Auto_Man = -1;
  byte Direct_Reverse = -1;
  
  while (Serial.available() && index < 26)
  {
    if (index == 0)
      Auto_Man = Serial.read();
    else if (index == 1)
      Direct_Reverse = Serial.read();
    //else if(index == 2)
      //Direct_Reverse = Serial.read();
    else
      foo.asBytes[index - 2] = Serial.read();
    index++;
  }

  // if the information we got was in the correct format,
  // read it into the system
  if (index == 26 && (Auto_Man == 0 || Auto_Man == 1) && (Direct_Reverse == 0 || Direct_Reverse == 1))
  {
    teste = double(foo.asFloat[0]);
    Setpoint = double(foo.asFloat[0]);
    // Input=double(foo.asFloat[1]);       // * the user has the ability to send the
    //    value of "Input"  in most cases (as
    //    in this one) this is not needed.
    if (Auto_Man == 0)                 // * only change the output if we are in
    {                                  //   manual mode.  otherwise we'll get an
      Output = double(foo.asFloat[2]); //   output blip, then the controller will
    }                                  //   overwrite.

    double p, i, d;             // * read in and set the controller tunings
    p = double(foo.asFloat[3]); //
    i = double(foo.asFloat[4]); //
    d = double(foo.asFloat[5]); //
    myPID.SetTunings(p, i, d);  //

    if (Auto_Man == 0)
      myPID.SetMode(MANUAL); // * set the controller mode
    else
      myPID.SetMode(AUTOMATIC); //

    if (Direct_Reverse == 0)
      myPID.SetControllerDirection(DIRECT); // * set the controller Direction
    else
      myPID.SetControllerDirection(REVERSE); //
  }
  Serial.flush(); // * clear any random data from the serial buffer
}

void SerialSend()
{
  Serial.print("PID ");
  Serial.print(Setpoint);
  Serial.print(" ");
  Serial.print(Input);
  Serial.print(" ");
  Serial.print(Output);
  Serial.print(" ");
  Serial.print(myPID.GetKp());
  Serial.print(" ");
  Serial.print(myPID.GetKi());
  Serial.print(" ");
  Serial.print(myPID.GetKd());
  Serial.print(" ");
  if (myPID.GetMode() == AUTOMATIC)
    Serial.print("Automatic");
  else
    Serial.print("Manual");
  Serial.print(" ");
  if (myPID.GetDirection() == DIRECT)
    Serial.println("Direct");
  else
    Serial.println("Reverse");
}
