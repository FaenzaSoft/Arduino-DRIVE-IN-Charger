/* DRIVE-IN Charger (SIBILLA_1_2_3_5110), hardware versione 1A:
 "SIBILLA": centralina per caricare piccole batterie da RETE, con programmazione ARDUINO:
 Vedasi video YouTube: https://www.youtube.com/watch?v=fpe12RXMBfs
 e https://www.youtube.com/watch?v=znkSQbhux9o
 Versione: 1.0 del 29/03/2017
 Autore: SoftPlus Consumo Zero 
 Utilizzabile con alimentatori stabilizzati (max 1 amper) da 9 volt, 12 volt, 15 volt
 
 Utilizzabile per la carica e il check-up di batterie singole o a pacchi al Litio, NiMH, Piombo/Gel
 
 Visualizzazione su display Nokia 5110 LCD Module
 
 Vengono visualizzte, rispettivamente - su 6 righe - le seguenti informazioni:
 1) Volt batteria sotto carica;
 2) Intensità della carica;
 3) Carica cumulata, cioè milliamperora;
 4) Timer cioè durata / Minuti trascorsi da inizio carica;
 5) Milliamper limite della carica;
 6) Tensione limite della carica.
 NB: nella riga 4 si alternano la visualizzazione del Timer e il tempo trascorso da inizio carica. 
 
 Queste informazioni vengono aggiornate continuamente. 
 
 Durante la carica è possibile modificare le informazioni di carica, tramite i potenziometri slider.
 
 caricoPin = 6;             // carico analogico PWM pin 6 per Mosfet di potenza:
 volt_IN = A0;              // ingresso analogico A0 sensore volt INGRESSO:
 volt_SH = A1;              // ingresso analogico A1 sensore prima della resistenza di shunt (negativo batteria):
 volt_DR = A2;              // ingresso analogico A2 sensore DRAIN MOSFET: 
 limite_volt = A3           // ingresso analogico A3 sensore tensione:
 limite_amper = A4          // ingresso analogico A4 sensore corrente: 
 timer = A5                 // ingresso analogico sensore durata carica:   

Collegamenti al display 5110, partendo da sinistra:

- RST collegato con Arduino pin D12
- CE collegato con Arduino pin D13
- DC collegato con Arduino pin D11
- Din collegato con Arduino pin D10
- Clk collegato con Arduino pin D9
- Vcc collegato con positivo 3,3 volt
- BL collegato con positivo 3,3 volt
- Gnd collegato con Arduino GND.

IMPORTANTE: i pin D9, D10, D11, D12 e D13 NON sono collegati direttamente ai pin del 5110, ma lo sono tramite un divisore di 
tensione che consente di arrivare con solo 3,3 volt, così articolato: lato pin Arduino resistenza da 4,7 K, lato massa 
resistenza da 10 K. I pin del 5110 sono collegati nel punto di unione del partitore. 
 
 */
// INIZIO CODICE//

//
#include <Arduino.h> //inclusione della libreria // definizione dei pin di INPUT e OUTPUT:
// includere la libreria:
// #include <LiquidCrystal.h>
//
#define CONTRASTO 155
#define PIN_SCE   13// 13
#define PIN_RESET 12 //12
#define PIN_DC    11 //11
#define PIN_SDIN  10//11
#define PIN_SCLK  9
#define SER_BAUD  9600

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48

// #include "font.h";

