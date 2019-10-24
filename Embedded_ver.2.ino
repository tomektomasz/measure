#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
void welcome();     //powitanie na ekranie
void codeRead();    //odczytywanie kodu z przycisku i zapisywanie w zm.glob. coding[]
char translate( );   //tlumaczenie kodu na literki
void measure();     //mierzenie odleglosci i wyswietlanie na lcd i zapisywanie w distance

int distance = 0; //zm.glob. do mierzenia odleglosci w measure()
bool end_log = false;
char coding[9] ;   //zm.globalna (kropki i kreski)
char password[10] , password2[10], cod_pass;
int i_pass = 0 , kk = 0, period = 0;
int beforeLog = 0 , afterLog = 0; //zmienne do sterowania switch-ami przed i po zalogowaniu

void setup() {
  pinMode(8, OUTPUT); //TRIG  -nadajnik
  pinMode(9, INPUT);  //ECHO  -odbiornik
  pinMode(10, INPUT); //przycisk
  pinMode(11, INPUT);
  pinMode(13, OUTPUT);  //dioda
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial1.begin(9600);
}
//=====================================
void loop() {
  if (!end_log) {
    switch (beforeLog) {
      case 0:
        welcome();
        i_pass = 0;
        beforeLog = 1;
        break;
      case 1:
        codeRead();
        if (digitalRead(11) == LOW) {
          Serial.print(coding);
          cod_pass = translate(coding);
          Serial.println(" " + cod_pass);
          password[i_pass] = cod_pass;
          i_pass++;
          lcd.print(cod_pass);
        }
        else beforeLog = 2;
        break;
      case 2:
        for (int i = 0; i < 10; i++) {
          cod_pass = EEPROM.read(i);
          if (cod_pass == '?') cod_pass = char(0);
          password2[i] = cod_pass;
          Serial.print(i);
          Serial.print(" EEPROM-");
          Serial.print(password2[i]);
          Serial.println('|');
        }
        Serial.print("odczytano z EEPROM: ");
        Serial.print(password2);
        Serial.println('|');
        if ( String(password) == String(password2) ) beforeLog = 4;
        else beforeLog = 3;
        break;
      case 3:
        lcd.noCursor();
        lcd.setCursor(0, 0);
        lcd.print("Zle haslo       ");
        lcd.setCursor(0, 1);
        lcd.print("Sprobuj ponownie");
        for (int j = 0; j < 10; j++) {
          password[j] = char(0);
        }
        delay(2000);
        beforeLog = 0;
        break;
      case 4:
        lcd.noCursor();
        Serial.print(String(password));
        lcd.setCursor(0, 0);
        lcd.print("zalogowales sie ");
        lcd.setCursor(0, 1);
        lcd.print(password);
        for (int j = 0; j < 10; j++) {
          password[j] = char(0);
        }
        delay(2000);
        end_log = true;
        break;
    }
  }
  else {      //================ TU zaczyna sie po ZALOGOWANIU =========
    switch (afterLog) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Czy chcesz       ");
        lcd.setCursor(0, 1);
        lcd.print("zmienic haslo?   ");
        afterLog = 1;
        break;
      case 1:
        if (digitalRead(10) == HIGH) afterLog = 2;
        if (digitalRead(11) == HIGH) {
          afterLog = 5;
          delay(1000);
        }
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Podaj nowe      ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        for (int i = 0; i < 10; i++) {
          EEPROM.update(i, 0);
        }
        delay(1000);
        afterLog = 3;
        kk = 0;
        break;
      case 3:
        codeRead();
        cod_pass = translate(coding);
        lcd.print(cod_pass);
        EEPROM.write(kk, cod_pass);
        Serial.println(cod_pass);
        kk++;
        if (digitalRead(11) == HIGH) afterLog = 4;
        break;
      case 4:
        for (int i = 0; i < 10; i++) {
          cod_pass = EEPROM.read(i);
          if (cod_pass == '?') cod_pass = ' ';
          password2[i] = cod_pass;
          Serial.print(i);
          Serial.print(" pass 2:");
          Serial.print(password2[i]);
          Serial.println('|');
        }
        for (int i = 0; i < 10; i++) {
          cod_pass = EEPROM.read(i);
          if (cod_pass == '?') cod_pass = char(0);
          password[i] = cod_pass;
          Serial.print(i);
          Serial.print(" pass  -");
          Serial.print(password[i]);
          Serial.println('|');
        }
        lcd.setCursor(0, 0);
        lcd.print("Nowe haslo to:   ");
        lcd.setCursor(0, 1);
        lcd.print(String(password2));
        Serial.print(String(password2));
        Serial.println("|");
        delay(1000);
        afterLog = 5;
        break;
      case 5:
        delay(1000);
        Serial1.println(" ");
        afterLog=6;
      break;
      case 6:
        measure();
        delay(30);
        period += 30;
        if (period > 2000) {
          period = 0;
          Serial1.print(distance);
          Serial1.println(" cm");
        }
        if (digitalRead(11)) afterLog = 7;

        break;
      case 7:
        lcd.setCursor(0, 0);
        lcd.print("Sensor wylaczony");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        
        Serial1.println("Sensor turned off");
        delay(1000);
        afterLog = 8;
        break;
      case 8:
        if (digitalRead(11)) afterLog = 5;
        break;

    }
  }
}
//==================================== KONIEC ======================
void welcome() {
  char zm[] = "Podaj haslo:    ";

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(16, 0);
  lcd.autoscroll();
  for (int i = 0; i < 16; i++) {
    lcd.print(zm[i]);
    delay(200);
  }
  lcd.noAutoscroll();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(zm);
  lcd.cursor();
  lcd.setCursor(0, 1);
}

