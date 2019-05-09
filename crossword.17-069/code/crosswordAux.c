#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crosswordFuncs.h"
#include "crosswordStructs.h"

void print_lexicon(char ****lexicon, int *domain_sizes){ /*used for debugging*/
  int i, j, k, l;
  for(i = 0; i < 30; i ++){
    for(j = 0; j < 26; j ++){
      for(k = 0; k < i; k ++){
        printf("Words of length %d with letter %c at place %d :\n", i, j + 'a', k);
        for(l = 0; l < domain_sizes[i]; l ++){
          printf("%d ", lexicon[i][j][k][l]);
        }
      }
    }
  }
}

void print_spaces(Spaceptr spaces, int spaces_count){
  int i, j;
  for(i = 0; i < spaces_count; i ++){
    printf("%s\n",spaces[i].word);
  }
}

void print_words(char ***words_by_length, int *domain_sizes){
  int i, j;
  for(i = 0; i < MAX_LENGTH + 1; i ++){
    for(j = 0; j < domain_sizes[i]; j ++){
      printf("%s\n", words_by_length[i][j]);
    }
  }
}

void get_dict_name(int argc, char **argv, char **dict_name){
  int i;
  for (i = 1; i < argc - 1; i ++){
    if(strcmp("-dict", argv[i]) == 0){
      *dict_name = malloc((strlen(argv[i + 1]) + 1) * sizeof(char)); /*get arg after -dict*/
      strcpy(*dict_name, argv[i + 1]);
      return;
    }
  }
  *dict_name = malloc((strlen("Words.txt") + 1) * sizeof(char)); /*deafult dict*/
  strcpy(*dict_name, "Words.txt");
  return;
}

void get_cross_name(char **argv, char **cross_name){
  *cross_name = malloc((strlen(argv[1]) + 1) * sizeof(char));/*get first argument which is the crossword name*/
  strcpy(*cross_name, argv[1]);
  return;
}

int get_words(char ***words_by_length, int *domain_sizes, char *dict_name){
  int i, count, j;
  int *curr_indices;
  char ch;
  char *buf;
  FILE *dict;
  curr_indices = malloc((MAX_LENGTH + 1) * sizeof(int)); /*used for filling words_by_length
  because we need to keep track of the next place(index) to fill*/
  for(i = 0; i < MAX_LENGTH + 1; i ++){
    domain_sizes[i] = 0;
    curr_indices[i] = 0;
  }
  dict = fopen(dict_name, "r");
  if(dict == NULL){
    printf("Did not find the dictionary specified\n");
    return 1;
  }
  do{
    count = 0;
    ch = getc(dict);
    while(ch != '\n' && ch != EOF){ /*we are in a word*/
      count ++;
      ch = getc(dict);
    }
    if(ch == '\n'){ /*end of word*/
      domain_sizes[count] ++; /*we have another word of length count*/
    }
  }while(ch != EOF);
  fclose(dict);
  for (i = 0; i < MAX_LENGTH + 1; i++){
    words_by_length[i] = malloc(domain_sizes[i] * sizeof(char *));
    for(j = 0; j < domain_sizes[i]; j ++){
      words_by_length[i][j] = malloc((i + 1) * sizeof(char));
    }
  }
  dict = fopen(dict_name, "r");
  buf = malloc((MAX_LENGTH + 2) * sizeof(char));
  while((buf = fgets(buf, MAX_LENGTH + 1, dict)) != NULL){
    correct_buffer(buf); /*remove change of line char that fgets returns*/
    count = strlen(buf);
    strcpy(words_by_length[count][curr_indices[count]], buf);
    curr_indices[count] ++; /*show to the next place we will fill*/
  }
  free(curr_indices);
  free(buf);
  fclose(dict);
  return 0;
}

void correct_buffer(char *buf){
  int end = strlen(buf) - 1;
  buf[end] = '\0';
  return;
}