static const byte ASCII[][5] =
{
  {
    0x00, 0x00, 0x00, 0x00, 0x00  } // 20  
  ,{
    0x00, 0x00, 0x5f, 0x00, 0x00  } // 21 !
  ,{
    0x00, 0x07, 0x00, 0x07, 0x00  } // 22 "
  ,{
    0x14, 0x7f, 0x14, 0x7f, 0x14  } // 23 #
  ,{
    0x24, 0x2a, 0x7f, 0x2a, 0x12  } // 24 $
  ,{
    0x23, 0x13, 0x08, 0x64, 0x62  } // 25 %
  ,{
    0x36, 0x49, 0x55, 0x22, 0x50  } // 26 &
  ,{
    0x00, 0x05, 0x03, 0x00, 0x00  } // 27 '
  ,{
    0x00, 0x1c, 0x22, 0x41, 0x00  } // 28 (
  ,{
    0x00, 0x41, 0x22, 0x1c, 0x00  } // 29 )
  ,{
    0x14, 0x08, 0x3e, 0x08, 0x14  } // 2a *
  ,{
    0x08, 0x08, 0x3e, 0x08, 0x08  } // 2b +
  ,{
    0x00, 0x50, 0x30, 0x00, 0x00  } // 2c ,
  ,{
    0x08, 0x08, 0x08, 0x08, 0x08  } // 2d -
  ,{
    0x00, 0x60, 0x60, 0x00, 0x00  } // 2e .
  ,{
    0x20, 0x10, 0x08, 0x04, 0x02  } // 2f /
  ,{
    0x3e, 0x51, 0x49, 0x45, 0x3e  } // 30 0
  ,{
    0x00, 0x42, 0x7f, 0x40, 0x00  } // 31 1
  ,{
    0x42, 0x61, 0x51, 0x49, 0x46  } // 32 2
  ,{
    0x21, 0x41, 0x45, 0x4b, 0x31  } // 33 3
  ,{
    0x18, 0x14, 0x12, 0x7f, 0x10  } // 34 4
  ,{
    0x27, 0x45, 0x45, 0x45, 0x39  } // 35 5
  ,{
    0x3c, 0x4a, 0x49, 0x49, 0x30  } // 36 6
  ,{
    0x01, 0x71, 0x09, 0x05, 0x03  } // 37 7
  ,{
    0x36, 0x49, 0x49, 0x49, 0x36  } // 38 8
  ,{
    0x06, 0x49, 0x49, 0x29, 0x1e  } // 39 9
  ,{
    0x00, 0x36, 0x36, 0x00, 0x00  } // 3a :
  ,{
    0x00, 0x56, 0x36, 0x00, 0x00  } // 3b ;
  ,{
    0x08, 0x14, 0x22, 0x41, 0x00  } // 3c <
  ,{
    0x14, 0x14, 0x14, 0x14, 0x14  } // 3d =
  ,{
    0x00, 0x41, 0x22, 0x14, 0x08  } // 3e >
  ,{
    0x02, 0x01, 0x51, 0x09, 0x06  } // 3f ?
  ,{
    0x32, 0x49, 0x79, 0x41, 0x3e  } // 40 @
  ,{
    0x7e, 0x11, 0x11, 0x11, 0x7e  } // 41 A
  ,{
    0x7f, 0x49, 0x49, 0x49, 0x36  } // 42 B
  ,{
    0x3e, 0x41, 0x41, 0x41, 0x22  } // 43 C
  ,{
    0x7f, 0x41, 0x41, 0x22, 0x1c  } // 44 D
  ,{
    0x7f, 0x49, 0x49, 0x49, 0x41  } // 45 E
  ,{
    0x7f, 0x09, 0x09, 0x09, 0x01  } // 46 F
  ,{
    0x3e, 0x41, 0x49, 0x49, 0x7a  } // 47 G
  ,{
    0x7f, 0x08, 0x08, 0x08, 0x7f  } // 48 H
  ,{
    0x00, 0x41, 0x7f, 0x41, 0x00  } // 49 I
  ,{
    0x20, 0x40, 0x41, 0x3f, 0x01  } // 4a J
  ,{
    0x7f, 0x08, 0x14, 0x22, 0x41  } // 4b K
  ,{
    0x7f, 0x40, 0x40, 0x40, 0x40  } // 4c L
  ,{
    0x7f, 0x02, 0x0c, 0x02, 0x7f  } // 4d M
  ,{
    0x7f, 0x04, 0x08, 0x10, 0x7f  } // 4e N
  ,{
    0x3e, 0x41, 0x41, 0x41, 0x3e  } // 4f O
  ,{
    0x7f, 0x09, 0x09, 0x09, 0x06  } // 50 P
  ,{
    0x3e, 0x41, 0x51, 0x21, 0x5e  } // 51 Q
  ,{
    0x7f, 0x09, 0x19, 0x29, 0x46  } // 52 R
  ,{
    0x46, 0x49, 0x49, 0x49, 0x31  } // 53 S
  ,{
    0x01, 0x01, 0x7f, 0x01, 0x01  } // 54 T
  ,{
    0x3f, 0x40, 0x40, 0x40, 0x3f  } // 55 U
  ,{
    0x1f, 0x20, 0x40, 0x20, 0x1f  } // 56 V
  ,{
    0x3f, 0x40, 0x38, 0x40, 0x3f  } // 57 W
  ,{
    0x63, 0x14, 0x08, 0x14, 0x63  } // 58 X
  ,{
    0x07, 0x08, 0x70, 0x08, 0x07  } // 59 Y
  ,{
    0x61, 0x51, 0x49, 0x45, 0x43  } // 5a Z
  ,{
    0x00, 0x7f, 0x41, 0x41, 0x00  } // 5b [
  ,{
    0x02, 0x04, 0x08, 0x10, 0x20  } // 5c ¥
  ,{
    0x00, 0x41, 0x41, 0x7f, 0x00  } // 5d ]
  ,{
    0x04, 0x02, 0x01, 0x02, 0x04  } // 5e ^
  ,{
    0x40, 0x40, 0x40, 0x40, 0x40  } // 5f _
  ,{
    0x00, 0x01, 0x02, 0x04, 0x00  } // 60 `
  ,{
    0x20, 0x54, 0x54, 0x54, 0x78  } // 61 a
  ,{
    0x7f, 0x48, 0x44, 0x44, 0x38  } // 62 b
  ,{
    0x38, 0x44, 0x44, 0x44, 0x20  } // 63 c
  ,{
    0x38, 0x44, 0x44, 0x48, 0x7f  } // 64 d
  ,{
    0x38, 0x54, 0x54, 0x54, 0x18  } // 65 e
  ,{
    0x08, 0x7e, 0x09, 0x01, 0x02  } // 66 f
  ,{
    0x0c, 0x52, 0x52, 0x52, 0x3e  } // 67 g
  ,{
    0x7f, 0x08, 0x04, 0x04, 0x78  } // 68 h
  ,{
    0x00, 0x44, 0x7d, 0x40, 0x00  } // 69 i
  ,{
    0x20, 0x40, 0x44, 0x3d, 0x00  } // 6a j 
  ,{
    0x7f, 0x10, 0x28, 0x44, 0x00  } // 6b k
  ,{
    0x00, 0x41, 0x7f, 0x40, 0x00  } // 6c l
  ,{
    0x7c, 0x04, 0x18, 0x04, 0x78  } // 6d m
  ,{
    0x7c, 0x08, 0x04, 0x04, 0x78  } // 6e n
  ,{
    0x38, 0x44, 0x44, 0x44, 0x38  } // 6f o
  ,{
    0x7c, 0x14, 0x14, 0x14, 0x08  } // 70 p
  ,{
    0x08, 0x14, 0x14, 0x18, 0x7c  } // 71 q
  ,{
    0x7c, 0x08, 0x04, 0x04, 0x08  } // 72 r
  ,{
    0x48, 0x54, 0x54, 0x54, 0x20  } // 73 s
  ,{
    0x04, 0x3f, 0x44, 0x40, 0x20  } // 74 t
  ,{
    0x3c, 0x40, 0x40, 0x20, 0x7c  } // 75 u
  ,{
    0x1c, 0x20, 0x40, 0x20, 0x1c  } // 76 v
  ,{
    0x3c, 0x40, 0x30, 0x40, 0x3c  } // 77 w
  ,{
    0x44, 0x28, 0x10, 0x28, 0x44  } // 78 x
  ,{
    0x0c, 0x50, 0x50, 0x50, 0x3c  } // 79 y
  ,{
    0x44, 0x64, 0x54, 0x4c, 0x44  } // 7a z
  ,{
    0x00, 0x08, 0x36, 0x41, 0x00  } // 7b {
  ,{
    0x00, 0x00, 0x7f, 0x00, 0x00  } // 7c |
  ,{
    0x00, 0x41, 0x36, 0x08, 0x00  } // 7d }
  ,{
    0x10, 0x08, 0x08, 0x10, 0x08  } // 7e ←
  ,{
    0x78, 0x46, 0x41, 0x46, 0x78  } // 7f →
};
//
// ************************** PARAMETRO MODIFICABILE UNA TANTUM ****************************************:
// unsigned equalizzatore = 4.33;        // regolazione fine misura tensioni da mettere a punto una tantum :
double equalizzatore = 4.15;        // regolazione fine misura delle tensioni, da mettere a punto una tantum :
// ************************** FINE **********************************************************************:
//
// ************************** PARAMETRI MODIFICABILI CON POTENZIOMETRI SLIDER **************************:
int limite_tensione = 0;            // limite tensione di carica, in volt, con 2 decimali (senza virgola):
int limite_corrente = 0;            // limite corrente di carica, in milliamper:
int limite_durata_carica = 0;       // limite orario, va espresso in minuti(120 = 2 ore):
// ************************** FINE PARAMETRI MODIFICABILI *******************************************:
//
//
int tensione_batterie = 0; 
unsigned long volt_INPUT = 0;
unsigned long volt_SHUNT = 0;
unsigned long volt_MOSFET = 0;
unsigned long volt_batterie = 0;
unsigned long somma_INPUT = 0;
unsigned long somma_SHUNT = 0;
unsigned long somma_MOSFET = 0;
unsigned long mAmper = 0;
unsigned long volt_crep = 0;
unsigned long coefficiente = 0;
int ciclopwm = 0;
int batterie = 0;
// int limite = 0;
unsigned long mAmperora = 0;
int Amperora = 0;
unsigned long durata = 0;
unsigned long stop_millis_nuovo = 0;
unsigned long stop_millis_vecchio = 0;
unsigned long millesimi_trascorsi = 0;
unsigned long carica = 0;
unsigned long ore = 0;
//unsigned long minuti = 0;
int minuti = 0;
unsigned long secondi = 0;
int stop_carica = 0;
int conta1 = - 50;
int conta2 = - 50;
int conta3 = 100;     // tempo per fare le regolazioni iniziali:
int conta4 = 0;
int migliaia = 0;
int centinaia = 0;
int decine = 0;
int unita = 0;
int resto = 0;
int dato = 0;
int numero = 0;
int inizio = 0;
//
void setup()
{
  //
  Serial.begin(9600);
  LcdInitialise();
  LcdClear();
  //  
  //  limite_durata_carica = limite_durata_carica * 60;       // la durata è espressa in secondi:
  pinMode(6, OUTPUT);     // pin che gestisce il PWM, diverso da quello delle altre versioni, che è il 9:
  pinMode(A0, INPUT);     // ingresso analogico A0 sensore volt ingresso (coincide con il positivo batteria):
  pinMode(A1, INPUT);     // ingresso analogico A1 sensore negativo batterie e monte resistenza:
  pinMode(A2, INPUT);     // ingresso analogico A2 sensore valle resistenza (drain Mosfet):
  pinMode(A3, INPUT);     // ingresso analogico A3 sensore limite volt ingresso:
  pinMode(A4, INPUT);     // ingresso analogico A4 sensore limite corrente:
  pinMode(A5, INPUT);     // ingresso analogico A5 sensore timer:  
  //
}
char string[1];
//    
void loop()
{
  if (inizio == 0)
  {
    gotoXY(0,1); 
    LcdString("  SoftPlus"); 
    gotoXY(1,3);   
    LcdString("Consumo Zero"); 
    inizio = 1;
    ciclopwm = 0;
  }
  //
  //
  // 
  // step 1 verifica tensioni:
  //
  somma_INPUT = 0;
  somma_SHUNT = 0;
  somma_MOSFET = 0;
  //
  for (int cicli = 0; cicli < 100; cicli++)
  {
    volt_INPUT = analogRead(A0);
    somma_INPUT = (somma_INPUT + volt_INPUT);
    //
    volt_SHUNT = analogRead(A1);
    somma_SHUNT = (somma_SHUNT + volt_SHUNT);
    //
    volt_MOSFET= analogRead(A2);
    somma_MOSFET = (somma_MOSFET + volt_MOSFET);
    //
    delay(2);
  }
  //
  // step 2 rettifica i dati delle tensioni:
  //
  volt_INPUT = (somma_INPUT / 100);  // perchè è stata fatta la somma di 100 cicli:
  volt_SHUNT = (somma_SHUNT / 100);  // perchè è stata fatta la somma di 100 cicli:
  volt_MOSFET = (somma_MOSFET / 100);  // perchè è stata fatta la somma di 100 cicli:
  volt_INPUT = (volt_INPUT * equalizzatore);     // perchè sull'ingresso c'è un divisore di tensione 1/4,3:
  volt_SHUNT = (volt_SHUNT * equalizzatore);     // perchè c'è un divisore di tensione 1/4,3:
  volt_MOSFET = (volt_MOSFET * equalizzatore);   // perchè c'è un divisore di tensione 1/4,3:
  volt_INPUT = (volt_INPUT * 500 / 1024);     // perchè i 5 volt corispondono a 1024 segmenti:
  volt_SHUNT = (volt_SHUNT * 500 / 1024);      // perchè i 5 volt corispondono a 1024 segmenti:
  volt_MOSFET = (volt_MOSFET * 500 / 1024);     // perchè i 5 volt corispondono a 1024 segmenti:
  // la tensione batterie va calcolata per differenza:
  volt_batterie = (volt_INPUT - volt_SHUNT);
  //
  limite_tensione = analogRead(A3) * 1.37;   // massimo 1024 segmenti, cioè 14 volt (on 2 decimali):
  //
  limite_corrente = analogRead(A4) * 0.97;     // massimo 1 amper con 1024 segmenti:
  //
//  if (conta4 > 2) 
//  {
    limite_durata_carica = analogRead(A5);      // arriva massimo a 1024 minuti:
//    delay(2);
//    limite_durata_carica = (limite_durata_carica + analogRead(A5)) / 2; 
//    conta4 = 0;
//  }
  //
  //  durata = millis()/1000;
  //  if (durata > limite_durata_carica * 60) stop_carica = 1; 
  durata = millis()/60000;
  if (durata > limite_durata_carica) stop_carica = 1; 
  //  
  if (volt_SHUNT > volt_MOSFET)
  {
    mAmper = (volt_SHUNT - volt_MOSFET);     // perchè con 1ohm di carico abbiamo un amper di carica:
    mAmper = mAmper * 10;                    // moltiplichiamo x 10 per avere i milliamper, anche perchè i volt sono già x 100:
    //    mAmper = mAmper / 3.3;                   // perchè abbiamo 1 resistenza da 3,3 ohm (3 da 10 ohm in parallelo) anzichè da 1 ohm:    
    mAmper = mAmper / 2;                   // perchè abbiamo 1 resistenza da 3,3 ohm (3 da 10 ohm in parallelo) anzichè da 1 ohm:      
  }
  else
  {
    mAmper = 0;
  }
  if (mAmper > 999) mAmper = 999;
  // 
  // step 3 modifica ciclo PWM e attiva il limitatore generale:
  //
  if (volt_batterie > limite_tensione) 
  {
    ciclopwm = ciclopwm - 2;
  }
  else 
  {
    ciclopwm = ciclopwm + 1;
  }
  //
  if (mAmper > limite_corrente) ciclopwm = ciclopwm - 3;        // limitatore a xxx mAmper definito in testa al listato:
  //
  if (ciclopwm > 254) ciclopwm = 254;
  //  if (ciclopwm < 1) ciclopwm = 1;
  if (ciclopwm < 0) ciclopwm = 0; 
  analogWrite (6, ciclopwm);    
  conta1 = conta1 + 1;
  conta2 = conta2 + 1;
  conta3 = conta3 + 1;    
  conta4 = conta4 + 1;     
  //
  if (conta1 > 3)
  {
    gotoXY(0,0); 
    LcdString("Voltbat     ");
    if (volt_batterie > 999)
    {
      numero = volt_batterie /1000;   
      resto = volt_batterie - (numero * 1000);
      Trasforma(); 
      gotoXY(54,0);    
      LcdCharacter(dato); 
      numero = resto /100;
      resto = resto - (numero * 100);     
    }
    else
    {
      numero = volt_batterie / 100;        
      resto = volt_batterie - (numero * 100);
    }
    Trasforma(); 
    gotoXY(60,0);    
    LcdCharacter(dato);  
    gotoXY(66,0);    
    LcdString(","); 
    numero = resto/10;
    resto = resto - (numero * 10);
    Trasforma();   
    gotoXY(71,0);    
    LcdCharacter(dato); 
    numero = resto;
    Trasforma();
    gotoXY(78,0);    
    LcdCharacter(dato);     
    //
    //  
    // 
    gotoXY(0,1); 
    LcdString("Intensit    "); 
    numero = mAmper /100; 
    resto = mAmper - (numero * 100); 
    Trasforma();    
    gotoXY(65,1);    
    LcdCharacter(dato);  
    numero = resto/10;
    resto = numero - (numero * 10);
    Trasforma();    
    gotoXY(71,1);    
    LcdCharacter(dato); 
    numero = resto;
    Trasforma();    
    gotoXY(78,1);    
    LcdCharacter(dato);   
    //
    //
    //
    if (conta3 > 10)
    {
      stop_millis_nuovo = millis();
      millesimi_trascorsi = stop_millis_nuovo - stop_millis_vecchio;
      carica = carica + (mAmper * millesimi_trascorsi);
      stop_millis_vecchio = stop_millis_nuovo;
      if (carica > 3600000000)
      {
        Amperora = Amperora + 1;
        carica = 0;
      } 
      //    stop_millis_vecchio = stop_millis_nuovo;
      //    lcd.print ((Amperora*1000) + (carica/3600000));       
      //    mAmperOra = carica/3600000;  
      conta3 = 0;
    }
    gotoXY(0,2); 
    LcdString("Carica..    "); 
    gotoXY(57,2); 
    numero = ((Amperora*1000) + (carica/3600000)); 
    migliaia = numero / 1000;                           // numero = 1001, migliaia = 1:
    resto = numero - (migliaia * 1000);                 // resto = 1;
    centinaia = resto / 100;                            // centinaia = 0: 
    resto = resto - (centinaia * 100);                  // resto = 1
    decine = resto / 10;                                // decine = 0;
    unita = resto - (decine * 10);                      // unità = 1:
    if (numero < 10) unita = numero;
    if (migliaia > 0)
    {
      numero = migliaia;        
      Trasforma();    
      gotoXY(59,2);    
      LcdCharacter(dato);  
    }    
    if (centinaia > 0 || (centinaia == 0 && migliaia > 0)) 
    {
      numero = centinaia;        
      Trasforma();    
      gotoXY(65,2);    
      LcdCharacter(dato);  
    } 
    if (decine > 0 || (decine == 0 && centinaia > 0) || (decine == 0 && centinaia == 0)) 
    {
      numero = decine;
      Trasforma();    
      gotoXY(71,2);    
      LcdCharacter(dato);
      numero = resto;
    }
    numero = unita;
    Trasforma();    
    gotoXY(78,2);    
    LcdCharacter(dato); 
    //   }
    //
    //  
    //
    if (conta2 < 25)
    { 
      gotoXY(0,3); 
      LcdString("Minuti..    "); 
      numero = (millis() / 60000);
      if (numero > 999) numero = 999;    
      centinaia = numero / 100;              // numero = 101, centinaia = 1: 
      resto = numero - (centinaia * 100);    // resto = 1:
      decine = resto /10;                    // decine = 0;
      unita = resto - (decine * 10);         // unità = 1:
      if (numero < 10) unita = numero;  
      if (centinaia > 0)
      {
        numero = centinaia;        
        Trasforma();    
        gotoXY(65,3);    
        LcdCharacter(dato);  
      } 
      if (decine > 0 || (decine == 0 && centinaia > 0)) 
      {
        numero = decine;
        Trasforma();    
        gotoXY(71,3);    
        LcdCharacter(dato);
        numero = resto;
      }
      numero = unita;
      Trasforma();    
      gotoXY(78,3);    
      LcdCharacter(dato);   
      //
      if (stop_carica == 1) 
      {
        analogWrite (6, 0);
        delay(1000000000);        // stop carica a fine durata carica:
      }   
    }
    else
    {
      gotoXY(0,3); 
      LcdString("Timer...    "); 
      numero = limite_durata_carica;   // in minuti:
      if (numero > 999) numero = 999;     
      centinaia = numero / 100;
      resto = numero - (centinaia * 100);
      decine = resto /10;
      unita = resto - (decine * 10); 
      if (numero < 10) unita = numero;
      if (centinaia > 0)
      {
        numero = centinaia;        
        Trasforma();    
        gotoXY(65,3);    
        LcdCharacter(dato);  
      } 
      if (decine > 0 || (decine == 0 && centinaia > 0)) 
      {
        numero = decine;
        Trasforma();    
        gotoXY(71,3);    
        LcdCharacter(dato);
        numero = resto;
      }
      numero = unita;
      Trasforma();    
      gotoXY(78,3);    
      LcdCharacter(dato);  
      if (conta2 > 50)  conta2 = 0;
    }
    //
    //
    //
    gotoXY(0,4); 
    LcdString("mAmp_max   "); 
    numero = limite_corrente /100; 
    resto = limite_corrente - (numero * 100); 
    Trasforma();    
    gotoXY(65,4);    
    LcdCharacter(dato);  
    numero = resto/10;
    resto = numero - (numero * 10);
    Trasforma();    
    gotoXY(71,4);    
    LcdCharacter(dato); 
    numero = resto;
    Trasforma();    
    gotoXY(78,4);    
    LcdCharacter(dato);     
    // 
    //
    gotoXY(0,5); 
    LcdString("Voltmax     "); 
    if (limite_tensione > 999)
    {
      numero = limite_tensione /1000;   
      resto = limite_tensione - (numero * 1000);
      Trasforma(); 
      gotoXY(54,5);    
      LcdCharacter(dato); 
      numero = resto /100;
      resto = resto - (numero * 100);     
    }
    else
    {
      numero = limite_tensione / 100;        
      resto = limite_tensione - (numero * 100);
    }
    Trasforma(); 
    gotoXY(60,5);    
    LcdCharacter(dato);  
    gotoXY(66,5);    
    LcdString(","); 
    numero = resto/10;
    resto = resto - (numero * 10);
    Trasforma();   
    gotoXY(71,5);    
    LcdCharacter(dato); 
    numero = resto;
    Trasforma();
    gotoXY(78,5);    
    LcdCharacter(dato);  
    //
      conta1 = 0;
  }
  //
}
//
//
// Ultima parte:
// Inizio funzioni: 
void gotoXY(int x, int y)
{
  LcdWrite( 0, 0x80 | x);  // Column.
  LcdWrite( 0, 0x40 | y);  // Row.  

}

