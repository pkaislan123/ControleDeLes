#include <IRremote.h> // Biblioteca IRemote
#include <Wire.h> //INCLUSÃO DE BIBLIOTECA
#include <Adafruit_GFX.h> //INCLUSÃO DE BIBLIOTECA
#include <Adafruit_SSD1306.h> //INCLUSÃO DE BIBLIOTECA



//variaveis globais
const int numLeds = 10;
const int numArray = numLeds - 1;
int pinsLeds[numArray]; //cria um array para guardar as portas dos les, com indices de 0 a 8
const int pinPot = A0; //porta A0 para o potenciometro
const int pinOledReset = 3;
const int pinIr = 2; // Arduino pino 2 conectado no Receptor IR
unsigned long key_value = 0;
int delaySeq = 0;

int indicesUsados[numArray];

//objetos
IRrecv iRReceiver(pinIr); // criando a instância
decode_results resultados; // declarando os resultados

Adafruit_SSD1306 display(pinOledReset);

boolean inProcess = false;
boolean finalizou = false;
boolean primeiraAcao = false;
unsigned long ultimaAcao = millis();
unsigned long ultimaAttLcd = millis();

int indiceUltimoLed = 0;
int seq = -1;
int tempoAttLcd = 1000;//frequencia em que o display se atualizara
int indiceAleatorio;
int contadorIndicesAleatorios = 0;


void setup() {
  //Configurações


  //Leds
  int portaLed = 4; //variavel com a porta inicial de les, iram de 4 até 13
  for (int i = 0; i <= 10; i++) {
    pinsLeds[i] = portaLed;
    pinMode(pinsLeds[i], OUTPUT);
    digitalWrite(pinsLeds[i], LOW);
    portaLed++;
  }

  //Analogico
  pinMode(pinPot, INPUT); //define o potenciometro na A0 como entrada

  //Comunicação Serial
  Serial.begin(9600);

  //InfraVermelho
  iRReceiver.enableIRIn(); //habilita o sensor IR

  //Display o Led
  Wire.begin(); //INICIALIZA A BIBLIOTECA WIRE
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); //INICIALIZA O DISPLAY COM ENDEREÇO I2C 0x3C
  display.setTextColor(WHITE); //DEFINE A COR DO TEXTO
  display.setTextSize(1); //DEFINE O TAMANHO DA FONTE DO TEXTO
  display.clearDisplay(); //LIMPA AS INFORMAÇÕES DO DISPLAY

  randomSeed(1023);
}

boolean start = true;


void loop() {

  delaySeq = map(analogRead(pinPot), 0, 1023, 0, 1000);

  if (inProcess) {
    if (seq == 1) {
      showSeq1();
    } else if (seq == 2) {
      showSeq2();
    } else if (seq == 3) {
      showSeq3();
    }
  } else {
    printFree();
  }

  if (start) {
    start = false;
    primeiraAcao = true;
    seq = 3;
    contadorIndicesAleatorios = 0;
    showSeq3();
  }




  if (iRReceiver.decode(&resultados)) {
    if (resultados.value == 0XFFFFFFFF)
      resultados.value = key_value;
    switch (resultados.value) {
      case 0xFF30CF: {
          Serial.println("Botão 1");
          primeiraAcao = true;
          seq = 1;
          showSeq1();

        } break;
      case 0xFF18E7: {
          Serial.println("Botão 2");
          primeiraAcao = true;
          seq = 2;
          showSeq2();

        } break;
      case 0xFF7A85: {
          Serial.println("Botão 3");
          primeiraAcao = true;
          seq = 3;
          contadorIndicesAleatorios = 0;
          showSeq3();

        } break;
    }
    key_value = resultados.value;
    iRReceiver.resume();
  }

}