int get_spaces(Spaceptr *spaces, char *cross_name, int *spaces_count_call, int *domain_sizes, int *dimension){
  int spaces_count;
  FILE *crossword;
  crossword = fopen(cross_name, "r");
  if(crossword == NULL){
    printf("Could not find crossword specified\n");
    return 1;
  }
  spaces_count = count_spaces(crossword, dimension);
  fclose(crossword);
  *spaces = malloc(spaces_count * sizeof(struct space));
  crossword = fopen(cross_name, "r");
  get_actual_spaces(*spaces, crossword, domain_sizes, spaces_count);
  fclose(crossword);
  get_crossings(*spaces, spaces_count, domain_sizes);
  *spaces_count_call = spaces_count;
  return 0;
}

int count_spaces(FILE *crossword, int *dimension_call){
  int dimension, i, j, x, y, start_x, start_y, count, spaces = 0;
  char **field; /*used for keeping track of black blocks*/
  char in_space = 0;
  fscanf(crossword, "%d\n", &dimension);
  *dimension_call = dimension;
  field = malloc(dimension * sizeof(char *));
  for(i = 0; i < dimension; i ++){
    field[i] = malloc(dimension * sizeof(char));
    for(j = 0; j < dimension; j ++){
      field[i][j] = 0;
    }
  }
  while(fscanf(crossword, "%d %d\n", &x, &y) == 2){
    field[y - 1][x - 1] = 1; /*make the block black*/
  }
  for(y = 0; y < dimension; y ++){
    x = 0;
    do{
      while((field[x][y] || field[x + 1][y]) && x < dimension - 2){ /*finds the start of a word
        which means at least 2 consecutive non-black blocks*/
        x ++;
      }
      start_x = x;
      count = 0;
      while(!field[x][y] && x < dimension - 1){ /*finds end of word*/
        count ++;
        x ++;
      }
      if((x == dimension - 1) && !field[x][y]){ /*if last in line block is non-black (we have not yet counted it)*/
        count ++;
      }
      if(count > 1){ /*probably redundant, remained after a previous implementation*/
        spaces ++;
      }
    }while(x < dimension - 2); /*there is no word that starts after dimension - 2*/
  }
  /*same logic but for vertical words*/
  for(x = 0; x < dimension; x ++){
    y = 0;
    do{
      while((field[x][y] || field[x][ y + 1]) && y < dimension - 2){
        y ++;
      }
      start_y = y;
      count = 0;
      while(!field[x][y] && y < dimension - 1){
        count ++;
        y ++;
      }
      if((y == dimension - 1) && !field[x][y]){
        count ++;
      }
      if(count > 1){
        spaces ++;
      }
    }while(y < dimension - 2);
  }
  free_2D_char_array(field, dimension);
  return spaces;
}

