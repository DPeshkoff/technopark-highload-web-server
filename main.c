// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include <netinet/in.h>

#include <stdbool.h> // bool
#include <stddef.h>  // size_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // sysconf

#include "pthreads.h"

#define MAX_CLIENTS 500

// Глобальный мьютекс
pthread_mutex_t mutex_queue;

// Глобальная переменная состояния
pthread_cond_t cond_queue;

// Глобальная очередь
server_task_queue queue;

int main(int argc, char **argv) {

  int server_sockfd, client_sockfd;
  int server_port = 8000;
  int err;

  queue.length = 0;

  size_t number_of_threads = sysconf(_SC_NPROCESSORS_ONLN);

  struct sockaddr_in server_sockaddr, client_sockaddr;
  socklen_t size;

  server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (server_sockfd == -1) {
    perror("[ERROR] Failed to create socket");
    return EXIT_FAILURE;
  }

  err = setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1},
                   sizeof(int));
  if (err == -1) {
    close(server_sockfd);
    perror("[ERROR] Failed to set socket options");
    return EXIT_FAILURE;
  }

  memset(&server_sockaddr, 0, sizeof(server_sockaddr));
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(server_port);
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  err = bind(server_sockfd, (struct sockaddr *)&server_sockaddr,
             sizeof(server_sockaddr));

  if (err == -1) {
    close(server_sockfd);
    perror("[ERROR] Failed to bind server");
    return EXIT_FAILURE;
  }

  printf("Server is listening port: %d\n", server_port);
  printf("Number of threads: %ld\n", number_of_threads);

  err = listen(server_sockfd, MAX_CLIENTS);

  if (err == -1) {
    close(server_sockfd);
    perror("[ERROR] Failed to listen");
    return EXIT_FAILURE;
  }

  pthread_t threads[number_of_threads];
  pthread_mutex_init(&mutex_queue, NULL);
  pthread_cond_init(&cond_queue, NULL);

  create_threads(number_of_threads, threads);

  while (true) {
    size = sizeof(client_sockaddr);
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_sockaddr, &size);

    if (client_sockfd != -1) {
      server_task *task = (server_task *)malloc(sizeof(server_task));
      task->sockfd = client_sockfd;
      push_task(task);
    }
  }

  join_threads(number_of_threads, threads);

  pthread_mutex_destroy(&mutex_queue);
  pthread_cond_destroy(&cond_queue);
  close(server_sockfd);

  return EXIT_SUCCESS;
}