void showSeq1() {

  printInfo();
  if (primeiraAcao) {
    digitalWrite(pinsLeds[0], HIGH);
    indiceUltimoLed = 0;
    ultimaAcao = millis();
    primeiraAcao = false;
    inProcess = true;
  } else {
    if (indiceUltimoLed <= numArray) {
      if ((millis() - ultimaAcao) > delaySeq) {
        digitalWrite(pinsLeds[indiceUltimoLed], LOW);
        indiceUltimoLed++;
        digitalWrite(pinsLeds[indiceUltimoLed], HIGH);
        ultimaAcao = millis();
      }
    } else if (indiceUltimoLed > numArray) {
      primeiraAcao = false;
      inProcess = false;
      indiceUltimoLed = 0;
      ultimaAcao = millis();
    }
  }
}

void showSeq2() {

  printInfo();

  if (primeiraAcao) {
    digitalWrite(pinsLeds[numArray], HIGH);
    indiceUltimoLed = numArray;
    primeiraAcao = false;
    inProcess = true;
    ultimaAcao = millis();
  } else {
    if (indiceUltimoLed >= 0) {
      if ((millis() - ultimaAcao) > delaySeq) {
        digitalWrite(pinsLeds[indiceUltimoLed], LOW);
        indiceUltimoLed--;
        digitalWrite(pinsLeds[indiceUltimoLed], HIGH);
        ultimaAcao = millis();
      }
    } else  {
      primeiraAcao = false;
      inProcess = false;
      indiceUltimoLed = 0;
      ultimaAcao = millis();
    }
  }
}

void showSeq3() {

  printInfo();

  if (primeiraAcao) {
  
    indiceAleatorio = random(0, numArray+1);
    indicesUsados[contadorIndicesAleatorios] = indiceAleatorio;
    digitalWrite(pinsLeds[indiceAleatorio], HIGH);

    primeiraAcao = false;
    contadorIndicesAleatorios++;
    inProcess = true;
    ultimaAcao = millis();
  } else {
    if (contadorIndicesAleatorios <= numArray) {

      if ((millis() - ultimaAcao) > delaySeq) {
       
        digitalWrite(pinsLeds[indiceAleatorio], LOW);//desliga o ultimo led ligado

        boolean indice_ja_usado = true;

        while (indice_ja_usado) {

          indiceAleatorio = random(0, numArray +1); //gera o indice aleatorio
          indice_ja_usado = false;

          for (int i = 0; i < contadorIndicesAleatorios  ; i++) {
            if (indicesUsados[i] == indiceAleatorio) {
              indice_ja_usado = true;
              break;
            }
          }

        }


        digitalWrite(pinsLeds[indiceAleatorio], HIGH);
        
        indicesUsados[contadorIndicesAleatorios] = indiceAleatorio;
        contadorIndicesAleatorios++;
        ultimaAcao = millis();
      }
    } else  {
      Serial.print("Contador: ");
      Serial.println(contadorIndicesAleatorios);
      Serial.println("Finalizado seq 3");

      digitalWrite(pinsLeds[indiceAleatorio], LOW);
      primeiraAcao = true;
      inProcess = false;
      seq = -1;
      ultimaAcao = millis();
    }
  }
}

void printLcd(int x, int y, String msg, int iDelay, boolean wash) {
  display.setCursor(x, y);
  display.print(msg);
  display.display();
  delay(iDelay);
  if (wash)
    display.clearDisplay();

}

void printFree() {
  seq = -1;
  printLcd(0, 0, "Modo: ", 0, false);
  printLcd(60, 0, "Parado", 0, false);

  printLcd(0, 10, "Velocidade: ", 0, false);
  String sDelay = String(delaySeq);
  sDelay.concat(" ms");
  printLcd(80, 10, sDelay, 0, false);

}

void printInfo() {
  if ((millis() - ultimaAttLcd) > tempoAttLcd) {
    display.clearDisplay();
    printLcd(0, 0, "Modo: ", 0, false);
    printLcd(60, 0, seq, 0, false);

    printLcd(0, 10, "Velocidade: ", 0, false);
    String sDelay = String(delaySeq);
    sDelay.concat(" ms");
    printLcd(80, 10, sDelay, 0, false);
    ultimaAttLcd = millis();
  }
}

void printLcd(int x, int y, int msg, int iDelay, boolean wash) {
  display.setCursor(x, y);
  display.print(msg);
  display.display();
  delay(iDelay);
  if (wash)
    display.clearDisplay();

}
