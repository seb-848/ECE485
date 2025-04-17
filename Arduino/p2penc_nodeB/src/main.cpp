#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <packet.cpp>
#include <diffie.h>
//#include <prng.cpp>

#define CE_PIN   7
#define CSN_PIN  8

typedef struct packet Packet;
typedef struct diffie Diffie;
// Todo: set shared secret as key
Diffie dataRecieve;
Diffie dataTransmit;

const byte Address[5] = {'N','e','s','s','y'};

RF24 radio(CE_PIN, CSN_PIN);

String username = "";
String dataInput;
char dataToSend[32];
char dataReceived[32];
byte node_A_address[6] = "NodeA";
byte node_B_address[6] = "NodeB";
int rand_bits[] = {1,0,1,0,1,1,0,0};
int parity_bits[8];
packet tx;
packet rx;
int shared_secret = 0;

void setup() {
    Serial.begin(9600);
    //Serial.println("Enter username...");

    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    Serial.setTimeout(500);
    radio.setRetries(3, 5);
    radio.openWritingPipe(node_B_address);
    radio.openReadingPipe(1, node_A_address);
    
    // TODO: add validation
    // diffie hellman
    radio.startListening();
    Serial.println("BOB IS RECEIVING");
    delay(500);
    while (radio.available() == 0) {};
    radio.read(&dataRecieve.user_num, sizeof(dataRecieve.user_num));
    shared_secret = power(dataRecieve.user_num, dataTransmit.user_num) % dataTransmit.N;
    Serial.println("Shared secret is " + (String)shared_secret);
    delay(1000);
    
    radio.stopListening();
    Serial.println("BOB IS TRANSMITTING");
    delay(3000);
    dataTransmit.output = power(dataTransmit.G, dataTransmit.user_num) % dataTransmit.N;
    radio.write(&dataTransmit.output, sizeof(dataTransmit.output));
    Serial.println("SENT TO ALICE");
    Serial.println();
    decToBinaryPrint(shared_secret);
    Serial.println("Enter username...");

    decToBinary(shared_secret, tx.sequence);
    for (int i = 0; i < 8; i++) {
      parity_bits[i] = tx.sequence[i];
    }
}

void loop() {

  // set username
  while (username == "") {
    if ( Serial.available() ) {
      username = Serial.readStringUntil('\n');
      Serial.print("Welcome ");
      Serial.println(username);
    }
  }
  // listen for radio data
  radio.startListening();
  //delay(5000);
  if ( radio.available() ) {
    // read data from radio
    radio.read( &rx.encryptedText, sizeof(rx.encryptedText) );
    for (int i = 0; i < BITS; i++) {
      rx.encryptedText[i] ^= rand_bits[i];
      
    }
    int letter = readBinaryString(rx.encryptedText);
    Serial.print((char)letter);
  }

  if( Serial.available() ) {
    // stop listening on radio
    radio.stopListening();

    // get serial input
    dataInput = "[" + username + "] " + Serial.readStringUntil('\n');
    Serial.println(dataInput);
    dataInput.toCharArray(tx.text, 10);
    //makeIntoPacket(tx.text, tx);

    for (int i = 0; i < 10; i++) {
      // get into binary string
      String(tx.text[i], BIN).toCharArray(tx.encryptedText, 8);
      int count = 0;

      // leading zeros
      for (int j = 0; j < BITS; j++) {
        if (tx.encryptedText[j] == NULL) break;
        else count++;
      }
      // fill leading zeros
      for (int j = count; j < BITS; j++) {
        tx.encryptedText[j] = '0';
      }
      
      // rotate array
      rotateRight(BITS - count, tx.encryptedText);
      for (int i = 0; i < BITS; i++) {
        tx.encryptedText[i] ^= rand_bits[i];
      }
      
      // send data
      radio.write(&tx.encryptedText, sizeof(tx.encryptedText));
    }

    //encrypt(rand_bits, parity_bits, tx);
    //delay(15000);
    // send data
    //radio.write( &tx.text, sizeof(tx.text) );

    //radio.write( &tx.encryptedText, sizeof(tx.encryptedText)); 
  }

}