// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#pragma once

#include <stddef.h>

#include "http.h"

/**
 * @brief отправляем заголовок ответа
 * @param sockfd дескриптор сокета
 * @param type тип содержимого
 * @param status_code код ответа HTTP
 * @param content_length длина ответа
 */
void send_header(int sockfd, char *method, char *code, char *type, int length);

/**
 * @brief отправляем тело ответа
 * @param sockfd дескриптор сокета
 * @param header заголовок
 */
void send_response(int sockfd, struct HTTP_Header *header);
