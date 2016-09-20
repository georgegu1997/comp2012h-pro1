#ifndef PROJ1_H
#define PROJ1_H

/* TODO: Server and Client classes */


class Server
{
public:
  Server(); // constructor
  int sock(); //create a sockfd and bing it to the server address. It return a int sockfd.
  void gen_price(int newsockfd); //function broadcasting the price every second
  void process_buy_request(int newsockfd); //function handling all packages received
private:
  struct package {
    int type;
    time_t gen_time;
    float gen_price;
    time_t response_to_time;
    float response_to_price;
  };
  //This ts the package structure used to transmit infomation. Every connection is via package.
  //type == 0: broadcast price
  //type == 1: buy request
  //type == 2: buy Success
  //type == 3: buy Fail
  double* price;
  int* buy_count;
  int sockfd, portno, gen_interval, change_interval;
  struct sockaddr_in serv_addr;
  time_t* last_change;
  void error(const char *msg); //function hanling error message
  void change_price(); //function used to change the mmap variable price
  struct package gen_pkg(int arg1); //function used to generate a package used to broadcast price infomation
  struct package gen_response_pkg(int arg1, struct package arg2);  //function to generate a package used to respond to the buy request
  //char* concat_str(const char* str1, const char* str2);
};

class Client
{
public:
  Client(char* arg1, int arg2); // constructor
  void get_price(); // Actually handle all the packages received, sort out the infomation, and print the corresponding infomation
  void gen_buy_request(); //generate buy request with the wanted price, and send it to the server
private:
  struct package {
    int type;
    time_t gen_time;
    float gen_price;
    time_t response_to_time;
    float response_to_price;
  };
  double *price;
  char *hostname;
  int sockfd, portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  void error(const char *msg); //handling error messages
  struct package gen_pkg(int arg1); //function to generate package including the wanted price and current time
};

#endif
