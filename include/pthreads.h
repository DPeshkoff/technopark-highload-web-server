// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#pragma once

#include "task.h"

/**
 * @brief полезная нагрузка треда
 */
void *thread_function(void *args);

/**
 * @brief создаем треды
 * @param number_of_threads количество допустимых тредов
 * @param threads массив тредов
 */
void create_threads(size_t const number_of_threads, pthread_t *threads);

/**
 * @brief соединяем треды
 * @param number_of_threads количество допустимых тредов
 * @param threads массив тредов
 */
void join_threads(size_t const number_of_threads, pthread_t *threads);
