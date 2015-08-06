#include <TimerOne.h>

#include <stdlib.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;

#define D_0_01_MM (0.01f)
#define D_0_1_MM  (0.1f)
#define D_1_MM    (1.0f)
#define D_10_MM   (10.0f)

#define INTERRUPTB 1  // that is, pin 3

typedef enum
{
  DIST_0_01_MM = 0u,
  DIST_0_1_MM,
  DIST_1_MM,
  DIST_10_MM,
  NUMBER_OF_DIST
} DistanceType;

typedef enum
{
  AXIS_X = 0x58u,
  AXIS_Y = 0x59u,
  AXIS_Z = 0x5Au
} AxisType;

AxisType SelectedAxis = AXIS_X;
uint8 SelectedDistance = 2;

enum PinAssignments {
  encoderPinA = 20,   // rigth
  encoderPinB = 21,   // left
  PushButton = 3    // another two pins
};

volatile unsigned int encoderPos = 0;  // a counter for the dial
unsigned int lastReportedPos = 1;   // change management
static boolean rotating = false;    // debounce management

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

volatile boolean fired;



void setup ()
{
  Timer1.initialize(100000);
  Timer1.attachInterrupt(AskStatusFromGrbl);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  pinMode(PushButton, INPUT);
  // turn on pullup resistors
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  digitalWrite(PushButton, HIGH);

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(2, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(3, doEncoderB, CHANGE);
  attachInterrupt (INTERRUPTB, isrp, FALLING);

  Serial1.begin(115200);
  //  Serial1.println("Grbl Serial started\r\n");
  Serial.begin (115200);
  //  Serial.println("PC Serial started\n\r");
  //  Serial1.write(0x18);
}

long position  = -999;
uint8 JoggingEnabled = 0;
uint8 AskStatusFromGrblFlag = 0;

void loop ()
{
  String grblString = String("");

  if (fired)
  {
    fired = false;
    if (SelectedDistance < DIST_1_MM)
    {
      SelectedDistance++;
    }
    else
    {
      SelectedDistance = DIST_0_01_MM;
    }
    Serial.println ("Distance Selected: ");
    switch (SelectedDistance)
    {
      case DIST_0_01_MM:
        {
          Serial.print(D_0_01_MM);
          break;
        }
      case DIST_0_1_MM:
        {
          Serial.print(D_0_1_MM);
          break;
        }
      case DIST_1_MM:
        {
          Serial.print(D_1_MM);
          break;
        }
      case DIST_10_MM:
        {
          Serial.print(D_10_MM);
          break;
        }
      default:
        {
          /* do nothing */
        }
    }
    Serial.print("\r\n");
  }

  rotating = true;  // reset the debouncer
  long newPos;

  if (JoggingEnabled == 1)
  {
//    Timer1.start();
    if (AskStatusFromGrblFlag == 1)
    {
      Serial1.println("?");
      AskStatusFromGrblFlag = 0;
    }

    if (lastReportedPos != encoderPos)
    {
      switch (SelectedAxis)
      {
        case AXIS_X:
          {
            if (lastReportedPos < encoderPos)
            {
              grblString =  "G91G1X";
            }
            else
            {
              grblString =  "G91G1X-";
            }

            break;
          }
        case AXIS_Y:
          {
            if (lastReportedPos < encoderPos)
            {
              grblString =  "G91G1Y";
            }
            else
            {
              grblString =  "G91G1Y-";
            }
            break;
          }
        case AXIS_Z:
          {
            if (lastReportedPos < encoderPos)
            {
              grblString = "G91G1Z";
            }
            else
            {
              grblString =   "G91G1Z-";
            }
            break;
          }
      }
      switch (SelectedDistance)
      {
        case DIST_0_01_MM:
          {
            grblString += "0.01";
            break;
          }
        case DIST_0_1_MM:
          {
            grblString += "0.1";
            break;
          }
        case DIST_1_MM:
          {
            grblString += "1.0";
            break;
          }
        case DIST_10_MM:
          {
            grblString += "10.0";
            break;
          }
        default:
          {
            /* do nothing */
          }
      }
      //    strcat(uartStr, grblString);

      grblString += "F400\r\n";


      //    Serial1.print(uartStr);
      Serial1.print(grblString);


      lastReportedPos = encoderPos;
    }
  }
  else
  {
//    Timer1.stop();
  }
}



void AskStatusFromGrbl(void)
{
  AskStatusFromGrblFlag = 1;
}

uint8 GrblCommand = 1;
uint8 uart_str_cnt = 0;
uint8 new_data = 0;
#define MAXUARTSTR 250
uint8 uart_str[MAXUARTSTR] = "";
uint8 block_finished = 0;
uint8 new_data_index = 0;
uint8 JoggingCommandFound = 0;

void serialEvent()
{
  uint8 next_char;
  next_char = Serial.read();
  //  Serial1.write(next_char);

  /* Tell SW that new command has been detected */
  if (next_char == ':')
  {
    uart_str_cnt = 0;
    new_data = 1;
    new_data_index = 0;
    block_finished = 0;
    GrblCommand = 0;
    //    Serial.print("New Block Detected\r\n");
  }

  /* Tell SW that end of command has been detected */
  if (next_char == '*')
  {
    block_finished = 1;
    //    Serial.print("Block Finished\r\n");
  }

  /* Save incoming data in buffer */
  if (new_data == 1 && block_finished == 0)
  {
    uart_str[uart_str_cnt] = next_char;
  }


  if (block_finished == 1)
  {
    //    Serial.print("Block Finished and Executing\r\n");
    uart_str[uart_str_cnt] = '\0';
    uart_str_cnt = 0;
    block_finished = 0;
    new_data = 0;
    //        if (uart_str[1] == 'J')
    {
      JoggingEnabled = uart_str[2] - 0x30;
      if (JoggingEnabled == 1)
      {
        Serial.println("Jogging Mode Enabled");
      }
      else
      {
        Serial.println("Jogging Mode Disabled");
      }
    }
  }
  else
  {
    uart_str_cnt++;
  }

  if (JoggingEnabled == 0)
  {
    //    Serial.write(next_char);
    Serial1.write(next_char);
  }
}

void serialEvent1()
{
  uint8 data = Serial1.read();
  Serial.write(data);
}

// Interrupt on A changing state
void doEncoderA() {
  // debounce
  if ( rotating ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change?
  if ( digitalRead(encoderPinA) != A_set ) { // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set )
      encoderPos += 1;

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB() {
  if ( rotating ) delay (1);
  if ( digitalRead(encoderPinB) != B_set ) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if ( B_set && !A_set )
      encoderPos -= 1;

    rotating = false;
  }
}


void isrp ()
{
  if (!digitalRead (PushButton))
    fired = true;
}