void get_actual_spaces(Spaceptr spaces, FILE *crossword, int *domain_sizes, int spaces_count){
  int dimension, i, j, x, y, k, start_x, start_y, count, curr_index = 0;
  char **field;
  char in_space = 0;
  fscanf(crossword, "%d\n", &dimension);
  field = malloc(dimension * sizeof(char *));
  for(i = 0; i < dimension; i ++){
    field[i] = malloc(dimension * sizeof(char));
    for(j = 0; j < dimension; j ++){
      field[i][j] = 0;
    }
  }
  while(fscanf(crossword, "%d %d\n", &x, &y) == 2){
    field[y - 1][x - 1] = 1;
  }
  /*same logic as count_spaces*/
  for(y = 0; y < dimension; y ++){
    x = 0;
    do{
      while((field[x][y] || field[x + 1][y]) && x < dimension - 2){
        x ++;
      }
      start_x = x;
      count = 0;
      while(!field[x][y] && x < dimension - 1){
        count ++;
        x ++;
      }
      if((x == dimension - 1) && !field[x][y]){
        count ++;
      }
      if(count > 1){
        spaces[curr_index].x = start_x; /*we have kept the space's x in start_x*/
        spaces[curr_index].y = y; /*y remains constant*/
        spaces[curr_index].length = count;
        spaces[curr_index].isFilled = 0;
        spaces[curr_index].isHorizontal = 1;
        spaces[curr_index].domain = malloc(domain_sizes[count] * sizeof(char)); /*initialize space's domain*/
        for(k = 0; k < domain_sizes[count]; k ++){
          (spaces[curr_index].domain)[k] = 1;
        }
        spaces[curr_index].domain_size = domain_sizes[count];
        spaces[curr_index].crossings = NULL;
        spaces[curr_index].jumpback = malloc((spaces_count + 1) * sizeof(int));
        for(k = 0; k < spaces_count + 1; k ++){
          (spaces[curr_index].jumpback)[k] = 0;
        }
        curr_index ++;
      }
    }while(x < dimension - 2);
  }

  for(x = 0; x < dimension; x ++){
    y = 0;
    do{
      while((field[x][y] || field[x][y + 1]) && y < dimension - 2){
        y ++;
      }
      start_y = y;
      count = 0;
      while(!field[x][y] && y < dimension - 1){
        count ++;
        y ++;
      }
      if((y == dimension - 1) && !field[x][y]){
        count ++;
      }
      if(count > 1){
        spaces[curr_index].x = x; /*x remains constant*/
        spaces[curr_index].y = start_y; /*we have kept the space's y at start_y*/
        spaces[curr_index].length = count;
        spaces[curr_index].isFilled = 0;
        spaces[curr_index].isHorizontal = 0;
        spaces[curr_index].domain = malloc(domain_sizes[count] * sizeof(char));/*initialize space's domain*/
        for(k = 0; k < domain_sizes[count]; k ++){
          (spaces[curr_index].domain)[k] = 1;
        }
        spaces[curr_index].domain_size = domain_sizes[count];
        spaces[curr_index].crossings = NULL;
        spaces[curr_index].jumpback = malloc(spaces_count * sizeof(int));
        for(k = 0; k < spaces_count; k ++){
          (spaces[curr_index].jumpback)[k] = 0;
        }
        curr_index ++;
      }
    }while(y < dimension - 2);
  }
  free_2D_char_array(field, dimension);
  return;
}

void get_crossings(Spaceptr spaces, int spaces_count, int *domain_sizes){
  int c_x, e_x, c_y, e_y, c_l, e_l, i, j, k;
  for(i = 0; i < spaces_count; i ++){
    c_x = spaces[i].x;
    c_y = spaces[i].y;
    c_l = spaces[i].length;
    spaces[i].crossings = malloc(c_l * sizeof(struct crossing));
    for(j = 0; j < c_l; j ++){
      /*initialize crossing for space that intersects current space at index j(if it exists)*/
      ((spaces[i].crossings)[j]).enemy = NULL;
      ((spaces[i].crossings)[j]).enemy_index = -1;
    }
    for(j = 0; j < spaces_count; j ++){
      e_x = spaces[j].x;
      e_y = spaces[j].y;
      e_l = spaces[j].length;
      if(spaces[j].isHorizontal != spaces[i].isHorizontal){ /*orientation must differ for crossing to exist*/
        if(spaces[i].isHorizontal){
          if(belongs(e_x, c_x, c_x + c_l - 1) &&
          belongs(c_y, e_y, e_y + e_l - 1)){ /*the words actually cross*/
            (spaces[i].crossings)[e_x - c_x].enemy = &spaces[j];
            (spaces[i].crossings)[e_x - c_x].enemy_index = c_y - e_y;
            /*initialize set that keeps track of values we remove from enemy's domain*/
            (spaces[i].crossings)[e_x - c_x].crossouts = malloc(domain_sizes[e_l] * sizeof(char));
            for(k = 0; k < domain_sizes[e_l]; k ++){
              ((spaces[i].crossings)[e_x - c_x].crossouts)[k] = 0;
            }
          }
        } else {
          /*same logic but for vertical words*/
          if(belongs(c_x, e_x, e_x + e_l - 1) &&
          belongs(e_y, c_y, c_y + c_l - 1)){
            (spaces[i].crossings)[e_y - c_y].enemy = &spaces[j];
            (spaces[i].crossings)[e_y - c_y].enemy_index = c_x - e_x;
            (spaces[i].crossings)[e_y - c_y].crossouts = malloc(domain_sizes[e_l] * sizeof(char));
            for(k = 0; k < domain_sizes[e_l]; k ++){
              ((spaces[i].crossings)[e_y - c_y].crossouts)[k] = 0;
            }
          }
        }
      }
    }
  }
  return;
}

