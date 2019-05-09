#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crosswordFuncs.h"
#include "crosswordStructs.h"

int solve(Spaceptr spaces, char ***words_by_length, int *domain_sizes, int spaces_count, char ****lexicon, int level, Spaceptr previous){
  int i, words, result, index;
  char empty_domain;
  char *tried;
  Spaceptr space_to_fill = NULL;
  space_to_fill = DVO(spaces, spaces_count); /*get next variable*/
  if(space_to_fill == NULL){ /*all variables have been assigned a value*/
    return 0;
  }
  // printf("level is : %d\n", level);
  words = domain_sizes[space_to_fill->length];
  tried = malloc(words * sizeof(char));
  for(i = 0; i < words; i ++){
    tried[i] = 0;
  }
  while((index = LCV(space_to_fill, tried, domain_sizes, words_by_length, lexicon)) != -1){
      // printf("filling space with word %s\n", words_by_length[space_to_fill->length][index]);
      fill_space(space_to_fill, index, words_by_length, domain_sizes, lexicon, level);
      // printf("filled space with word %s\n", space_to_fill->word);
      /*solve rest of problem recursively*/
      result = solve(spaces, words_by_length, domain_sizes, spaces_count, lexicon, level + 1, space_to_fill);
      if(result == 0){ /*if we're done, we're done*/
        return 0;
      } else if(result == level) { /*Backtrack here*/
        unfill_space(space_to_fill, domain_sizes, spaces, spaces_count, level);
        continue;
      } else { /*Backtrack earlier*/
        unfill_space(space_to_fill, domain_sizes, spaces, spaces_count, level);
        if(previous != NULL){
          merge_jumpbacks(previous, space_to_fill, level, spaces_count);
        }
        return result;
    }
  }
  // printf("no elligible word\n");
  /*dead end, inform caller solve*/
  if(previous != NULL){
    merge_jumpbacks(previous, space_to_fill, level, spaces_count);
  }
  return get_last_conf(space_to_fill, spaces_count);
}
