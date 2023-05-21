/** vector of string structure */
typedef struct {
    unsigned int length;
    char **string_list;
} vecstr;

/** create new vector of string */
vecstr *vecstr_new();

/** push string to the end of vector */
int vecstr_push_back(vecstr *self, char *str);

/** delete last element in the vector */
int vecstr_pop_back(vecstr *self);

/** deallocate string_list */
void vecstr_clear(vecstr *self);

/** get element at index */
char *vecstr_get(vecstr *self, int index);