/*implements value in range [start, end]*/
char belongs(int value, int start, int end){
  char result;
  if(value >= start && value <= end){
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

/*returns pointer to best space to fill according to
 Dynamic Variable Ordering with Most Constrainted Variable heuristic*/
Spaceptr DVO(Spaceptr spaces, int spaces_count){
  int i, best, best_index = -1, curr;
  for(i = 0; i < spaces_count; i ++){
    if(!spaces[i].isFilled){
      curr = spaces[i].domain_size;
      if(best_index == -1){
        best_index = i;
        best = curr;
        continue;
      }
      if(curr < best){
        best_index = i;
        best = curr;
      }
    }
  }
  if(best_index == -1){
    return NULL;
  } else {
    return &spaces[best_index];
  }
}

void createLexicon(char ****lexicon, char ***words_by_length, int *domain_sizes){
  int i, j, k, l, words;
  for(i = 0; i < MAX_LENGTH + 1; i ++){
    words = domain_sizes[i];
    lexicon[i] = malloc(26 * sizeof(char **));
    for(j = 0; j < 26; j ++){
      lexicon[i][j] = malloc(i * sizeof(char *));
      for(k = 0; k < i; k ++){
        lexicon[i][j][k] = malloc(words * sizeof(char));
        for(l = 0; l < words; l ++){
          lexicon[i][j][k][l] = 0; /*initialize lexicon*/
        }
      }
    }
  }
  for(i = 0; i < MAX_LENGTH + 1; i ++){
    words = domain_sizes[i];
    for(j = 0; j < words; j ++){
      for(k = 0; k < i; k ++){
        lexicon[i][words_by_length[i][j][k] - 'a'][k][j] = 1;
      }
    }
  }
  return;
}

/*fills the space with the word specified and applies Forward Checking, also updates jumpback sets for CBJ*/
void fill_space(Spaceptr space_to_fill, int word_index, char ***words_by_length, int *domain_sizes, char ****lexicon, int level){
  int i, space_length = space_to_fill->length, enemy_index, enemy_length, j;
  char letter;
  Spaceptr enemy;
  char *word = words_by_length[space_length][word_index];
  space_to_fill->word = word;
  space_to_fill->isFilled = 1;
  for(i = 0; i < space_length; i ++){
    enemy_index = ((space_to_fill->crossings)[i]).enemy_index;
    if(enemy_index != -1){ /*if current word has a crossing at index i*/
      letter = word[i] - 'a';
      enemy = ((space_to_fill->crossings)[i]).enemy;
      enemy_length = enemy->length;
      for(j = 0; j < domain_sizes[enemy_length]; j ++){ /*for all words in enemy's domain*/
        if((enemy->domain)[j]){ /*if they haven't yet been ruled out*/
          if(!lexicon[enemy_length][letter][enemy_index][j]){ /*if the word j in enemy's domain does not
           satisfy the constraint we have just introduced*/
            (enemy->jumpback)[level] = 1; /*Add to jumpback set of reduced domain variable*/
            (enemy->domain)[j] = 0; /*remove it from the domain*/
            (((space_to_fill->crossings)[i]).crossouts)[j] = 1; /*keep its index so we can recover it*/
            (enemy->domain_size)--;
          }
        }
      }
    }
  }
  return;
}

/*clears the space and recovers enemies' domains*/
void unfill_space(Spaceptr space, int *domain_sizes, Spaceptr spaces, int spaces_count, int level){
  int space_length = space->length, i, j, enemy_length, enemy_index;
  Spaceptr enemy;
  space->word = NULL;
  space->isFilled = 0;
  for(i = 0; i < space_length; i ++){
    enemy_index = ((space->crossings)[i]).enemy_index;
    if(enemy_index != -1){ /*if there is a crossing*/
      enemy = ((space->crossings)[i]).enemy;
      enemy_length = enemy->length;
      for(j = 0; j < domain_sizes[enemy_length]; j ++){
        enemy->domain[j] = enemy->domain[j] || (((space->crossings)[i]).crossouts)[j]; /*domain = domain U crossouts*/
        enemy->domain_size += (((space->crossings)[i]).crossouts)[j]; /*|domain| += |crossouts|*/
        (((space->crossings)[i]).crossouts)[j] = 0;
      }
    }
  }
  for(i = 0; i < spaces_count; i ++){
    ((spaces[i]).jumpback)[level] = 0; /*unassigned so it can not be in a jumpback set*/
  }
  return;
}

/*poetically decides whether to draw or not*/
char to_draw_or_not_to_draw(int argc, char **argv){
  int i;
  char to_draw = 0;
  for(i = 0; i < argc; i ++){
    if(strcmp(argv[i], "-draw") == 0){
      to_draw = 1;
      break;
    }
  }
  return to_draw;
}


void draw_crossword(Spaceptr spaces, int spaces_count, int dimension){
  int i, j;
  char **field;
  field = malloc(dimension * sizeof(char *));
  for(i = 0; i < dimension; i ++){
    field[i] = malloc(dimension * sizeof(char));
    for(j = 0; j < dimension; j ++){
      field[i][j] = ' '; /*set all blocks to black for starters*/
    }
  }

  /*actually fill the field with words*/
  for(i = 0; i < spaces_count; i ++){
    if(spaces[i].isHorizontal){
      for(j = 0; j < spaces[i].length; j ++){
        field[spaces[i].x + j][spaces[i].y] = (spaces[i].word)[j];
      }
    } else {
      for(j = 0; j < spaces[i].length; j ++){
        field[spaces[i].x][spaces[i].y + j] = (spaces[i].word)[j];
      }
    }
  }
  /*pretty print result*/
  for(i = 0; i < dimension; i ++){
    printf("~~");
  }
  printf("~\n");
  for(j = 0; j < dimension; j ++){
    for(i = 0; i < dimension; i ++){
      printf("|%c", field[i][j]);
    }
    printf("|\n");
  }
  for(i = 0; i < dimension; i ++){
    printf("~~");
  }
  printf("~\n");
  free_2D_char_array(field, dimension);
  return;
}

/*no inspiration here*/
char to_solve_or_to_check(int argc, char **argv){
  char to_solve = 1;
  int i;
  for(i = 0; i < argc; i++){
    if(strcmp(argv[i], "-check") == 0){
      to_solve = 0;
      break;
    }
  }
  return to_solve;
}

void check(Spaceptr spaces, int spaces_count, char ***words_by_length, int *domain_sizes, int dimension, char to_draw){
  int i = 0, count, index; /*i counts the words we have inserted so far*/
  char *buf;
  char consistent;
  buf = malloc((MAX_LENGTH + 2) * sizeof(char));
  while((buf = fgets(buf, MAX_LENGTH + 1, stdin)) != NULL){ /*while there is a word in input*/
    if(i >= spaces_count){
      printf("More words than needed.\n");
      return;
    }
    correct_buffer(buf);
    count = strlen(buf);
    if((index = in(buf, words_by_length[count], domain_sizes[count])) != -1){ /*search for word*/
      if(count != spaces[i].length){ /*satisfy the length property*/
        printf("word '%s' cannot be placed because of its length.\n", buf);
        free(buf);
        return;
      } else {
        spaces[i].word = words_by_length[spaces[i].length][index];
        spaces[i].isFilled = 1;
        consistent = isConsistent(spaces + i); /*see if we have introduced any inconsistencies*/
        if(!consistent){
          printf("word '%s' cannot be placed because of inconsistencies with previous words.\n", buf);
          free(buf);
          return;
        }
      }
    } else {
      printf("word '%s' not in dictionary\n", buf);
      free(buf);
      return;
    }
    i ++;
  }
  if(i < spaces_count){
    printf("Need more words.\n");
    free(buf);
    return;
  } else {
    if(to_draw){
      draw_crossword(spaces, spaces_count, dimension);
    }
    free(buf);
    return;
  }
}

/*linear search because of generally low amount of searches we will need to make
  against the high amount of words we would have to order (and a tiny bit of laziness)*/
int in(char *word, char **dict, int dict_size){
  int i;
  for(i = 0; i < dict_size; i ++){
    if(strcmp(word, dict[i]) == 0){
      return i;
    }
  }
  return -1;
}

/*could make use of the lexicon here, but as with in(), decided it was
  not worth the increase in efficiency*/
char isConsistent(Spaceptr space){
  int i, space_length, enemy_index, enemy_length;
  char consistent = 1;
  Spaceptr enemy;
  space_length = space->length;
  for(i = 0; i < space_length; i ++){
    enemy_index = ((space->crossings)[i]).enemy_index;
    if(enemy_index != -1){
      enemy = ((space->crossings)[i]).enemy;
      if(enemy->isFilled){
        if((space->word)[i] != (enemy->word)[enemy_index]){
          consistent = 0;
          break;
        }
      }
    }
  }
  return consistent;
}

void merge_jumpbacks(Spaceptr previous, Spaceptr current, int level, int spaces_count){
  int i;
  for(i = 0; i < spaces_count; i ++){
    (previous->jumpback)[i] = (previous->jumpback)[i] || (current->jumpback)[i];
  }
  (previous->jumpback)[level - 1] = 0;
}

int get_last_conf(Spaceptr space, int spaces_count){ /*returns most recent level that conflicts with current*/
  int i = spaces_count;
  while((space->jumpback)[i] == 0 && i > 0){
    i --;
  }
  if(i == 0 && (space->jumpback)[i] == 0){
    i --;
  }
  return i;
}

int LCV(Spaceptr space, char *tried, int *domain_sizes, char ***words_by_length, char ****lexicon){
  int i, j, k, count, best_index = -1, enemy_index;
  unsigned __int128 curr, best = 0;
  char letter;
  Spaceptr enemy;
  int **char_counts = malloc(space->length * sizeof(char *));
  for(j = 0; j < space->length; j ++){
    enemy_index = ((space->crossings)[j]).enemy_index;
    if(enemy_index != -1){
      enemy = ((space->crossings)[j]).enemy;
      char_counts[j] = malloc(26 * sizeof(char *));
      for(i = 0; i < 26; i ++){
        char_counts[j][i] = 0;
      }
      for(k = 0; k < domain_sizes[enemy->length]; k ++){
        if((enemy->domain)[k]){
          char_counts[j][words_by_length[enemy->length][k][enemy_index] - 'a'] ++;
        }
      }
    }
  }


  for(i = 0; i < domain_sizes[space->length]; i ++){
    if(space->domain[i] && !tried[i]){
      curr = 1;
      for(j = 0; j < space->length && curr != 0; j ++){
        enemy_index = ((space->crossings)[j]).enemy_index;
        if(enemy_index != -1){
          count = char_counts[j][words_by_length[space->length][i][j] - 'a'];
          curr += count;
        }
      }
      if (curr >= best){
        best = curr;
        best_index = i;
      }
    }
  }
  tried[best_index] = 1;
  return best_index;
}

char isEmpty(Spaceptr spaces, int spaces_count){
  int crosses = 0, i, j;
  for(i = 0; i < spaces_count; i ++){
    for(j = 0; j < spaces[i].length; j ++){
      if(spaces[i].crossings[j].enemy_index == -1){
        return 0;
      }
    }
  }
  return 1;
}

Spaceptr create_empty_spaces(int dimension, int *domain_sizes){
  Spaceptr spaces = malloc(dimension * sizeof(struct space));
  int i, k, j;
  for(i = 0; i < dimension; i ++){
    spaces[i].x = 0; /*we have kept the space's x in start_x*/
    spaces[i].y = 0; /*y remains constant*/
    spaces[i].length = dimension;
    spaces[i].isFilled = 0;
    spaces[i].isHorizontal = 1;
    spaces[i].domain = malloc(domain_sizes[dimension] * sizeof(char)); /*initialize space's domain*/
    for(k = 0; k < domain_sizes[dimension]; k ++){
      (spaces[i].domain)[k] = 1;
    }
    spaces[i].domain_size = domain_sizes[dimension];
    spaces[i].crossings = NULL;
    spaces[i].jumpback = malloc(dimension * sizeof(int));
    for(k = 0; k < dimension; k ++){
      (spaces[i].jumpback)[k] = 0;
    }
  }
  for(i = 0; i < dimension; i++){
    spaces[i].crossings = malloc(dimension * sizeof(struct crossing));
    for(k = 0; k < dimension; k ++){
      if(k != i){
        spaces[i].crossings[k].enemy_index = i;
        spaces[i].crossings[k].enemy = &spaces[k];
        spaces[i].crossings[k].crossouts = malloc(domain_sizes[dimension] * sizeof(char));
        for(j = 0; j < domain_sizes[dimension]; j ++){
          spaces[i].crossings[k].crossouts[j] = 0;
        }
      } else {
        spaces[i].crossings[k].enemy_index = -1;
        spaces[i].crossings[k].enemy = NULL;
      }
    }
  }
  return spaces;
}

void free_2D_char_array(char **array, int dimension){
  int i;
  for(i = 0; i < dimension; i ++){
    free(array[i]);
  }
  free(array);
  return;
}

void free_spaces(Spaceptr spaces, int spaces_count){
  int i, j;
  for(i = 0; i < spaces_count; i++){
    for(j = 0; j < spaces[i].length; j++){
      if(((spaces[i]).crossings)[j].enemy_index != -1){
        free(((spaces[i].crossings)[j]).crossouts);
      }
    }
    free(spaces[i].crossings);
    free(spaces[i].domain);
  }
  free(spaces);
  return;
}

void free_words(char ***words_by_length, int *domain_sizes){
  int i, j;
  for(i = 0; i < MAX_LENGTH + 1; i ++){
    for(j = 0; j < domain_sizes[i]; j ++){
      free(words_by_length[i][j]);
    }
    free(words_by_length[i]);
  }
  free(words_by_length);
  return;
}

void free_lexicon(char ****lexicon, int *domain_sizes){
  int i, j, k;
  for(i = 0; i < MAX_LENGTH + 1; i ++){
    for(j = 0; j < 26; j ++){
      for(k = 0; k < i; k ++){
        free(lexicon[i][j][k]);
      }
      free(lexicon[i][j]);
    }
    free(lexicon[i]);
  }
  free(lexicon);
  return;
}

void free_rest(char ***words_by_length, char ****lexicon, int *domain_sizes, char *dict_name, char *cross_name){
  free_words(words_by_length, domain_sizes);
  free_lexicon(lexicon, domain_sizes);
  free(domain_sizes);
  free(dict_name);
  free(cross_name);
  return;
}
