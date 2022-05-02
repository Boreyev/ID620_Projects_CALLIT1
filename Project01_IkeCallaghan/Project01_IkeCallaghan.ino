#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   //Pin definitions

const int LED1 = 3;
const int LED2 = 5;
const int pirSensor1 = 2;
const int pirSensor2 = 4;
int buzzerPin = 6;
int BUTTONstate1 = 0;
int BUTTONstate2 = 0;
int availableSeats = 3;    

void loop()
{
  characterAnimation(); //Call animation function.
}

//FUNCTIONS:
//------------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pirSensor1, INPUT);
  pinMode(pirSensor2, INPUT);
  lcd.begin(16, 2);             //LCD Display is 16x02                 
  lcd.clear();                      
}

void checkSensors(){  //Check sensors for motion, display text and increment / decrement if needed.
  int sensorValue1 = digitalRead(pirSensor1);
  int sensorValue2 = digitalRead(pirSensor2);
  
    if (sensorValue1 == 1){
      if (availableSeats > 1){
        availableSeats--; 
        lcd.clear();                      //clear the display
        lcd.setCursor(0, 0);              //set the cursor to the 0,0 position (top left corner)
        lcd.print(" /)__/)  Weclome, please find a seat!");
        availableSeatsFunction();
        entryChime();
        }  
         else if(availableSeats == 1){
             availableSeats--; 
             lcd.clear();                     
             lcd.setCursor(0, 0);              
             lcd.print(" /)__/)  Weclome, Please wait here!");
             availableSeatsFunction();
             noSeatChime();
             }
           else if(availableSeats == 0){
                 availableSeatsFunction();
                 noSeatChime();
           }
      sensorValue1 = 0;
      textScroll();
  }
  
  if (sensorValue2 == 1)
  {
    if (availableSeats < 30)  //Increment occupancy if below max occupancy
    {
      availableSeats++; 
    }  
    sensorValue2 = 0;
    lcd.clear();                      
    lcd.setCursor(0, 0);              
    lcd.print(" /)__/) Thank you, please come again!");  
    lcd.setCursor(0, 1);              //set the cursor to the 0,1 position (bottom left corner)
    lcd.print("( O .O)/");  
    exitChime();
    textScroll();
  }
}

void textScroll(){  //Controls textscroll speed and length, if length exceeded before sensor detected, animation will play. 
  for (int i = 0; i < 40; i++) {
    lcd.scrollDisplayLeft();    //Scroll displayed text on LCD 
    delay(200);                 //Set scroll speed to 200ms 
  }
    for (int i = 0; i < 40; i++) {
      lcd.scrollDisplayLeft();    //Scroll displayed text on LCD 
      delay(200);                 //Set scroll speed to 200ms 
      checkSensors();             //Check sensors on second cycle, allowing for text to be read.
    }
}

void characterAnimation(){  //Idle Character animation
  for (int i = 0; i < 5; i++){
  checkSensors(); //Call function to check sensors for each animation frame.
  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)");  
  lcd.setCursor(0, 1);
  lcd.print("( O .O)/"); 

  checkSensors();
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)");  
  lcd.setCursor(0, 1);
  lcd.print("( O .O)_"); 
 }
  checkSensors();
  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)");  
  lcd.setCursor(0, 1);
  lcd.print("( o ,o)_");  

  checkSensors();
  delay(800);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)    ");  
  lcd.setCursor(0, 1);
  lcd.print("( - o-)_");  
  
  checkSensors();
  delay(800);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)    ");  
  lcd.setCursor(0, 1);
  lcd.print("( - o-)");  
  delay(800);
  
  for (int i = 0; i < 64; i++) {
  checkSensors(); 
  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)   Z");  
  lcd.setCursor(0, 1);
  lcd.print("( - _-) z");  

  checkSensors();
  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" /)__/)   z");  
  lcd.setCursor(0, 1);
  lcd.print("( - _-) Z");
  }  
}
//Play entry tune on piezo, flash LED1 at same time.
void entryChime(){
    tone(buzzerPin, 131); 
    digitalWrite(LED1, HIGH);
    delay(300);
    tone(buzzerPin, 165);
    digitalWrite(LED1, LOW);
    delay(300);
    tone(buzzerPin, 196);
    digitalWrite(LED1, HIGH);
    delay(300);
    tone(buzzerPin, 165); 
    digitalWrite(LED1, LOW);
    delay(300);
    noTone(buzzerPin);
    digitalWrite(LED1, HIGH);
    delay(300);
    tone(buzzerPin, 262); 
    digitalWrite(LED1, LOW);
    delay(300);
    noTone(buzzerPin);
    digitalWrite(LED1, HIGH);
    delay(2000);
    digitalWrite(LED1, LOW);
}

//Play exit tune on piezo, flash LED2 at same time.
void exitChime(){
    tone(buzzerPin, 262); 
    digitalWrite(LED2, HIGH);
    delay(300);
    tone(buzzerPin, 195);
    digitalWrite(LED2, LOW);
    delay(300);
    tone(buzzerPin, 162);
    digitalWrite(LED2, HIGH);
    delay(300);
    tone(buzzerPin, 195); 
    digitalWrite(LED2, LOW);
    delay(300);
    tone(buzzerPin, 131); 
    digitalWrite(LED2, HIGH);
    delay(300);
    noTone(buzzerPin);
    digitalWrite(LED2, HIGH);
    delay(2000);
    digitalWrite(LED2, LOW);
}

//Play no seat chime on piezo, flash LED1 at same time.
void noSeatChime(){
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 131);
    digitalWrite(LED1, HIGH); 
    delay(300);
    tone(buzzerPin, 262);
    digitalWrite(LED1, LOW);
    delay(300);
    noTone(buzzerPin);
}
    digitalWrite(LED1, HIGH);
    delay(2000);
    digitalWrite(LED1, LOW);
}

//Change available seats statement depending on current occupancy.
void availableSeatsFunction(){
  lcd.setCursor(0, 1);               
  lcd.print("( O .O)/ There ");       
  if (availableSeats == 1)
  {
    lcd.print("is ");
    lcd.print(availableSeats);
    lcd.print(" seat");  
  }
  else if (availableSeats > 1){
    lcd.print("are ");
    lcd.print(availableSeats);
    lcd.print(" seats");  
  }
  else if (availableSeats == 0){
    lcd.print("are no seats");
  }
  lcd.print(" available.");  
}
