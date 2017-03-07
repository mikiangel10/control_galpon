// #############################################################################
//Escrito por Miguel Gomez en 6/6/16 
//#Este programa prende y apaga el rele 2 segun las configuracones de horas y minutos
// en los programas 1,2 y 3
//Agregando enlace inhalambrico nrf24l01- Un maestro escucha en 5 canales, y hasta 5 esclavos pueden  
//enviar datos. el maestro es la direccion 0, los esclavos  de 1 a 5 .
//Agregando programas de luces en eeprom( 4 programas con 4 datos c/u) 16 bytes
//que se cuentan desde el 20 al 35. c/prog cuenta en este orden: Hora inicio, minutos de inicio
//Hora de apagado y minutos de apagado
//
// #############################################################################
// *********************************************
// INCLUDE
// *********************************************
#include <Wire.h>                       // For some strange reasons, Wire.h must be included here
#include <DS1307new.h>
#include <SPI.h>
#include "RF24.h"
#include <EEPROM.h>

// *********************************************
// DEFINE
// *********************************************

#define programareloj 0 // '1' para programar; '0' para no programar
//    completar los datos deseados//
#define dia 06
#define mes 06
#define ano 2016 
#define hora 22
#define minuto 14
#define segundo 0

//pines
#define pinsegundo 2 //ds1307 da la señal 1 seg
#define rele1 4
#define rele2 5
//configuracion de los programas///////
#define hp11 18
#define hp12 22
#define hp21 05
#define hp22 8
#define mp11 00
#define mp12 30
#define mp21 30
#define mp22 30
#define hp31 00
#define hp32 01
#define mp31 00
#define mp32 00

//segundos para cambiar (pruebas solamente)///
#define espera 5


// *********************************************
// VARIABLES
// *********************************************
uint16_t startAddr = 0x0000;            // Start address to store in the NV-RAM
uint16_t lastAddr;                      // new address for storing in NV-RAM
uint16_t TimeIsSet = 0xaa55;   // Helper that time must not set again
boolean para=0;
int contador1=0;
int radioNumber=0; 
byte direcciones[][7]={"Master","1Nodo","2Nodo","3Nodo","4Nodo","5Nodo","6Nodo" };
//********************************************************************************************
//Manejo de la interrupcion 0 producida por el ds1307 c/1 seg.
//***************************************************************

 
void enviadatos(){
para=0;
//Serial.println("Toma");
}
  
  //*************************************************************************
  //Rutina de configuracion
  //**********************************************************************
  
 
void setup()
{
  RF24 radio(9,10);
  delay(500);
  pinMode(rele1,OUTPUT);
  pinMode(rele2,OUTPUT);
  pinMode(pinsegundo, INPUT);                    // Test of the SQW pin, D2 = INPUT
  //digitalWrite(pinsegundo, HIGH);                // Test of the SQW pin, D2 = Pullup on
  //attachInterrupt(digitalPinToInterrupt(2), enviadatos ,FALLING);
  digitalWrite(rele1,HIGH);
  digitalWrite(rele2,HIGH);

  Serial.begin(115200);
  attachInterrupt(0, enviadatos,RISING);
/*
   PLEASE NOTICE: WE HAVE MADE AN ADDRESS SHIFT FOR THE NV-RAM!!!
                  NV-RAM ADDRESS 0x08 HAS TO ADDRESSED WITH ADDRESS 0x00=0
                  TO AVOID OVERWRITING THE CLOCK REGISTERS IN CASE OF
                  ERRORS IN YOUR CODE. SO THE LAST ADDRESS IS 0x38=56!
*/
  RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t));// Store startAddr in NV-RAM address 0x08 

/*
   modificar el define 'programareloj' en 1 para modificar los datos del reloj, 
   sino poner en 0 para modificar el programa sin afectar el reloj.
*/

