#include <stdio.h>
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

/* TODO: define Client member functions */

Client::Client(char* arg1, int arg2)
{
  hostname = arg1;
  portno = arg2;
  buffer = (char *) mmap(NULL, sizeof(char) * 256, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  price = (double *) mmap(NULL, sizeof(double), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0 ) {
    error("ERROR opening socket");
  }
  server = gethostbyname(hostname);
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    error("ERROR connecting");
  }
}


void Client::get_price()
{
  int n;
  struct package pkg;
  n = read(sockfd, &pkg, sizeof(pkg));
  if(n < 0){
    error("ERROR reading from socket");
  }

  if (pkg.type == 0) {
    *price = pkg.gen_price;
    printf("$%0.2f %s", pkg.gen_price, ctime(&pkg.gen_time));
  }
  else if (pkg.type == 1) {
    printf("error package sent\n");
  }
  else if (pkg.type == 2) {
    printf("client buy at %0.2f at %s", pkg.response_to_price, ctime(&pkg.response_to_time));
    printf("server sell at %0.2f at %s", pkg.gen_price, ctime(&pkg.gen_time));
    printf("buy SUCCESS\n");
  }
  else if (pkg.type == 3) {
    printf("client buy at %0.2f at %s", pkg.response_to_price, ctime(&pkg.response_to_time));
    printf("server sell at %0.2f at %s", pkg.gen_price, ctime(&pkg.gen_time));
    printf("buy FAIL\n");
  }
  //if (*buf == '$') {
  //  strcpy(price_info, buf);
  //}
  //printf("%s", buf);
}

void Client::gen_buy_request()
{
  if(getchar() != '\n') return;

  sleep(1);

  struct package pkg;
  pkg = gen_pkg(1);

  int n;
  printf("from Client: client send buy request at %0.2f at %s", pkg.gen_price, ctime(&pkg.gen_time));
  n = write(sockfd, &pkg, sizeof(pkg));
  if (n < 0) error("ERROR writing into socket");
}

struct Client::package Client::gen_pkg(int arg1)
{
  struct package new_pkg;
  new_pkg.type = arg1;
  new_pkg.gen_time = time(NULL);
  new_pkg.gen_price = *price;
  return new_pkg;
}

void Client::error(const char *msg)
{
  perror(msg);
  exit(1);
}
