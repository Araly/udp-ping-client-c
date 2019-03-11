// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT	 18000
#define MAXLINE 1024
#define TIMEOUT 1000
#define NUMBER_OF_TRIES 10

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "ground, ACC74, taxi to RW 22";
	struct sockaddr_in	 servaddr;

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	int n, len;
  struct timeval tv, before, after;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  int list[NUMBER_OF_TRIES], timeout = 0, min = -1, max = -1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    printf("error\n");
  }
  for (int i = 0; i < NUMBER_OF_TRIES; i++) {
    gettimeofday(&before, NULL);
    sendto(sockfd, (const char *)hello, strlen(hello),
           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));
    printf(" -> sent:\n%s\n", hello);

    if ((n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,
                      (struct sockaddr *) &servaddr,
                      &len)) < 0) {
      timeout++;
      printf("[timeout]\n");
    }
    else {
      gettimeofday(&after, NULL);
      buffer[n] = '\0';
      list[i] = ((after.tv_sec - before.tv_sec)*1000000L
                  +after.tv_usec) - before.tv_usec;
      printf(" <- received: (%ldµs)\n%s\n", list[i], buffer);
      if (min == -1 && max == -1) {
        min = list[i];
        max = list[i];
      }
      if (list[i] < min) {
        min = list[i];
      }
      if (list[i] > max) {
        max = list[i];
      }
    }
  }
  int avg = 0;
  float timeoutpercent = 100 * timeout / NUMBER_OF_TRIES;
  for (int i = 0; i < NUMBER_OF_TRIES; i++) {
    avg += list[i];
  }
  avg /= NUMBER_OF_TRIES;
  printf("recap:\nmin: %ldµs, max: %ldµs, avg: %ldµs\nwith %d out of %d timeouts, so %f %\% of the total number of attempts\n", min, max, avg, timeout, NUMBER_OF_TRIES, timeoutpercent);
	close(sockfd);
	return 0;
}
