#define echoPin  4
#define trigPin 2
#include <Ultrasonic.h>

long duration;
int distance;
int andarAtual;

const int motorB1 = 5;
const int motorB2 = 3;


const int bterreo = 9;
const int bandar1 = 10;
const int bandar2 = 11;
const int bandar3 = 12;

int terreo = 0; 
int andar1 = 0;
int andar2 = 0;
int andar3 = 0;

int vSpeed = 200; 

byte botaoandar = 4;
byte andar();

unsigned long delayTempo;

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(echoPin, trigPin);

void setup() {

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);
  
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  
  pinMode(bterreo, INPUT_PULLUP);
  pinMode(bandar1, INPUT_PULLUP);
  pinMode(bandar2, INPUT_PULLUP);
  pinMode(bandar3, INPUT_PULLUP);

  Serial.begin(9600);
}
byte andar() {
  //Le as informacoes do sensor, em cm e pol
  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  //inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);
  //Exibe informacoes no serial monitor
  //Serial.print("Distancia em cm: ");
  //Serial.print(cmMsec);
  delay(1000);
  return checarAndar(cmMsec);
  
}
int checarAndar(float distancia){
  //terreo
  if(distancia >= 30 && distancia <= 40){
    Serial.print("0\n");
    andarAtual = 0;
  }
  else if(distancia < 30 && distancia >= 20){
    Serial.print("1\n");
    andarAtual = 1;
  }
  else if(distancia < 20 && distancia >= 10){
    Serial.print("2\n");
    andarAtual = 2;
  }
  else if(distancia < 10 || distancia > 40){
    Serial.print("3\n");
    andarAtual = 3;
  }
  return andarAtual;
  }

// Inicia o motor com o comando descer
void desce() {
  analogWrite(motorB1, vSpeed);
  analogWrite(motorB2, 0);
}

// Inicia o motor com o comando subir
void sobe() {
  analogWrite(motorB1, 0);
  analogWrite(motorB2, vSpeed);
}

// Inicia o motor com o comando parar
void parado() {
  analogWrite(motorB1, 0);
  analogWrite(motorB2, 0);
}

void loop() {

  if (botaoandar == 4) {
    terreo = digitalRead(bterreo);
    if (terreo == LOW) {
      botaoandar = 0;
      Serial.print("ter\n");

      delayTempo = millis();
      
    }

    andar1 = digitalRead(bandar1);
    if (andar1 == LOW) {
      botaoandar = 1;
      Serial.print("one\n");
      delayTempo = millis();
    }


    andar2 = digitalRead(bandar2);
    if (andar2 == LOW) {
      botaoandar = 2;
      Serial.print("two\n");
      delayTempo = millis();
    }


    andar3 = digitalRead(bandar3);
    if (andar3 == LOW) {
      botaoandar = 3;
      Serial.print("three\n");
      delayTempo = millis();
    }
    if (andar() != 4) {
      parado();
    }

    if (andar() == 4) {
      desce();
    }
  }
  else {
    if(andar() != 4){
      if (botaoandar > andar()) {
        while (botaoandar != andar()) {
          Serial.print("subiu\n");
          sobe();
        }
      }

      if (botaoandar < andar()) {
        while (botaoandar != andar()) {
          Serial.print("desceu\n");
          desce();
        }
      }

      if (botaoandar == andar()) {
        parado();
        botaoandar = 4;
      }
      }
      }
    }
