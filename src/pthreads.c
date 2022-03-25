// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include "pthreads.h"

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "receiver.h"

extern pthread_mutex_t mutex_queue;
extern pthread_cond_t cond_queue;
extern server_task_queue queue;

/**
 * @see pthreads.h
 */
void *thread_function(void *args) {
  while (true) {
    server_task *task;

    pthread_mutex_lock(&mutex_queue);
    while (queue.length == 0) {
      pthread_cond_wait(&cond_queue, &mutex_queue);
    }

    task = pop_task();
    pthread_mutex_unlock(&mutex_queue);

    int err = accept_client(task->sockfd);
    if (err != 0) {
      free(task);
      continue;
    }
    free(task);
  }
}

/**
 * @see pthreads.h
 */
void create_threads(size_t const number_of_threads, pthread_t *threads) {
  for (size_t i = 0; i < number_of_threads; ++i) {
    int err = pthread_create(&threads[i], NULL, &thread_function, NULL);
    if (err != 0) {
      perror("[ERROR] Failed to create thread");
    }
  }
}

/**
 * @see pthreads.h
 */
void join_threads(size_t const number_of_threads, pthread_t *threads) {
  for (size_t i = 0; i < number_of_threads; ++i) {
    if (pthread_join(threads[i], NULL) != 0) {
      perror("Failed while joining threads");
    }
  }
}