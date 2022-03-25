// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include "task.h"

#include <string.h>

extern pthread_mutex_t mutex_queue;
extern pthread_cond_t cond_queue;
extern server_task_queue queue;

char *types[9][2] = {{"html", "text/html"},
                     {"js", "application/javascript"},
                     {"txt", "text/htm"},
                     {"css", "text/css"},
                     {"png", "image/png"},
                     {"jpg", "image/jpeg"},
                     {"jpeg", "image/jpeg"},
                     {"gif", "image/gif"},
                     {"swf", "application/x-shockwave-flash"}};

/**
 * @see task.h
 */
void push_task(server_task *task) {
  pthread_mutex_lock(&mutex_queue);
  if (queue.length == 0) {
    queue.head = task;
    queue.tail = task;
  } else {
    queue.tail->next = task;
    queue.tail = task;
  }
  queue.length++;
  pthread_mutex_unlock(&mutex_queue);
  pthread_cond_signal(&cond_queue);
}

/**
 * @see task.h
 */
server_task *pop_task() {
  server_task *buf = queue.head;
  if (queue.length == 1) {
    queue.head = NULL;
    queue.tail = NULL;
  } else {
    queue.head = queue.head->next;
  }

  queue.length--;
  return buf;
}

/**
 * @see task.h
 */
char *get_content_type(char const *file_type) {
  for (size_t i = 0; i < 9; ++i) {
    if (strcmp(file_type, types[i][0]) == 0) {
      return types[i][1];
    }
  }

  return "*/*";
}
