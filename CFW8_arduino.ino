/*
SBIG CFW8 filter wheel controler By Ian Behar
Published under Creative Commons Zero v1.0 Universal liscence
french-toast74
Ian Behar March 2020
Version 1.0
*/


#include <U8g2lib.h>
#include <Wire.h>

#define BUTTON_PIN 4
#define PULSE_OUT_PIN 12
#define MOVE_COMPLETE_PIN 10

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
int wheel_position = 1;
int selected_wheel_position = 1;
char *filter_color[] = {"Red", "Green", "Blue","Lumin.", "Clear"};
bool button_state = 0; 
bool last_button_state = 0;
bool move_flag = 0;
unsigned long next_millis = 0;
bool moving_state = 1;
bool last_moving_state = 1;
bool error_flag = 0;



void setup() {

  // put your setup code here, to run once:

u8g2.begin();
u8g2.setFont(u8g2_font_luBIS18_tf);  // choose a suitable font
pinMode(PULSE_OUT_PIN, OUTPUT); // Pin 2 on CFW 8 input for timing pulses
pinMode(BUTTON_PIN, INPUT); // Button Pin
pinMode(MOVE_COMPLETE_PIN, INPUT); //Pin 1 on CFW 8 output active LOW indicates movement of filter wheel
Serial.begin(9600); //DEBUG
// on power up CFW 8 initializes to position 1 and makes a movment

next_millis = millis() + 8000; // setup 8 sec timeout 
while(millis() < next_millis){ 
  moving_state = digitalRead(MOVE_COMPLETE_PIN); //read if wheel is moving 
  if(moving_state != last_moving_state ){ 
    if(moving_state == HIGH){           // if high going pulse from movement signal
      print_to_screen(filter_color[wheel_position-1], 0, 26); // print inital filter color
      Serial.println(filter_color[wheel_position-1]); // DEBUG 
      error_flag = 0; // no errors go to void loop clears error/init condition
      next_millis = millis() - 1000; // init complete exit timeout loop (does this even work ?)
      }
    else{print_to_screen("init.",0,26);error_flag = 1;Serial.println("Init.");} // prints init while movemnt in process or max 8 sec. error flag will stay up is movment never completes
    }
    last_moving_state = moving_state;
   
  }
  if(error_flag){print_to_screen("ERROR!",0,26);Serial.println("ERROR!");} // no movement prints error void loop has an infinite loop to prevent button presses
  if(!error_flag){}; // no errors/ initialized go to void loop
  
}



void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(PULSE_OUT_PIN, LOW); // keep filter wheel command pin low while not in use (just in case)
//if(error_flag){while(1){}} //enable for release!  inifinite loop is error during initializtion prevents button presses. 
delay(50);
selected_wheel_position = wheel_position; //prevents making movment when selecting the posstion the wheel is already in

next_millis = millis() + 4000; // 4 secs to select filter, after each press an extra secon is added before movement
while (millis() < next_millis){
button_state = digitalRead(BUTTON_PIN);
if(button_state != last_button_state){
  if(button_state == HIGH){
    move_flag = 1;
    wheel_position++;
    if (wheel_position > 5){ //Reset position counter to not exeed 5
    wheel_position = 1;
    }
    print_to_screen(filter_color[wheel_position-1], 0, 26);
    Serial.println(filter_color[wheel_position-1]); // DEBUG 
    next_millis = millis() + 1000;  // gives an extra second if button is pressed before movement
  }
  else{   }

  delay(50);
  }
last_button_state = button_state;


}

  if(wheel_position == selected_wheel_position){move_flag = 0;} // don't move if you select the same position

  if(move_flag == 1){
    print_to_screen("Moving", 0,26); // prints moving (print again below but it takes some time, if theres an error it will go to error)
    Serial.println("Moving"); //DEBUG
    filter_wheel_position(wheel_position, PULSE_OUT_PIN); // Move the filter wheel!
    
    next_millis = millis() + 8000; // setup 8 sec timeout
    while(millis() < next_millis){ 
      moving_state = digitalRead(MOVE_COMPLETE_PIN); //read if wheel is moving 
      if(moving_state != last_moving_state ){ 
        if(moving_state == HIGH){           // if high going pulse from movement signal
        //print_to_screen(filter_color[wheel_position-1], 0, 26); // print filter color
        error_flag = 0; // no errors go to void loop clears error/init condition
        next_millis = millis() - 1000; // movement complete exit timeout loop 
        }
      else{print_to_screen("Moving",0,26);error_flag = 1;Serial.println("Moving"); } // prints moving while movemnt in process or max 8 sec. error flag will stay up is movment never completes.
      }
      last_moving_state = moving_state;
   
    }
    if(error_flag){print_to_screen("ERROR!",0,26);Serial.println("Moving");}
    if(!error_flag){print_to_screen(filter_color[wheel_position-1], 0, 26);Serial.println(filter_color[wheel_position-1]);} // print filter color} // no errors proceed!
    
  } 
  
move_flag = 0; // clear moving message flag
  }
    
  //while(digitalRead(MOVE_COMPLETE_PIN) == LOW){ // locks code until move pin goes low
  //  }


//pwm_55(+Width in us, pin) 
void pwm_55(unsigned long wt, int pin){
  digitalWrite(pin, HIGH);
  delayMicroseconds(wt); // wt must be less than 2000us  
  digitalWrite(pin, LOW); 
  delay(16); //delayMicrosends can't be too long
  delayMicroseconds(2000-wt);
  }

//filter_wheel_position( selected position 1 through 5 , output pin)  
void filter_wheel_position(int ps, int output_pin){
  unsigned long wt[5]={500, 800, 1100, 1400, 1700}; //500us filter 1, 800us filter 2, 1100us filter 3, 1400us filter 4, 1700us filter 5
  unsigned long next_micros = micros() + 1000000;
  while (micros() < next_micros) // repeat pwm_55 funtion for 1000000us (1s)
  {
    pwm_55(wt[ps-1], output_pin);
   }
}

void print_to_screen(char msg[], int x, int y){
  u8g2.clearBuffer();// clear the internal memory
    u8g2.setCursor(x,y);
    u8g2.print(msg);
    u8g2.sendBuffer();         
}
