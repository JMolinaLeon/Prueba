
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>


using namespace std;
RF24 radio(22,0); 
int interruptPin = 23; // GPIO pin for interrupts
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const int min_payload_size = 4;
const int max_payload_size = 32;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char
bool role_ping_out = 1, role_pong_back = 0;
bool role = 0;

void intHandler(){

    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      uint8_t len=0;

      while (radio.available())
      {
        // Fetch the payload, and see if this was the last one.
	len = radio.getDynamicPayloadSize();
	radio.read( receive_payload, len );

	// Put a zero at the end for easy printing
	receive_payload[len] = 0;

	// Spew it
	printf("Got payload size=%i value=%s\n\r",len,receive_payload);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( receive_payload, len );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
}


int main(int argc, char** argv){


  // Print preamble:
  cout << "RF24/examples/pingpair_dyn/\n";

  // Setup and configure rf radio
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(0,15);
  radio.setPALevel(RF24_PA_MAX);
  radio.printDetails();
  cout << "Role: Pong Back, awaiting transmission " << endl << endl;
  
/***********************************/
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    radio.startListening();
    attachInterrupt(interruptPin, INT_EDGE_FALLING, intHandler); //Attach interrupt to bcm pin 23

// forever loop
	while (1)
	{

  }
}


