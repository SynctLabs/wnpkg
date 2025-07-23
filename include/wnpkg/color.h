#ifndef __COLOR_H__
#define __COLOR_H__

#define RESET "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"

#define RED_PRINT(fmt, ...) printf("%s" fmt "%s", RED, ##__VA_ARGS__, RESET)
#define GREEN_PRINT(fmt, ...) printf("%s" fmt "%s", GREEN, ##__VA_ARGS__, RESET)
#define YELLOW_PRINT(fmt, ...)                                                 \
  printf("%s" fmt "%s", YELLOW, ##__VA_ARGS__, RESET)

#endif