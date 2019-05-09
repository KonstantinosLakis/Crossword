#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crosswordFuncs.h"
#include "crosswordStructs.h"


int main(int argc, char **argv){
  char to_draw, to_solve, empty;
  char *cross_name;
  char *dict_name;
  char ***words_by_length;
  char ****lexicon;
  int *domain_sizes;
  int spaces_count, result, dimension;
  Spaceptr spaces;
  if (argc < 2){
    printf("Need a crossword filename\n");
    return 1;
  }
  get_dict_name(argc, argv, &dict_name);
  get_cross_name(argv, &cross_name);
  to_draw = to_draw_or_not_to_draw(argc, argv);
  to_solve = to_solve_or_to_check(argc, argv);
  words_by_length = malloc((MAX_LENGTH + 1) * sizeof(char **));
  domain_sizes = malloc((MAX_LENGTH + 1) * sizeof(int)); /*amount of words per length*/
  if(get_words(words_by_length, domain_sizes, dict_name) != 0){
    return 1;
  }
  /*lexicon[i][j][k][l] == 1 means that the length i word of index l has the letter j
  (j == 0 -> letter == a etc) at index k*/
  lexicon = malloc((MAX_LENGTH + 1) * sizeof(char ***));
  createLexicon(lexicon, words_by_length, domain_sizes);
  if(get_spaces(&spaces, cross_name, &spaces_count, domain_sizes, &dimension) != 0){
    return 1;
  }
  empty = isEmpty(spaces, spaces_count);
  if(empty){
    // printf("crossord empty\n");
    spaces = create_empty_spaces(dimension, domain_sizes);
    // printf("created empty spaces\n");
    spaces_count = dimension;
    // printf("spaces count is : %d\n", spaces_count);
  }
  if(to_solve){ /*no -check*/
    result = solve(spaces, words_by_length, domain_sizes, spaces_count, lexicon, 1, NULL);
    if(result != 0){
      printf("Could not solve crossword\n");
      free_spaces(spaces, spaces_count);
    } else {
      if(to_draw){
        draw_crossword(spaces, spaces_count, dimension);
      } else {
        print_spaces(spaces, spaces_count);
      }
      free_spaces(spaces, spaces_count);
    }
  } else { /* -check*/
    check(spaces, spaces_count, words_by_length, domain_sizes, dimension, to_draw);
    free_spaces(spaces, spaces_count);
  }
  free_rest(words_by_length, lexicon, domain_sizes, dict_name, cross_name);
  return 0;
}
