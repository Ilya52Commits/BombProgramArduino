/* Подключение библиотек */
#include <Wire.h>               // Включаем библиотеку Wire для I2C связи
#include <LiquidCrystal_I2C.h>  // Включаем библиотеку LiquidCrystal_I2C для работы с ЖК дисплеем
#include <Keypad.h>             // Включаем библиотеку Keypad для работы с клавиатурой


/* Настройка переменных и констант */
// Номера пинов переферии
const int RED_LED = 13;   // номер красного светодиода 
const int GREEN_LED = 12; // номер зелёного светодиода
const int BUZZER = 10;    // номер пина активного зумера

// Определяем размерность клавиатуры
const byte ROWS = 4; // четыре строки
const byte COLS = 4; // четыре столбца

bool isPasswordSpecified = false; // переменная для статуса указания пароля

String password = "";     // переменная для пароля
String inputString = "";  // перменная строки пароля

uint32_t currentMillis = 0;   // переменная для хранения текущего времени в миллисекундах
uint32_t previousMillis = 0;  // переменная для сохранения милисекунд после последнего события

int numberOfAttempts = 3;     // количество попыток на обезвреживание 
int countdown = 60;           // переменные для отсчета секунд

// Определяем символы клавиатуры
char arrayKeyboardCharacters[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Определяем подключение клавиатуры к пинам Arduino
byte rowPins[ROWS] = {9, 8, 7, 6}; // подключаем строки к пинам 9, 8, 7, 6
byte colPins[COLS] = {5, 4, 3, 2}; // подключаем столбцы к пинам 5, 4, 3, 2

// Создаем объект Keypad
Keypad keypad = Keypad(makeKeymap(arrayKeyboardCharacters), rowPins, colPins, ROWS, COLS);

// Инициализируем объект LiquidCrystal_I2C с I2C адресом (0x27) и размерами дисплея (16 столбцов, 2 строки)
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Функция начальных сообщений */
void StaticMessageShowMethod()
{
  lcd.setCursor(0, 0); 
  lcd.print("Timer:");
  lcd.setCursor(7, 0);
  lcd.print(countdown);

  lcd.setCursor(10, 0);
  lcd.print("Attempts: ");
  lcd.print(numberOfAttempts);
}

void IncorrectPasswordMessageMethod() 
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("The password must be");
  lcd.setCursor(0, 1);
  lcd.print("6 characters long!");
            
  delay(1500); 
            
  lcd.clear();
            
  password = ""; 
}

/* Метод стартовых настроек */
void setup() 
{    
  // Настройка переферии на выход
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
   
  // Инициализируем ЖК дисплей
  lcd.init();                                 
  lcd.backlight(); 

  if (password == "")
  { 
    while (!isPasswordSpecified) 
    {
      lcd.setCursor(0, 0);
      lcd.print("Enter the password: ");
      
      char inputSymbol = keypad.getKey(); 

      if (inputSymbol) 
      {    
        digitalWrite(BUZZER, HIGH);
        delay(50);
        digitalWrite(BUZZER, LOW);

        if (password.length() > 6) 
          IncorrectPasswordMessageMethod();
          
        if (inputSymbol == '*') 
        {
          if (password.length() != 6) 
            IncorrectPasswordMessageMethod();  
          else 
            isPasswordSpecified = true;
        }
        else 
        {        
          password += inputSymbol;
          
          lcd.setCursor(0, 1);                  
          lcd.print("You pressed: ");     
          lcd.print(password);             
          lcd.setCursor(0, 0);               
        }
      }
    }
  }
  
  lcd.clear();
  lcd.setCursor(2, 0);         
  lcd.print("BOMB IS ACTIVATE!");
  
  delay(1500);
  
  lcd.clear();
  StaticMessageShowMethod();

  // Включаем красный сетодиод для мигания
  digitalWrite(RED_LED, HIGH);
}

/* Главный цикл */
void loop() 
{
  if (countdown == 0 || numberOfAttempts == 0) 
  {
    lcd.clear();
    
    while (true) 
    {
      digitalWrite(BUZZER, HIGH);
      
      lcd.setCursor(1, 0); 
      lcd.print("YOU EXPLODED!");
    }
  }
  
  currentMillis = millis();
    
  if (((currentMillis - previousMillis ) > 250 
      || currentMillis < previousMillis) 
      && countdown <= 10 && countdown != 0) 
  {  
    digitalWrite(RED_LED, HIGH);
    delay(250);
    digitalWrite(RED_LED, LOW);
        
    digitalWrite(BUZZER, HIGH); 
    delay(250);
    digitalWrite(BUZZER, LOW);
  }
      
  // Событие срабатывающее каждые 500 мс   
  if (((currentMillis - previousMillis ) > 1000 
      || currentMillis < previousMillis) && countdown > 0) 
  {
    if (countdown > 10) 
    {
      digitalWrite(BUZZER, HIGH);
      delay(50);
      digitalWrite(BUZZER, LOW);    
      
      digitalWrite(RED_LED, HIGH);
      delay(100);
      digitalWrite(RED_LED, LOW);
    }
    
    previousMillis = currentMillis;
    countdown--;
    
    lcd.setCursor(0, 0); 
    lcd.print("Timer:");
    lcd.setCursor(7, 0);
    lcd.print(countdown);
  }
  
  char inputSymbol = keypad.getKey();              

  if (inputSymbol == '*') 
  {
   if (inputString == password) 
   {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    
    while(true) 
    {
      lcd.clear();
      delay(1000);  
      lcd.setCursor(0, 1);                
      lcd.print("You disconnect bomb!"); 
      delay(1000);  
    }
   }
   else 
   {
    delay(500);
    
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RED_LED, HIGH);
    
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("INVALID PASSWORD!"); 
    
    numberOfAttempts--;
    
    delay(1000);
    
    inputSymbol = '\0';
    inputString = "";
    
    lcd.clear();
    
    digitalWrite(BUZZER, LOW);
    digitalWrite(RED_LED, LOW);
    
    StaticMessageShowMethod();
   }
  }
  
  if (inputSymbol) 
  { 
    inputString += inputSymbol;
    
    lcd.setCursor(7, 1);            
    lcd.print(inputString);              
  }
}
