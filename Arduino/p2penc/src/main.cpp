// // #include <SPI.h>
// // #include <nRF24L01.h>
// // #include <RF24.h>

// // #define CE_PIN   7
// // #define CSN_PIN 8

// // //const byte Address[5] = {'N','e','s','s','y'};

// // RF24 radio(CE_PIN, CSN_PIN);

// // String username = "";
// // String dataInput;
// // char dataToSend[32];
// // char dataReceived[32];

// // byte node_A_address[6] = "NodeA";
// // byte node_B_address[6] = "NodeB";
// // //int status = 0;

// // void setup() {
// //     Serial.begin(115200);
// //     Serial.println("Enter username...");

// //     radio.begin();
// //     radio.setChannel(115); 
// //     radio.setPALevel(RF24_PA_MAX);
// //     radio.setDataRate(RF24_250KBPS);
// //     //radio.setRetries(3, 5);
// //     Serial.setTimeout(10000);
// //     radio.openWritingPipe(node_B_address);
// //     radio.openReadingPipe(1, node_A_address);
// // }

// // void loop() {

// //   // set username
// //   while (username == "") {
// //     if ( Serial.available() ) {
// //       username = Serial.readStringUntil('\n');
// //       Serial.print("Welcome ");
// //       Serial.println(username);
// //     }
// //   }

// //   // listen for radio data
// //   radio.startListening();

// //   if ( radio.available() ) {
// //     // read data from radio
// //     radio.read( &dataReceived, sizeof(dataReceived) );
// //     Serial.println(dataReceived);  
// //   }

// //   if( Serial.available() ) {
// //     // stop listening on radio
// //     radio.stopListening();

// //     // get serial input
// //     dataInput = "[" + username + "] " + Serial.readStringUntil('\n');
// //     Serial.println(dataInput);
// //     dataInput.toCharArray(dataToSend, 10);

// //     // send data
// //     radio.write( &dataToSend, sizeof(dataToSend) );  
// //   }

// // }


// // NEW PROGRAM BELOW
// #include <Arduino.h>
// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>
// #include <packet.cpp>
// #include <prng.cpp>

// #define CE_PIN    7
// #define CSN_PIN   8

// RF24 radio(CE_PIN, CSN_PIN);

// String username = "";
// String dataInput;
// char dataToSend[TEXT_SIZE];
// char dataReceived[TEXT_SIZE];

// byte node_A_address[6] = "NodeA";
// byte node_B_address[6] = "NodeB";
// int rand_bits[] = {1,0,1,0,1,1,0,0};
// int parity_bits[] = {1,1,1,1,0,0,0,0};
// //int status = 0;

// struct packet rec_packet;
// struct packet user_packet;

// void setup() {
//     Serial.begin(115200);
//     Serial.println("Enter username...");

//     radio.begin();
//     radio.setChannel(115); 
//     radio.setPALevel(RF24_PA_MAX);
//     radio.setDataRate(RF24_250KBPS);
//     //radio.setRetries(3, 5);
//     Serial.setTimeout(10000);
//     radio.openWritingPipe(node_B_address);
//     radio.openReadingPipe(1, node_A_address);
// }

// void loop() {

//   // set username
//   while (username == "") {
//     if ( Serial.available() ) {
//       username = Serial.readStringUntil('\n');
//       Serial.print("Welcome ");
//       Serial.println(username);
//     }
//   }

//   // listen for radio data
//   radio.startListening();
//   //struct packet rec_packet;
//   if ( radio.available() ) {
//     // read data from radio
//     //radio.read( &dataReceived, sizeof(dataReceived) );
//     //radio.read(&rec_packet.encryptedText, sizeof(rec_packet.encryptedText));
//     //radio.read(&rec_packet.sequence, sizeof(rec_packet.sequence));
//     radio.read(&rec_packet.text, sizeof(rec_packet.text));
//     // for (int i = 0; i < TEXT_SIZE * 4; i++) {
//     //   Serial.print(rec_packet.encryptedText[i]);
//     // }
//     // Serial.print('\n');

//     // for (int i = 0; i < TEXT_SIZE * 4; i++) {
//     //   Serial.print(rec_packet.sequence[i]);
//     // }
//     // Serial.print('\n');

//     // for (int i = 0; i < TEXT_SIZE; i++) {
//     //   Serial.print(rec_packet.text[i]);
//     // }
//     // Serial.print('\n');
//     Serial.println(rec_packet.text);

//     //Serial.println(dataReceived);  
//   }

//   if( Serial.available() ) {
//     // stop listening on radio
//     radio.stopListening();

//     // get serial input
//     dataInput = "[" + username + "] " + Serial.readStringUntil('\n');
//     Serial.println(dataInput);
//     dataInput.toCharArray(dataToSend, TEXT_SIZE);
//     //struct packet user_packet;

//     //makeIntoPacket(dataToSend, user_packet);

//     int count = 0;
//     char c[4];

//     // fill text
//     for (int i = 0 ; i < TEXT_SIZE; i++) {
//         user_packet.text[i] = dataToSend[i];
//     }
    
//     // fill encrypted array with binary
//     for (int i = 0; i < TEXT_SIZE; i++) {
//         itoa(user_packet.text[i], c, 2);
//         for (int j = 0; j < 4; j++) {
//             user_packet.encryptedText[count] = c[j] - '0';
//             count++;
//         }
//     }







//     Serial.print(user_packet.text);
//     for (int i = 0; i < TEXT_SIZE * 4; i++) {
//       user_packet.encryptedText[i] ^= rand_bits[BITS - 1];
//       user_packet.sequence[i] = rand_bits[BITS - 1];
//       shiftRegister(rand_bits, parity_bits);
//     }
//     delay(500);
    
//     // send data
//     //radio.write( &dataToSend, sizeof(dataToSend) );  
//     //radio.write(&user_packet.encryptedText, sizeof(user_packet.encryptedText));
//     //radio.write(&user_packet.sequence, sizeof(user_packet.sequence));
//     radio.write(&user_packet.text, sizeof(user_packet.text));
//   }
// }















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
int shared_secret;
char decryptedChar[8];

void setup() {
    Serial.begin(9600);
    //Serial.println("Enter username...");

    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    Serial.setTimeout(500);
    radio.setRetries(3, 5);
    radio.openWritingPipe(node_A_address);
    radio.openReadingPipe(1, node_B_address);

    // TODO: add validation
    // diffie hellman
    radio.stopListening();
    Serial.println("ALICE IS TRANSMITTING");
    delay(3000);
    dataTransmit.output = power(dataTransmit.G, dataTransmit.user_num) % dataTransmit.N;
    radio.write(&dataTransmit.output, sizeof(dataTransmit.output));
    Serial.println("SENT TO BOB");

    radio.startListening();
    Serial.println("ALICE IS RECEIVING");
    delay(500);
    while (radio.available() == 0) {};
    radio.read(&dataRecieve.user_num, sizeof(dataRecieve.user_num));
    shared_secret = power(dataRecieve.user_num, dataTransmit.user_num) % dataTransmit.N;
    Serial.println("Shared secret is " + (String)shared_secret);
    delay(1000);
    Serial.println();
    decToBinaryPrint(shared_secret);
    Serial.println("Enter username...");
    radio.stopListening();

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
  }

}
