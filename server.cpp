#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include "proj1.h"

/* TODO: define Server member functions here */

Server::Server() // constructor
{
  portno = 5001;
  gen_interval = 1;
  change_interval = 10;
  //parameter initialization to store some basic settings

  buy_count = (int *) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  *buy_count = 0;
  last_change = (time_t *) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  *last_change = time(NULL);
  price = (double*) mmap(NULL, sizeof(double), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  change_price();
  //these pointer varaibles created by mmap will be used crossing different processes. And there they are initailized

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  //setting the address of the server
}

int Server::sock() //create a sockfd and bing it to the server address. It return a int sockfd.
{
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket");
  }

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }
  return sockfd;
}

void Server::gen_price(int newsockfd) //function broadcasting the price every second
{
  int n;

  if (time(NULL) - *last_change >= change_interval) {
    change_price();
  }
  //the price will change in every change_interval seconds.

  struct package pkg = gen_pkg(0);// create a package to broadcast price infomation

  n = write(newsockfd, &pkg, sizeof(pkg));
  if (n<0) error("ERROE writing to socket"); //write to the socket

  printf("$%0.2f at %s", pkg.gen_price, ctime(&(pkg.gen_time))); //print the infomation

  sleep(gen_interval);
}

struct Server::package Server::gen_pkg(int arg1) //function used to generate a package used to broadcast price infomation
{
  struct package new_pkg;
  new_pkg.type = arg1;
  new_pkg.gen_time = time(NULL);
  new_pkg.gen_price = *price;
  return new_pkg;
}

struct Server::package Server::gen_response_pkg(int arg1, struct package arg2) //function to generate a package used to respond to the buy request
{
  struct package new_pkg;
  new_pkg.type = arg1;
  new_pkg.gen_time = time(NULL);
  new_pkg.gen_price = *price;
  new_pkg.response_to_time = arg2.gen_time;
  new_pkg.response_to_price = arg2.gen_price;
  //the pakcage contain the message of the puchase request.
  return new_pkg;
}

void Server::process_buy_request(int newsockfd) //function handling all packages received
{
  int n;
  struct package rcv_pkg; //received package
  struct package send_pkg; //package to be sent

  n = read(newsockfd, &rcv_pkg, sizeof(rcv_pkg));
  if (n<0) error("ERROE reading from socket");

  if (rcv_pkg.type == 1) {
    *buy_count += 1;
    if ((rcv_pkg.gen_price - *price) < 1e-6) { //compare two fouble float.
      //sleep(1); //this line is totally used to simulate the delay of the network and the processing time of the computer
      send_pkg = gen_response_pkg(2, rcv_pkg); //generate package meaning buy SUCCESS
    }
    else {
      //sleep(1); //this line is totally used to simulate the delay of the network and the processing time of the computer
      send_pkg = gen_response_pkg(3, rcv_pkg); //generate package meaning buy FAIL
    }
    n = write(newsockfd, &send_pkg, sizeof(send_pkg));
    printf("total puchases: %d\n", *buy_count);
  }
}

void Server::change_price() //function used to change the mmap variable price
{
  srand((unsigned)time(NULL)); //this line is used to change the seed of rand number
  *price = rand() / double(RAND_MAX); //to ensure that price are between 0 and 1
  *last_change = time(NULL);
}

void Server::error(const char *msg) //function hanling error message
{
  perror(msg);
  exit(1);
}
