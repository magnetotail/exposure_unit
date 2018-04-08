/*
  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
const int btn_up = 8, btn_down = 9, btn_cancel = 10, btn_ok = 11;
bool btn_pressed[4] = {false, false, false, false};
const int led_pin = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ENTRY_LIGHT_LENGTH 0
#define ENTRY_BEEPER 1
#define ENTRY_STATUS_BLINK_LENGTH 2
#define ENTRY_START 3

#define ENTRY_COUNT 3

enum menu_entry_name {
  light_length,
  beeper,
  status_blink_length,
  start
};

class menu_entry {
  private: menu_entry_name name;
    char *title;

  public:
    menu_entry() {}

    menu_entry(menu_entry_name name, char *title) {
      this->name = name;
      this->title = title;
    }

    boolean is_equal(menu_entry other) {
      return this->name == other.name;
    }

  public: menu_entry_name get_name() {
      return this->name;
    }

    char* get_title() {
      return this->title;
    }

};



enum direction {
  up,
  down
};

void switch_menu_entry(direction switch_direction);

void update_display(direction marker_position=up, char* marker="");
void read_btns();
//current selected menu entry
int current_entry = 0;

//booleans to show the button was already pressed
boolean button[4] = {HIGH, HIGH, HIGH, HIGH};

menu_entry* menu_entries[4];

int previous[4] = {HIGH, HIGH, HIGH, HIGH};    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int *pressed_button;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  menu_entries[ENTRY_LIGHT_LENGTH] = new menu_entry(light_length, "Dauer");
  menu_entries[ENTRY_BEEPER] = new menu_entry(beeper, "Piepser");
  menu_entries[ENTRY_STATUS_BLINK_LENGTH] = new menu_entry(status_blink_length, "Blinkdauer");
  menu_entries[ENTRY_START] = new menu_entry(start, "Starten");
  pinMode(btn_up, INPUT);
  pinMode(btn_down, INPUT);
  pinMode(btn_cancel, INPUT);
  pinMode(btn_ok, INPUT);
  pinMode(led_pin, OUTPUT);
  update_display(up, ">>>");
}

void loop() {
  read_btns();
  if(btn_pressed[0]){
    switch_menu_entry(up);
    update_display(up, ">>>");
    btn_pressed[0] = false;
  }else if(btn_pressed[1]){
    switch_menu_entry(down);
    update_display(down, ">>>");
    btn_pressed[1] = false;
  }else if(btn_pressed[2]){
    btn_pressed[2] = false;
    open_entry();
  }
}

void open_entry(){
  update_display();
  lcd.setCursor(0,1);
  lcd.print("                ");
  while(true){
    read_btns();
    if(btn_pressed[3]){
      update_display(up, ">>>");
      break;
    }else{
      handle_entry();    
    }
  }
}

void handle_entry(){
  switch(current_entry){
    
  }
}

void read_btns(){
  for (int curr_btn = btn_up, index = 0; curr_btn <= btn_ok; curr_btn++, index++) {
    button[index] = digitalRead(curr_btn);
    
    // if the input just went from LOW and HIGH and we've waited long enough
    // to ignore any noise on the circuit, toggle the output pin and remember
    // the time
    if (button[index] == LOW && previous[index] == HIGH && millis() - time > debounce) {
      btn_pressed[index] = true;
      time = millis();
    }
    if (button[index] == HIGH && previous[index] == LOW ) {
      btn_pressed[index] = false;
      time = millis();
    }
    previous[index] = button[index];
  }
}
void switch_menu_entry(direction switch_direction) {
  if (switch_direction == up) {
    current_entry = current_entry == 0 ? ENTRY_COUNT : current_entry -1;
  } else {
    current_entry = current_entry == ENTRY_COUNT ? 0 : current_entry + 1;
  }
}

void update_display(direction marker_position=up, char* marker="") {
  if (marker_position == up ) {
    lcd.clear();
    lcd.print(marker);
    lcd.print(menu_entries[current_entry % (ENTRY_COUNT + 1)]->get_title());
    lcd.setCursor(0, 1);
    lcd.print(menu_entries[(current_entry + 1) % (ENTRY_COUNT + 1)]->get_title());
  } else if (marker_position == down){
    lcd.clear();
    lcd.print(menu_entries[(current_entry + ENTRY_COUNT) % (ENTRY_COUNT + 1)]->get_title());
    lcd.setCursor(0, 1);
    lcd.print(marker);
    lcd.print(menu_entries[(current_entry) % (ENTRY_COUNT + 1)]->get_title());
  }
}
