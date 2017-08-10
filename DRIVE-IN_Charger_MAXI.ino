/* DRIVE-IN Charger MAXI:
 Versione: 1.0 del 24/03/2017
 Autore: SoftPlus Consumo Zero
 Resistenza di shunt da 1 ohm
 Video YouTube: https://www.youtube.com/watch?v=9qNa5FgpiT4
 Rispetto al video sono stati aggiunti sul frontale gli slider.
 
 Vengono visualizzte le seguenti informazioni:
 1) durata della carica, in ore, minuti e secondi; 
 2) tensione batterie sotto carico, in volt con 2 decimali;
 3) energia immagazzinata in mAh;
 4) corrente di carica, in milliamper.
 
 E' possibile definire, tramite potenziometri a slitta, i seguenti parametri:
 1) tensione massima raggiungibili dalle batterie sotto carica (dipende dalla tensione nominale e dal tipo);
 2) corrente massima di carica (1/10 della capacità, ma varia tra piombo, NiMh e Litio);
 3) durata massima della carica (10 ore o più).
 
 caricoPin = 9;             // carico analogico PWM pin 9 per Mosfet di potenza:
 volt_IN = A0;              // ingresso analogico A0 sensore volt INGRESSO:
 volt_SH = A1;              // ingresso analogico A1 sensore prima della resistenza di shunt (negativo batteria):
 volt_DR = A2;              // ingresso analogico A2 sensore DRAIN MOSFET: 
 limite_volt = A3           // ingresso analogico A3 sensore tensione (centrale slider da 10K lineare):
 limite_amper = A4          // ingresso analogico A4 sensore corrente (centrale slider da 10K lineare): 
 timer = A5                 // ingresso analogico sensore durata carica (centrale slider da 47K lineare o log):  
 
 N.B: gli slider vanno collegati con un capo al positivo 5 volt e l'altro capo a GND. 
      i pin Arduino A3, A4 e A5 sono collegati al cursore centrale degli slider. 
 */
