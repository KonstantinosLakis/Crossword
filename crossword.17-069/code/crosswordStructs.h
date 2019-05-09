#include <stdio.h>
#define MAX_LENGTH 128
typedef struct space* Spaceptr;

struct space{
  int x;
  int y;
  int length;
  char isFilled;
  char isHorizontal;
  char *word;
  char *domain;
  int domain_size;
  struct crossing* crossings;
  int *jumpback;
};

struct crossing{
  Spaceptr enemy;
  int enemy_index;
  char *crossouts;
};
