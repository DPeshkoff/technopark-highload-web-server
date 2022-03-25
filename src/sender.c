// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#include "sender.h"

#define HEADER_BUFFER_SIZE 450

/**
 * @see sender.h
 */
void send_header(int sockfd, char *method, char *code, char *type, int length) {
  char buf[HEADER_BUFFER_SIZE];

  time_t current_time;
  char *c_time_string;

  current_time = time(NULL);
  c_time_string = ctime(&current_time);
  c_time_string[strlen(c_time_string) - 1] = '\0';

  if (strcmp(code, "200") == 0) {
    sprintf(buf,
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Server: twhs\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "Date: %s\r\n\r\n",
            type, length, c_time_string);
  } else if (strcmp(code, "400") == 0) {
    strcpy(buf, "HTTP/1.0 404 Bad Request\r\n"
                "Server: twhs\r\n\r\n");
  } else if (strcmp(code, "403") == 0) {
    strcpy(buf, "HTTP/1.0 403 Forbidden\r\n"
                "Server: twhs\r\n\r\n");
  } else if (strcmp(code, "404") == 0) {
    strcpy(buf, "HTTP/1.0 404 Not Found\r\n"
                "Server: twhs\r\n\r\n");
  } else if (strcmp(code, "405") == 0) {
    strcpy(buf, "HTTP/1.0 405 Not Found\r\n"
                "Server: twhs\r\n\r\n");
  }

  send(sockfd, buf, strlen(buf), 0);
}

/**
 * @see sender.h
 */
void send_response(int sockfd, struct HTTP_Header *header) {
  FILE *file;
  char code[4];
  char *tmp;

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    char* path = strcat(cwd, header->filename);
    file = fopen(path, "r");
  }
  
  if (file == NULL) {
    if (errno == ENOENT) {
      tmp = strstr(header->filename, "index");
      if (tmp == NULL) {
        strcpy(code, "404");
      } else {
        strcpy(code, "403");
      }
    } else if (errno == EACCES) {
      strcpy(code, "403");
    } else {
      strcpy(code, "404");
    }

    send_header(sockfd, header->method, code, header->content_type, 0);
    printf("%s %s %s\n", header->method, header->filename, code);
  } else {
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);

    fseek(file, 0L, SEEK_SET);

    if (strcmp(header->method, "HEAD") == 0) {
      strcpy(code, "200");
      send_header(sockfd, header->method, code, header->content_type, size);
      printf("%s %s %s\n", header->method, header->filename, code);

      fclose(file);
      return;
    }

    if (strcmp(header->method, "POST") == 0) {
      strcpy(code, "405");
      send_header(sockfd, header->method, code, header->content_type, size);
      printf("%s %s %s\n", header->method, header->filename, code);

      fclose(file);
      return;
    }

    strcpy(code, "200");
    send_header(sockfd, header->method, code, header->content_type, size);
    printf("%s %s %s\n", header->method, header->filename, code);

    int nbytes = 0;
    char filedata[size];
    while (size > 0) {
      nbytes = fread(filedata, sizeof(char), size - nbytes, file);
      if (nbytes == -1) {
        fclose(file);
        return;
      }
      size -= nbytes;

      ssize_t nwrite = 0;
      nwrite = write(sockfd, filedata, nbytes);
      if (nwrite == -1) {
        fclose(file);
        return;
      }
      nbytes -= nwrite;
      while (nbytes > 0) {
        nwrite = write(sockfd, filedata + nwrite, nbytes);
        if (nwrite == -1) {
          fclose(file);
          return;
        }
        nbytes -= nwrite;
      }
    }

    fclose(file);
  }
}
