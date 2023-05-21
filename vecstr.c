/*
 * Okay vector implementation, I guess...
 */
#include "vecstr.h"

#include <stdlib.h>
#include <string.h>

vecstr *vecstr_new() {
    vecstr *new_vec = calloc(1, sizeof(vecstr));
    return new_vec;
}

int vecstr_push_back(vecstr *self, char *str) {
    if (self->string_list == NULL) self->string_list = malloc(0);
    self->string_list = realloc(self->string_list, sizeof(char *) * (++self->length));

    char *new_str = malloc(strlen(str) + 1);
    strcpy(new_str, str);
    self->string_list[self->length - 1] = new_str;
    return 0;
}

int vecstr_pop_back(vecstr *self) {
    if (self->length == 0) return 0;
    free(self->string_list[--self->length]);
    self->string_list = realloc(self->string_list, sizeof(char *) * (self->length));
    return 1;
}

void vecstr_clear(vecstr *self) {
    if (self->string_list == NULL) return;

    // deallocating
    const unsigned int immutable_length = self->length;
    for (int i = 0; i < immutable_length; i++) {
        free(self->string_list[i]);
    }
    free(self->string_list);

    // reset
    self->string_list = NULL;
    self->length      = 0;
}

char *vecstr_get(vecstr *self, int index) {
    if (index < 0 || index >= self->length) return NULL;
    return self->string_list[index];
}