void LcdCharacter(char character)
{
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++)
  {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void LcdClear(void)
{
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    LcdWrite(LCD_D, 0x00);
  }
}

void LcdInitialise(void)
{
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, CONTRASTO );  // Set LCD Vop (Contrast). 
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.
  LcdWrite(LCD_C, 0x20 );
  LcdWrite(LCD_C, 0x0C );
}

void LcdString(char *characters)
{
  while (*characters)
  {
    LcdCharacter(*characters++);
  }
}

void Trasforma(void)
{
  switch(numero)
  {
  case 0: 
    dato = 48;
    break;
  case 1:
    dato = 49;
    break;
  case 2: 
    dato = 50;
    break;
  case 3:
    dato = 51;
    break; 
  case 4: 
    dato = 52;
    break;
  case 5:
    dato = 53;
    break;
  case 6: 
    dato = 54;
    break;
  case 7:
    dato = 55;
    break;   
  case 8: 
    dato = 56;
    break;
  case 9:
    dato = 57;
    break;     
  default:
    //  dato = 48;
    break;  
  } 
}
//
//

void drawLine(void)
{
  unsigned char  j;  
  for(j=0; j<84; j++) // top
  {
    gotoXY (j,0);
    LcdWrite (1,0x01);
  } 	
  for(j=0; j<84; j++) //Bottom
  {
    gotoXY (j,5);
    LcdWrite (1,0x80);
  } 	

  for(j=0; j<6; j++) // Right
  {
    gotoXY (83,j);
    LcdWrite (1,0xff);
  } 	
  for(j=0; j<6; j++) // Left
  {
    gotoXY (0,j);
    LcdWrite (1,0xff);
  }

}


void SerialInitialise(void) {
  Serial.begin(SER_BAUD);
}

void SerialRead(void) {
  if (Serial.available())
  {
    while (Serial.available())
    {
      LcdWrite(LCD_D, Serial.read());
    }
  }
}
void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}
//
// FINE:




































