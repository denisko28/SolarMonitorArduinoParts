#include <SimpleModbusSlave.h>

#define TOTAL_REGS_SIZE 17
#define TOTAL_ERRORS TOTAL_REGS_SIZE - 1

/* This example code has 9 holding registers. 6 analogue inputs, 1 button, 1 digital output
   and 1 register to indicate errors encountered since started.
   Function 5 (write single coil) is not implemented so I'm using a whole register
   and function 16 to set the onboard Led on the Atmega328P.

   The modbus_update() method updates the holdingRegs register array and checks communication.

   Note:
   The Arduino serial ring buffer is 128 bytes or 64 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.

   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 122 bytes or 61 registers.

   In a function 16 request the master will attempt to write to your
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS,
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   118 bytes or 59 registers.

   Using the FTDI USB to Serial converter the maximum bytes you can send is limited
   to its internal buffer which is 60 bytes or 30 unsigned int registers.

   Thus:

   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 54 bytes or 27 registers.

   In a function 16 request the master will attempt to write to your
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS,
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   50 bytes or 25 registers.

   Since it is assumed that you will mostly use the Arduino to connect to a
   master without using a USB to Serial converter the internal buffer is set
   the same as the Arduino Serial ring buffer which is 128 bytes.
*/


// Using the enum instruction allows for an easy method for adding and
// removing registers. Doing it this way saves you #defining the size
// of your slaves register array each time you want to add more registers
// and at a glimpse informs you of your slaves register layout.

unsigned int holdingRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

void setup()
{
    /* parameters(long baudrate,
                  unsigned char ID,
                  unsigned char transmit enable pin,
                  unsigned int holding registers size,
                  unsigned char low latency)

       The transmit enable pin is used in half duplex communication to activate a MAX485 or similar
       to deactivate this mode use any value < 2 because 0 & 1 is reserved for Rx & Tx.
       Low latency delays makes the implementation non-standard
       but practically it works with all major modbus master implementations.
    */
    randomSeed(analogRead(1));
    modbus_configure(9600, 1, 2, 2050+TOTAL_REGS_SIZE, 0);
}

uint16_t comissionValue = 52618;
unsigned long up_time = 0;
bool first = true;

void collectEnergy()
{
    // COMISSIONING VALUE
    comissionValue += random(0, 3);
    holdingRegs[0] = (comissionValue >> 16) & 0xFFFF;
    holdingRegs[1] = (comissionValue >> 0) & 0xFFFF;

    uint16_t value;

    // ENERGY BY DAYS 1-7
    for(int i = 0; i < 7; i++)
    {
      value = random(10, 16);
      holdingRegs[2 + i * 2] = (value >> 16) & 0xFFFF;
      holdingRegs[2 + i * 2 + 1] = (value >> 0) & 0xFFFF;
    }
}

void loop()
{
    // modbus_update() is the only method used in loop(). It returns the total error
    // count since the slave started. You don't have to use it but it's useful
    // for fault finding by the modbus master.
    holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);

    //delay 10 minutes
    if(first == true || millis() - up_time > 600000)
    {
      collectEnergy();
      first = false;
      up_time = millis();
    }
}
