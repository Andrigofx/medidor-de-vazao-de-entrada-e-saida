// Medidor de vazão de entrada e saida de um reservatório
// Scketch adaptado por Andrigo Farias Xavier
// Laboratório de Agrotécnologia UFPEL
// @andrigofx   andrigoxavier@gmail.com
// 23/07/2024

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // Endereço I2C e tamanho do display (20 colunas x 4 linhas)
float fluxo_entrada = 0; // Variável para a vazão de entrada
float fluxo_saida = 0; // Variável para a vazão de saída
float volume_entrada = 0; // Volume de entrada
float volume_saida = 0; // Volume de saída
float volume_total_entrada = 0; // Volume total acumulado de entrada
float volume_total_saida = 0; // Volume total acumulado de saída
unsigned long tempo_antes_entrada = 0; // Tempo anterior para entrada
unsigned long tempo_antes_saida = 0; // Tempo anterior para saída
const int INTERRUPCAO_SENSOR_ENTRADA = 0; // Interrupção para o sensor de entrada (pino digital 2)
const int INTERRUPCAO_SENSOR_SAIDA = 1; // Interrupção para o sensor de saída (pino digital 3)
const int PINO_SENSOR_ENTRADA = 2; // Pino do sensor de entrada
const int PINO_SENSOR_SAIDA = 3; // Pino do sensor de saída
const float FATOR_CALIBRACAO = 4.5; // Fator de calibração

volatile unsigned long contador_entrada = 0; // Contador de pulsos para entrada
volatile unsigned long contador_saida = 0; // Contador de pulsos para saída

void setup() {
  lcd.init();
  lcd.backlight(); // Ativa o backlight (luz de fundo)
  Serial.begin(9600);
  Serial.println("Medidor de Fluxo e Volume de Líquidos\n");
  pinMode(PINO_SENSOR_ENTRADA, INPUT_PULLUP); // Configura o pino do sensor de entrada como entrada em nível lógico alto
  pinMode(PINO_SENSOR_SAIDA, INPUT_PULLUP); // Configura o pino do sensor de saída como entrada em nível lógico alto
  attachInterrupt(INTERRUPCAO_SENSOR_ENTRADA, contador_pulso_entrada, FALLING); // Contagem de pulsos do sensor de entrada
  attachInterrupt(INTERRUPCAO_SENSOR_SAIDA, contador_pulso_saida, FALLING); // Contagem de pulsos do sensor de saída
}

void loop() {
  // Cálculos para o sensor de entrada
  if ((millis() - tempo_antes_entrada) > 1000) {
    detachInterrupt(INTERRUPCAO_SENSOR_ENTRADA);
    fluxo_entrada = ((1000.0 / (millis() - tempo_antes_entrada)) * contador_entrada) / FATOR_CALIBRACAO;
    volume_entrada = fluxo_entrada / 60;
    volume_total_entrada += volume_entrada;
    contador_entrada = 0;
    tempo_antes_entrada = millis();
    attachInterrupt(INTERRUPCAO_SENSOR_ENTRADA, contador_pulso_entrada, FALLING);
  }

  // Cálculos para o sensor de saída
  if ((millis() - tempo_antes_saida) > 1000) {
    detachInterrupt(INTERRUPCAO_SENSOR_SAIDA);
    fluxo_saida = ((1000.0 / (millis() - tempo_antes_saida)) * contador_saida) / FATOR_CALIBRACAO;
    volume_saida = fluxo_saida / 60;
    volume_total_saida += volume_saida;
    contador_saida = 0;
    tempo_antes_saida = millis();
    attachInterrupt(INTERRUPCAO_SENSOR_SAIDA, contador_pulso_saida, FALLING);
  }

  // Atualiza o display LCD
  lcd.setCursor(0, 0);
  lcd.print("Entrada: ");
  lcd.print(fluxo_entrada);
  lcd.print(" L/min");
  lcd.setCursor(0, 1);
  lcd.print("Volume Entrada: ");
  lcd.print(volume_total_entrada);
  lcd.print(" L");
  lcd.setCursor(0, 2);
  lcd.print("Saida: ");
  lcd.print(fluxo_saida);
  lcd.print(" L/min");
  lcd.setCursor(0, 3);
  lcd.print("Volume Saída: ");
  lcd.print(volume_total_saida);
  lcd.print(" L");
}

void contador_pulso_entrada() {
  contador_entrada++;
}

void contador_pulso_saida() {
  contador_saida++;
}