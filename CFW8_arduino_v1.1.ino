/*
SBIG CFW8 filter wheel controler By Ian Behar
Published under Creative Commons Zero v1.0 Universal liscence
french-toast74
Ian Behar March 2020
Miroslav Vukovic 2023
Version 1.1
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 4
#define PULSE_OUT_PIN 12                          // The filter wheel Pulse Input pin
#define MOVE_COMPLETE_PIN 11                      // The filter wheel returns a TTL level low-going pulse on pin 1 while it is moving
#define MAX_SERIAL_CHAR 4
#define DEBUG 1

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
int wheel_position = 1;
int selected_wheel_position = 1;
char *filter_color[] = {"Red", "Green", "Blue","Lumin.", "Clear"};       // Edit filter names as mounted on filter wheel from 1 to 5 (1 = Red if using SBIG's filter sequence set)
bool button_state = 0; 
bool last_button_state = 0;
bool move_flag = 0;
unsigned long next_millis = 0;
bool moving_state = 1;
bool last_moving_state = 1;
bool error_flag = 0;
int serialInput = 0;
char last_inSerial;


// Initialization: put your setup code here, to run once

void setup() {

display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
pinMode(PULSE_OUT_PIN, OUTPUT);                   // Pin 2 on CFW 8 input for timing pulses
pinMode(BUTTON_PIN, INPUT);                       // Button Pin
pinMode(MOVE_COMPLETE_PIN, INPUT);                // Pin 1 on CFW 8 output active LOW indicates movement of filter wheel
Serial.begin(9600);                               // Initialize serial port

display.clearDisplay();
// delay(1000);

// on power up CFW 8 initializes to position 1 and makes a movment

next_millis = millis() + 4000;                                                    // setup 4 sec timeout
while(millis() < next_millis)
{
  moving_state = digitalRead(MOVE_COMPLETE_PIN);                                  // Read if wheel is moving, LOW = wheel is moving.
    if(moving_state != last_moving_state)                                         // If moving state changes enter the if statement
    { 
      if(moving_state == HIGH)                                                    // HIGH = movement is complete
      {
        PrintScreen(filter_color[wheel_position-1], 3);
        Serial.println(filter_color[wheel_position-1]); // DEBUG 
        error_flag = 0; // no errors go to void loop clears error/init condition
        next_millis = millis() - 1000; // init complete exit timeout loop (does this even work ?)
      }

      else                                                                        // Prints Init. while movement in progress or max 4 sec. Error flag will stay up is movement never completes
      { 
        error_flag = 1;
        PrintScreen("Init.", 2);
        Serial.println("Init.");
      }
    }
    last_moving_state = moving_state;
}
  if(error_flag)                                                                  // No movement prints error void loop has an infinite loop to prevent button presses
  {
    PrintScreen("ERROR!\nCheck FW connection \nor 12V power supply!", 1);
    Serial.println("ERROR! Check FW connection or 12V power supply!");
  }
  
  if(!error_flag)                                                                 // No errors, initialized go to void loop
  { };
}                                                                                 // End of setup()


// put your main code here, to run repeatedly:

void loop()
{
  digitalWrite(PULSE_OUT_PIN, LOW);             // Keep filter wheel command pin low while not in use (just in case)
  //if(error_flag){while(1){}}                  // enable for release!  inifinite loop is error during initializtion prevents button presses. 
  delay(50);
  selected_wheel_position = wheel_position;     // Prevents making movment when selecting the position the wheel is already in

// Detection of an input either from the button or from the serial input
  SerialCheck();
  ButtonCheck();

if(wheel_position == selected_wheel_position)                       // Don't move if you select the same position
{
  move_flag = 0;
}

if(move_flag == 1)
{
    //print_to_screen("Moving", 0,26); // prints moving (print again below but it takes some time, if theres an error it will go to error)
    PrintScreen("Moving", 2);
    // Serial.println("Moving"); //DEBUG
    filter_wheel_position(wheel_position, PULSE_OUT_PIN);                                     // Move the filter wheel!
    
    next_millis = millis() + 3000;                                                            // 8 sec move timeout
    while(millis() < next_millis)
    { 
      moving_state = digitalRead(MOVE_COMPLETE_PIN);                                          // Read if wheel is moving 
      if(moving_state != last_moving_state ){ 
        if(moving_state == HIGH)                                                              // Moving is COMPLETE    
        {                                                             
          PrintScreen(filter_color[wheel_position-1], 2);                                     // Print filter color
          error_flag = 0;                                                                     // No errors go to void loop clears error/init condition
          next_millis = millis() - 1000; // movement complete exit timeout loop 
        }
        
        else                                                                                  // Prints moving while movemnt in process or max 3 sec. error flag will stay up is movment never completes.
        {
          PrintScreen("Moving", 2);
          error_flag = 1;
          // Serial.println("Moving");
        } 
      }
      last_moving_state = moving_state;
   
    }
    
    if(error_flag)
    {
      PrintScreen("Moving", 2);
      // Serial.println("Moving");
    }
    
    if(!error_flag)                                                                           // print filter color if no errors proceed!
    {
      PrintScreen(filter_color[wheel_position-1], 2);
      Serial.println(filter_color[wheel_position-1]);
    } 
    
  }

  move_flag = 0; // clear moving message flag
}                                                                 // End of loop()
    
  //while(digitalRead(MOVE_COMPLETE_PIN) == LOW){ // locks code until move pin goes low
  //  }


// Button process

void ButtonCheck()
{
  button_state = digitalRead(BUTTON_PIN);

  if(button_state != last_button_state)
  {
    if(button_state == HIGH)
    {
      move_flag = 1;
      wheel_position++;
      if (wheel_position > 5)                                             // Reset position counter to not exceed 5
      { 
        wheel_position = 1;
      }

    PrintScreen(filter_color[wheel_position-1], 2);
    Serial.println(filter_color[wheel_position-1]); // DEBUG 
    }
    delay(50);
  }

  last_button_state = button_state;
}

// Serial data process
void SerialCheck() {
  int inserial;
  char inchar;
  
  while (Serial.available() > 0) {
    inchar = Serial.read();
        
    if (inchar == '\r') {
      if(inserial == 9) {
        Serial.println(filter_color[wheel_position-1]);
      }
      else if(inserial > 0 && inserial <= 5) {
        filter_wheel_position(inserial,PULSE_OUT_PIN);
        wheel_position = inserial;
        Serial.println(filter_color[inserial-1]);
        PrintScreen("Moving",2); PrintScreen(filter_color[inserial-1],2);
      }
      else {
        Serial.println("Error");
      }
    }
    else {
      inserial = inchar - '0';
    }
  }
}

//pwm_55(+Width in us, pin) 
void pwm_55(unsigned long wt, int pin){
  digitalWrite(pin, HIGH);
  delayMicroseconds(wt); // wt must be less than 2000us  
  digitalWrite(pin, LOW); 
  delay(16); //delayMicrosends can't be too long
  delayMicroseconds(2000-wt);
  }


// Move filter wheel to selected position 1 through 5 on output pin

void filter_wheel_position(int ps, int output_pin){
  unsigned long wt[5]={500, 800, 1100, 1400, 1700}; //500us filter 1, 800us filter 2, 1100us filter 3, 1400us filter 4, 1700us filter 5
  unsigned long next_micros = micros() + 1000000;
  while (micros() < next_micros) // repeat pwm_55 funtion for 1000000us (1s)
  {
    pwm_55(wt[ps-1], output_pin);
   }
}

void PrintScreen(char msg[], int size) {
  display.clearDisplay();
  display.setTextSize(size);                  // 1 for normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);                    // Start at top-left corner
  display.println(msg);
  display.display();
}