if(programareloj==1)
{
  TimeIsSet = 0xffff;
  RTC.setRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));  
}
/*
  Control the clock.
  Clock will only be set if NV-RAM Address does not contain 0xaa.
  DS1307 should have a battery backup.
*/
  RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
  if (TimeIsSet != 0xaa55)
  {
    RTC.stopClock();
        
    RTC.fillByYMD(ano,mes,dia);
    RTC.fillByHMS(hora,minuto,segundo);
    
    RTC.setTime();
    TimeIsSet = 0xaa55;
    RTC.setRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
    RTC.startClock();
  }
  else
  {
    RTC.getTime();
  }

/*
   Control Register for SQW pin which can be used as an interrupt.
*/
  RTC.ctrl = 0x10;                      // 0x00=disable SQW pin, 0x10=1Hz,
                                        // 0x11=4096Hz, 0x12=8192Hz, 0x13=32768Hz
  RTC.setCTRL();
 RTC.getCTRL();
  Serial.println("DS1307 Testsketch");
  Serial.println("Format is \"hh:mm:ss dd-mm-yyyy DDD\"");
  Serial.println(RTC.ctrl);
  
}

// *********************************************
// MAIN (LOOP)
// *********************************************
void loop()
{
  
  
  RTC.getTime();
  if (RTC.hour < 10)                    // correct hour if necessary
  {
    Serial.print("0");
    Serial.print(RTC.hour, DEC);
  } 
  else
  {
    Serial.print(RTC.hour, DEC);
  }
  Serial.print(":");
  if (RTC.minute < 10)                  // correct minute if necessary
  {
    Serial.print("0");
    Serial.print(RTC.minute, DEC);
  }
  else
  {
    Serial.print(RTC.minute, DEC);
  }
  Serial.print(":");
  if (RTC.second < 10)                  // correct second if necessary
  {
    Serial.print("0");
    Serial.print(RTC.second, DEC);
  }
  else
  {
    Serial.print(RTC.second, DEC);
  }
  Serial.print(" ");
  if (RTC.day < 10)                    // correct date if necessary
  {
    Serial.print("0");
    Serial.print(RTC.day, DEC);
  }
  else
  {
    Serial.print(RTC.day, DEC);
  }
  Serial.print("-");
  if (RTC.month < 10)                   // correct month if necessary
  {
    Serial.print("0");
    Serial.print(RTC.month, DEC);
  }
  else
  {
    Serial.print(RTC.month, DEC);
  }
  Serial.print("-");
  Serial.print(RTC.year, DEC);          // Year need not to be changed
  Serial.print(" ");
  switch (RTC.dow)                      // Friendly printout the weekday
  {
    case 1:
      Serial.print("LUN");
      break;
    case 2:
      Serial.print("MAR");
      break;
    case 3:
      Serial.print("MIE");
      break;
    case 4:
      Serial.print("JUE");
      break;
    case 5:
      Serial.print("VIE");
      break;
    case 6:
      Serial.print("SAB");
      break;
    case 7:
      Serial.print("DOM");
      break;
  }
  
  //usar RTC.setRAM(posicion,dato, tamaño)
  //usar RTC.getRAM(posicion,variable, tamaño)
  if (TimeIsSet == 0xaa55)              // check if the clock was set or not
  {
    Serial.println(" - Clock was set!");
  }
  else
  {
    Serial.println(" - Clock was NOT set!");
  }    
  
    
    
  if(RTC.hour==hp11 & RTC.minute==mp11){
    digitalWrite(rele2,LOW);
  }
  if(RTC.hour==hp21 & RTC.minute==mp21)    {
    digitalWrite(rele2,LOW);
  }
  
  if(RTC.hour==hp12 & RTC.minute==mp12){
    digitalWrite(rele2,HIGH);
  }
  if(RTC.hour==hp22 & RTC.minute==mp22)    {
    digitalWrite(rele2,HIGH);
  }
   if(RTC.hour==hp31 & RTC.minute==mp31){
    digitalWrite(rele2,LOW);
  }
  if(RTC.hour==hp32 & RTC.minute==mp32)    {
    digitalWrite(rele2,HIGH);
  }
    
  para=1;
  while(para){}

}

