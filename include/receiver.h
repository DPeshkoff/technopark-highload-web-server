// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#pragma once

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "http.h"
#include "sender.h"
#include "task.h"

/**
 * @brief принимаем клиента и читаем, что он нам прислал такого интересного
 * @param sockfd дескриптор сокета
 */
int accept_client(int sockfd);

/**
 * @brief парсим заголовки
 * @param buffer буффер заголовка
 * @param header заголовок
 * @return числовой код статуса (0: OK, 1: 404, 2: 403)
 */
int parse_header(char *buffer, struct HTTP_Header *header);

/**
 * @brief декодируем юрл
 * @param str строка запроса
 */
char *url_decode(char *str);