// 
// INIZIO CODICE//
//
#include <Arduino.h> //inclusione della libreria // definizione dei pin di INPUT e OUTPUT:
// includere la libreria:
#include <LiquidCrystal.h>
// vedi anche: http://www.maffucci.it/2012/02/17/appunti-su-arduino-pilotare-un-display-lcd/#sthash.Z10I1f17.dpuf
// uso del display LCD 16x2 standard Hitachi HD44780
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//
// ************************** PARAMETRO MODIFICABILE UNA TANTUM ****************************************:
// unsigned equalizzatore = 4.33;        // regolazione fine misura tensioni da mettere a punto una tantum :
double equalizzatore = 4.36;        // regolazione fine misura tensioni da mettere a punto una tantum :
// ************************** FINE PARAMETRI MODIFICABILI *********************************************:
//
int limite_tensione = 0;            // si regola con il potenziometro a slitta, in volt con due decimali:
int limite_corrente = 0;            // si regola con il potenziometro a slitta, in milliamper:
unsigned int limite_durata_carica = 0;       // si regola con il potenziometro a slitta, espresso in minuti(600 = 10 ore):
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
// unsigned long coefficiente = 0;
int ciclopwm = 0;
// int ciclopwm2 = 0;
int batterie = 0;
unsigned long mAmperora = 0;
int Amperora = 0;
unsigned long durata = 0;
unsigned long stop_millis_nuovo = 0;
unsigned long stop_millis_vecchio = 0;
unsigned long millesimi_trascorsi = 0;
unsigned long carica = 0;
unsigned long ore = 0;
unsigned long minuti = 0;
unsigned long secondi = 0;
int stop_carica = 0;
int conta1 = - 50;
int conta2 = - 50;
int conta3 = 100;     // tempo per fare le regolazioni iniziali:
//
void setup()
{
  limite_durata_carica = limite_durata_carica * 60;       // la durata è espressa in secondi:
  pinMode(9, OUTPUT);
  pinMode(A0, INPUT);     // ingresso analogico A0 sensore volt ingresso (coincide con il positivo batteria):
  pinMode(A1, INPUT);     // ingresso analogico A1 sensore negativo batterie e monte resistenza:
  pinMode(A2, INPUT);     // ingresso analogico A2 sensore valle resistenza (drain Mosfet):
  pinMode(A3, INPUT);     // ingresso analogico A3 sensore limite tensione:
  pinMode(A4, INPUT);     // ingresso analogico A4 sensore limite corrente:  
  pinMode(A5, INPUT);     // ingresso analogico A5 sensore limite durata carica:    
  //
}
void loop()
{
  // step 1 verifica tensioni:
  somma_INPUT = 0;
  somma_SHUNT = 0;
  somma_MOSFET = 0;
  for (int cicli = 0; cicli < 100; cicli++)
  {
    volt_INPUT = analogRead(A0);
    somma_INPUT = (somma_INPUT + volt_INPUT);
    volt_SHUNT = analogRead(A1);
    somma_SHUNT = (somma_SHUNT + volt_SHUNT);
    volt_MOSFET= analogRead(A2);
    somma_MOSFET = (somma_MOSFET + volt_MOSFET);
    delay(2);
  }
  // step 2 rettifica i dati delle tensioni:
  volt_INPUT = (somma_INPUT / 100);  // perchè è stata fatta la somma di 100 cicli:
  volt_SHUNT = (somma_SHUNT / 100);  // perchè è stata fatta la somma di 100 cicli:
  volt_MOSFET = (somma_MOSFET / 100);  // perchè è stata fatta la somma di 100 cicli:
  volt_INPUT = (volt_INPUT * 500 / 1024);     // perchè i 5 volt corispondono a 1024 segmenti:
  volt_SHUNT = (volt_SHUNT * 500 / 1024);      // perchè i 5 volt corispondono a 1024 segmenti:
  volt_MOSFET = (volt_MOSFET * 500 / 1024);     // perchè i 5 volt corispondono a 1024 segmenti:
  volt_INPUT = (volt_INPUT * equalizzatore);     // perchè sull'ingresso c'è un divisore di tensione 1/4,3:
  volt_SHUNT = (volt_SHUNT * equalizzatore);     // perchè c'è un divisore di tensione 1/4,3:
  volt_MOSFET = (volt_MOSFET * equalizzatore);   // perchè c'è un divisore di tensione 1/4,3: 
  // la tensione batterie va calcolata per differenza:
  volt_batterie = (volt_INPUT-volt_SHUNT); 
  //
  limite_tensione = analogRead(A3) * 1.37;   // massimo 1024 segmenti, cioè 14 volt (con 2 decimali):
  //
  limite_corrente = analogRead(A4) * 1.9;     // massimo 2 amper con 1024 segmenti:
  //
  limite_durata_carica = analogRead(A5);      // arriva massimo a 1024 minuti:
  //
  if (volt_SHUNT > volt_MOSFET)
  {
    mAmper = (volt_SHUNT - volt_MOSFET);     // perchè con 1ohm di carico abbiamo un amper di carica:
    mAmper = mAmper * 10;                    // moltiplichiamo x 10 per avere i milliamper, anche perchè i volt sono già x 100:
    //    mAmper = mAmper / 4.7;                   // perchè abbiamo una resistenza da 4,7 ohm, anzichè da 1 ohm:
    // mAmper = mAmper / 3.3;                   // perchè abbiamo 1 resistenza da 3,3 ohm (3 da 10 ohm in parallelo) anzichè da 1 ohm:    
    mAmper = mAmper / 1;                   // perchè abbiamo 1 resistenza da 1 ohm (10 da 10 ohm in parallelo):   
  }
  else
  {
    mAmper = 0;
  }
  //
  // step 3 modifica ciclo PWM e attiva il limitatore generale:
  if (conta3 > 30) ciclopwm = 0;    // azzera la carica nella fase di definizione dei valori:
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
//  if (mAmper > limite_corrente) ciclopwm = ciclopwm - 3;  
  if (mAmper > limite_corrente + 20)       // limitatore a xxx mAmper definito dall'utente:
  {
    ciclopwm = ciclopwm - 3;
  }
  if (mAmper < limite_corrente - 20)
  {
    ciclopwm = ciclopwm + 1; 
  }
  // 
  if (ciclopwm > 254) ciclopwm = 254;
  if (ciclopwm < 0) ciclopwm = 0; 
  analogWrite (9, ciclopwm);    
  conta1 = conta1 + 1;
  conta2 = conta2 + 1;
  conta3 = conta3 + 1;  
  //
  // step 4 visualizza la durata della carica e la tensione delle batterie:
  if (conta1 > 2)
  {
    // Visualizzo il messaggio sul display
    lcd.begin(16, 2);    
    lcd.print("hh-mm-sec V-Batt");         
    lcd.setCursor(0, 1);
    lcd.print("  :  :          ");
    //      lcd.print(ciclopwm1 * 100 / 254);
    durata = millis()/1000;
    if (durata > limite_durata_carica * 60) stop_carica = 1;
    if (durata >= 3600) 
    {
      ore = durata/3600;
      durata = durata - (ore * 3600);
    }
    if (durata < 3600) 
    {
      minuti = durata/60;
      durata = durata - (minuti * 60);
    }      
    if (durata < 60) secondi = durata;
    lcd.setCursor(0, 1);         
    lcd.print(ore);   
    lcd.setCursor(3, 1);         
    lcd.print(minuti);        
    lcd.setCursor(6, 1);         
    lcd.print(secondi);   
    lcd.setCursor(11, 1);    
    lcd.print(volt_batterie); 
    if (volt_batterie > 999) lcd.setCursor(12, 1);
    else lcd.setCursor(11, 1);
    lcd.print(",");        
    lcd.setCursor(10, 1); 
    lcd.print(volt_batterie/100);  
    conta1 = 0;
    delay(250);
  }
  // step 5 visualizza la carica cumulata e i milliamper di carica, in corso:
  if (conta2 > 5)
  {
    lcd.begin(16, 2);
    lcd.print("mAmperora mAmper");       
    lcd.setCursor(0, 1);
    lcd.print("               ");
    lcd.setCursor(2, 1);
    stop_millis_nuovo = millis();
    millesimi_trascorsi = stop_millis_nuovo - stop_millis_vecchio;
    carica = carica + (mAmper * millesimi_trascorsi);
    if (carica > 3600000000)
    {
      Amperora = Amperora + 1;
      carica = 0;
    } 
    stop_millis_vecchio = stop_millis_nuovo;
    lcd.print ((Amperora*1000) + (carica/3600000));    
    //
    lcd.setCursor(12, 1);
    lcd.print(mAmper);  
    conta2 = 0;
    delay(250);
    if (stop_carica == 1) 
    {
      analogWrite (9, 0);
      delay(1000000000);        // stop carica
    }
  }
  // step 6 visualizza i parametri impostati:
  if (conta3 > 25)
  { 
    lcd.begin(16, 2);
    lcd.print("volt  mAmp minut");       
    lcd.setCursor(0, 1);
    lcd.print("               ");
    lcd.setCursor(1, 1);    
    lcd.print(limite_tensione); 
    if (limite_tensione > 999) lcd.setCursor(2, 1);
    else lcd.setCursor(1, 1);
    lcd.print(",");        
    lcd.setCursor(0, 1); 
    lcd.print(limite_tensione/100);     
    lcd.setCursor(6, 1);     
    lcd.print(limite_corrente); 
    lcd.setCursor(12, 1);   
    lcd.print(limite_durata_carica); 
    if (conta1 > 0)       
    {
      delay(2000);
      conta3 = 0;
    }
  }  
}
// FINE:




































