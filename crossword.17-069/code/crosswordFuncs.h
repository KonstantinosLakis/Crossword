#include <stdio.h>
#define MAX_LENGTH 128
typedef struct space* Spaceptr;

void get_dict_name(int, char **, char **);
void get_cross_name(char **, char **);
void correct_buffer(char *buf);
void print_words(char ***, int *);
void print_spaces(Spaceptr, int);
void get_actual_spaces(Spaceptr, FILE *, int *, int);
void get_crossings(Spaceptr, int, int *);
void createLexicon(char ****, char ***, int *);
void print_lexicon(char ****, int *);
void unfill_space(Spaceptr, int *, Spaceptr, int, int);
void draw_crossword(Spaceptr, int, int);
void check(Spaceptr, int, char ***, int *, int, char);
void free_2D_char_array(char **, int);
void free_spaces(Spaceptr, int);
void free_words(char ***, int *);
void free_lexicon(char ****, int *);
void free_rest(char ***, char ****, int *, char *, char *);
void fill_space(Spaceptr, int , char ***, int *, char ****, int);
void merge_jumpbacks(Spaceptr, Spaceptr, int, int);

int get_spaces(Spaceptr *, char *, int *, int *, int *);
int get_words(char ***, int *, char *);
int count_spaces(FILE *, int *);
int solve(Spaceptr, char ***, int *, int, char ****, int, Spaceptr);
int in(char *, char **, int);
int get_last_conf(Spaceptr, int);
int LCV(Spaceptr, char *, int *, char ***, char ****);


char belongs(int, int, int);
char to_draw_or_not_to_draw(int, char **);
char to_solve_or_to_check(int, char **);
char isConsistent(Spaceptr);
char isEmpty(Spaceptr, int);

Spaceptr DVO(Spaceptr, int);
Spaceptr create_empty_spaces(int, int *);
