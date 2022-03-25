// Copyright 2022 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#pragma once

// Заголовок запроса
typedef struct HTTP_Header
{
    char method[5];
    char filename[200];
    char content_type[100];
} HTTP_Header;
