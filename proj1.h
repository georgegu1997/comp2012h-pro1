#ifndef PROJ1_H
#define PROJ1_H

/* TODO: Server and Client classes */


class Server
{
public:
  Server();
  int sock();
  void gen_price(int newsockfd);
  void process_buy_request(int newsockfd);
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
  char buffer[256];
  struct sockaddr_in serv_addr;
  time_t* last_change;
  void counter();
  void error(const char *msg);
  void change_price();
  struct package gen_pkg(int arg1);
  struct package gen_response_pkg(int arg1, struct package arg2);
  //char* concat_str(const char* str1, const char* str2);
};

class Client
{
public:
  Client(char* arg1, int arg2);
  void get_price();
  void gen_buy_request();
private:
  struct package {
    int type;
    time_t gen_time;
    float gen_price;
    time_t response_to_time;
    float response_to_price;
  };
  char *buffer, *price_info;
  double *price;
  char *hostname;
  int sockfd, portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  void error(const char *msg);
  struct package gen_pkg(int arg1);
};

#endif
