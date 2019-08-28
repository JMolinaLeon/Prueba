

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>


using namespace std;
RF24 radio(22,0);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char
bool role_ping_out = 1, role_pong_back = 0;
bool role = 0;

int main(int argc, char** argv){


  // Print preamble:
  cout << "RF24/examples/pingpair_dyn/\n";

  // Setup and configure rf radio
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(0,15);
  radio.setPALevel(RF24_PA_MAX);
  radio.printDetails();
  cout << "Role: Ping Out, starting transmission " << endl << endl;
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);


// forever loop
	while (1)
	{

    // The payload will always be the same, what will change is how much of it we send.
    static char send_payload[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    printf("Now sending length %i...",next_payload_size);
    radio.write( send_payload, next_payload_size );

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 500 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      uint8_t len = radio.getDynamicPayloadSize();
      radio.read( receive_payload, len );

      // Put a zero at the end for easy printing
      receive_payload[len] = 0;

      // Spew it
      printf("Got response size=%i value=%s\n\r",len,receive_payload);
    }

    // Update size for next time.
    next_payload_size += payload_size_increments_by;
    if ( next_payload_size > max_payload_size )
      next_payload_size = min_payload_size;

    // Try again 1s later
    delay(100);

}
}


