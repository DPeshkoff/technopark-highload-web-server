// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#pragma once

#include <pthread.h>

// Задача
typedef struct server_task
{
    int sockfd;
    struct server_task *next;
} server_task;

// Очередь задач
typedef struct server_task_queue
{
    server_task *head;
    server_task *tail;
    size_t length;
} server_task_queue;

/**
 * @brief добавляем задачу в очередь
 * @param task указатель на задачу
 */
void push_task(server_task *task);

/**
 * @brief забираем начальный элемент очереди
 * @returns указатель на первую задачу
 */
server_task *pop_task();

/**
 * @brief взять content-type 
 * @param file_type расширение файла
 */
char *get_content_type(char const *file_type);
