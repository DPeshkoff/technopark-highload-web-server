// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include "receiver.h"

#include <stdio.h>
#include <unistd.h>

#define MAX_CLIENT_BUFFER 128

/**
 * @see receiver.h
 */
int accept_client(int sockfd) {
  char *buffer = malloc(sizeof(char) * MAX_CLIENT_BUFFER);
  int nbytes = 0;
  int nread;
  struct HTTP_Header header;

  nread = read(sockfd, buffer, MAX_CLIENT_BUFFER);
  if (nread == 0) {
    puts("error reading from socket");
    send_header(sockfd, NULL, "404", NULL, 0);
    printf("Empty request %s\n", "404");
    close(sockfd);
    free(buffer);
    return -1;
  }

  nbytes = nread;
  if (strstr(buffer, "\r\n\r\n") == NULL) {
    while (nread > 0) {
      buffer = (char *)realloc(buffer, nbytes + MAX_CLIENT_BUFFER);
      nread = read(sockfd, buffer + nbytes, MAX_CLIENT_BUFFER);
      nbytes += nread;
    }
  }
  if (nread == -1) {
    send_header(sockfd, NULL, "404", NULL, 0);
    close(sockfd);
    free(buffer);
    return -1;
  }

  buffer[nbytes + 1] = '\0';
  int err = 0;
  err = parse_header(buffer, &header);
  if (err == -1) {
    send_header(sockfd, NULL, "404", NULL, 0);
    close(sockfd);
    free(buffer);
    return -1;
  }
  if (err == -2) {
    send_header(sockfd, NULL, "403", NULL, 0);
    close(sockfd);
    free(buffer);
    return -1;
  }

  send_response(sockfd, &header);
  close(sockfd);
  free(buffer);
  return 0;
}

/**
 * @see receiver.h
 */
int parse_header(char *buff, struct HTTP_Header *header) {
  char *tmp;
  char section[strlen(buff)];
  strcpy(section, buff);

  tmp = strstr(section, " ");
  if (tmp == NULL) {
    return -1;
  }
  memcpy(header->method, section, strlen(buff) - strlen(tmp));
  header->method[strlen(buff) - strlen(tmp)] = '\0';

  tmp = strstr(buff, "HTTP/");
  if (tmp == NULL) {
    return -1;
  }
  memcpy(header->filename, section + strlen(header->method) + 1,
         strlen(buff) - strlen(header->method) - strlen(tmp) - 2);
  header->filename[strlen(buff) - strlen(header->method) - strlen(tmp) - 2] =
      '\0';

  if (strstr(header->filename, "../")) {
    return -2;
  }

  if (strchr(header->filename, '%') != NULL) {
    char *decoded_url;
    decoded_url = url_decode(header->filename);
    strcpy(header->filename, decoded_url);
    free(decoded_url);
  }

  tmp = strchr(header->filename, '?');
  if (tmp != NULL) {
    memcpy(header->filename, header->filename,
           strlen(header->filename) - strlen(tmp));
    header->filename[strlen(header->filename) - strlen(tmp)] = '\0';
  }

  if (header->filename[strlen(header->filename) - 1] == '/') {
    if (strchr(header->filename, '.')) {
      return -1;
    }
    strcat(header->filename, "index.html");
  }

  tmp = strrchr(header->filename, '.');
  if (tmp != NULL) {
    strcpy(header->content_type, get_content_type(tmp + 1));
  }

  return 0;
}

/**
 * @see receiver.h
 */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/**
 * @see receiver.h
 */
char *url_decode(char *str) {
  char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') {
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';

  return buf;
}
