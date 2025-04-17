#include <Arduino.h>

uint64_t power(uint32_t number,uint32_t exponent){
  uint64_t result =1;
  for(uint32_t i=0; i<exponent; i++)result*=number;
  return result;
}


struct diffie {
  int id = 0;
  int user_num = 6; // bob random
  int G = 5; // publicly known
  int N = 23; // publicly known
  int output = 0;//power(G, user_num) % N;
  int shared_secret = 0;
};