void codeRead() {
  int i = 0 , tense_1 = 0 , tense_2 = 0;
  for (int j = 0; j < 8; j++) {
    coding[j] = ' ';
  }

  while (true) {
    if (digitalRead(10)) {
      tense_1 = 0;
      Serial.println(' ');
      Serial.println(digitalRead(10));
      while (digitalRead(10)) {
        tense_1 += 20;
        digitalWrite(13 , HIGH);
        delay(20);
        digitalWrite(13, LOW);
      }

      Serial.print("wew-wcisniete: ");
      Serial.println(tense_1);
      if (tense_1 < 200 ) coding[i] = '.' ;
      else if (tense_1 >= 200) coding[i] = '-' ;
      Serial.print(coding);
      Serial.println('|');
      i++;
    }
    //-----------------------------
    if (i > 7) break;
    if (!digitalRead(10) && tense_1 > 0) {
      tense_2 = 0;
      while (!digitalRead(10)) {
        tense_2 += 20;
        delay(20);
        if (tense_2 > 600 ) break;
      }
      Serial.print("wew-Pauza: ");
      Serial.println(tense_2);

    }
    if (digitalRead(11) == HIGH)   break;
    if (tense_2 > 600) break;
  }
}

char translate(char code[9]) {
  const char literki[][9] = {{"a.-     "}, {"b-...   "}, {"c-.-.   "}, {"d-..    "}, {"e.      "},
    {"f..-.   "}, {"g--.    "}, {"h....   "}, {"i..     "}, {"j.---   "}, {"k-.-    "}, {"l.-..   "},
    {"m--     "}, {"n-.     "}, {"o---    "}, {"p.--.   "}, {"r.-.    "}, {"s...    "}, {"t-      "},
    {"u..-    "}, {"w.--    "}, {"v...-   "}, {"x-..-   "}, {"y-.-    "}, {"z--..   "},
    {"1.----  "}, {"2..---  "}, {"3...--  "}, {"4....-  "}, {"5.....  "},
    {"6-....  "}, {"7--...  "}, {"8---..  "}, {"9----.  "}, {"0-----  "}
  };
  bool well;

  for (int j = 0; j < 35; j++) {
    well = true;
    for (int i = 1; i < 8; i++) {
      //if (code[i - 1] != ' ') {
      if (literki[j][i] != code[i - 1] ) well = false;
      //}
    }
    /*   Serial.print(literki[j]);
       Serial.println('|');
       Serial.print(code);
       Serial.println('|'); */
    if (well) return literki[j][0];
  }
  if (!well) return '?';

}

void measure() {
  int distance_time = 0 ;
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);
  distance_time = pulseIn(9, HIGH);
  distance = distance_time / 58;
  lcd.setCursor(0, 0);
  if (distance < 5 || distance > 200) {
    lcd.print("Pomiar poza     ");
    lcd.setCursor(0, 1);
    lcd.print("zakresem        ");
  }
  else {
    // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Odleglosc=");
    lcd.print(distance);
    lcd.print(" cm ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}
