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

void Server::gen_price(int newsockfd)
{
  int n;
  struct package pkg = gen_pkg(0);// create a package to broadcast price infomation

  if (pkg.gen_time - *last_change >= change_interval) {
    change_price();
  }
  //the price will change in every change_interval seconds.

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
  //the pakcage content the message of the puchase request.
  return new_pkg;
}

void Server::process_buy_request(int newsockfd)
{
  int n;
  struct package rcv_pkg;
  struct package send_pkg;

  n = read(newsockfd, &rcv_pkg, sizeof(rcv_pkg));

  sleep(1);

  if (n<0) error("ERROE reading from socket");

  if (rcv_pkg.type == 1) {
    *buy_count += 1;
    if ((rcv_pkg.gen_price - *price) < 1e-6) {
      send_pkg = gen_response_pkg(2, rcv_pkg);
    }
    else {
      send_pkg = gen_response_pkg(3, rcv_pkg);
    }
    n = write(newsockfd, &send_pkg, sizeof(send_pkg));
    printf("total puchases: %d\n", *buy_count);
  }
}

void Server::change_price()
{
  srand((unsigned)time(NULL));
  *price = rand() / double(RAND_MAX);
  *last_change = time(NULL);
}

void Server::error(const char *msg)
{
  perror(msg);
  exit(1);
}
