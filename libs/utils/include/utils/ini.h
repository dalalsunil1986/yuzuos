#pragma once

#define INI_SECTION_MAX 50
#define INI_NAME_MAX 50
#define INI_LINE_MAX 200
#define INI_START_COMMENT ";#"
#define INI_INLINE_COMMENT ";"

typedef int (*ini_handler_t)(const char *section, const char *key, const char *value);

void ini_open(const char *path, ini_handler_t handler);
