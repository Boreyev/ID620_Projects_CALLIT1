#include <LiquidCrystal.h>

int Contrast=80;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   // tell the RedBoard what pins are connected to the display

const int LED1 = 3;
const int LED2 = 5;
const int pirSensor1 = 2;
const int pirSensor2 = 4;
int buzzerPin = 6;
int BUTTONstate1 = 0;
int BUTTONstate2 = 0;
int availableSeats = 30;    

void loop()
{
  int sensorValue1 = digitalRead(pirSensor1);
  int sensorValue2 = digitalRead(pirSensor2);

  if (sensorValue1 == 1)
  {
    if (availableSeats > 1)
    {
      availableSeats--; 
      lcd.clear();                      //clear the display
      lcd.setCursor(0, 0);              //set the cursor to the 0,0 position (top left corner)
      lcd.print(" /)__/)  Weclome, please find a seat!");
      availableSeatsFunction();
      entryChime();
      }  
     else if(availableSeats == 1){
         availableSeats--; 
         lcd.clear();                      //clear the display
         lcd.setCursor(0, 0);              //set the cursor to the 0,0 position (top left corner)
         lcd.print(" /)__/)  Weclome, Please wait here!");
         availableSeatsFunction();
         noSeatChime();
         }
     else if(availableSeats == 0){
           noSeatChime();
     }
    sensorValue1 = 0;
  }
  
  if (sensorValue2 == 1)
  {
    if (availableSeats < 30)  //Increment occupancy if below max occupancy
    {
      availableSeats++; 
    }  
    sensorValue2 = 0;
    lcd.clear();                      //clear the display
    lcd.setCursor(0, 0);              //set the cursor to the 0,0 position (top left corner)
    lcd.print(" /)__/) Thank you, we hope to see you again!");  
    lcd.setCursor(0, 1);              //set the cursor to the 0,1 position (bottom left corner)
    lcd.print("( O .O)/");  
    exitChime();
  }
  
  lcd.scrollDisplayLeft();  //Scroll displayed text on LCD 
  delay(200);               //Set scroll speed to 200ms 
}

//FUNCTIONS:
//------------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pirSensor1, INPUT);
  pinMode(pirSensor2, INPUT);
  analogWrite(7,Contrast);
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display
}

//Play entry tune on piezo, flash LED1 at same time.
void entryChime(){
    tone(buzzerPin, 330); 
    digitalWrite(LED1, HIGH);
    delay(300);
    tone(buzzerPin, 290);
    digitalWrite(LED1, LOW);
    delay(300);
    tone(buzzerPin, 523);
    digitalWrite(LED1, HIGH);
    delay(300);
    tone(buzzerPin, 330); 
    digitalWrite(LED1, LOW);
    delay(700);
    noTone(buzzerPin);
    digitalWrite(LED1, HIGH);
    delay(2000);
    digitalWrite(LED1, LOW);
}

//Play exit tune on piezo, flash LED2 at same time.
void exitChime(){
    tone(buzzerPin, 523); 
    digitalWrite(LED2, HIGH);
    delay(300);
    tone(buzzerPin, 330);
    digitalWrite(LED2, LOW);
    delay(300);
    tone(buzzerPin, 290);
    digitalWrite(LED2, HIGH);
    delay(300);
    tone(buzzerPin, 523); 
    digitalWrite(LED2, LOW);
    delay(700);
    noTone(buzzerPin);
    digitalWrite(LED2, HIGH);
    delay(2000);
    digitalWrite(LED2, LOW);
}

//Play no seat chime on piezo, flash LED1 at same time.
void noSeatChime(){
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 330);
    digitalWrite(LED1, HIGH); 
    delay(300);
    tone(buzzerPin, 290);
    digitalWrite(LED1, LOW);
    delay(300);
    noTone(buzzerPin);
}
    digitalWrite(LED1, HIGH);
    delay(2000);
    digitalWrite(LED1, LOW);
}

//Change available seats statement depending on current occupancy.
void availableSeatsFunction() {
  lcd.setCursor(0, 1);                //move the cursor to the first space of the bottom row
  lcd.print("( O .O)/ There ");       //print the number of seconds that have passed since the last reset
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
