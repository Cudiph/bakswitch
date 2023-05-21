#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "vecstr.h"

#define PATH_MAX_SIZE 0xff

static const char *ALPHA_NUM = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const char *HELP_MSG =
    "Program to swap original and backup file\n"
    "\n--help            -h           show this message\n"
    "\nUSAGE\n"
    "    $ bakswitch <extension>\n"
    "    if extension is not specified, it is default to \"bak\"\n"
    "\nEXAMPLE\n"
    "    $ bakswitch backup \n"
    "    will swap file named \"nginx.conf.backup\" to \"nginx.conf\" and \"nginx.conf\" to \"nginx.conf.backup\"";

/** reverse string in place */
void str_reverse(char *text) {
    int len = strlen(text);

    int back_ptr = len - 1;
    for (int front_ptr = 0; front_ptr < len / 2; front_ptr++, back_ptr--) {
        char temp       = text[front_ptr];
        text[front_ptr] = text[back_ptr];
        text[back_ptr]  = temp;
    }
}

/** return 1 if true otherwise 0 */
int str_endswith(char *text, char *suffix) {
    int suff_len = strlen(suffix);
    int t_len    = strlen(text);

    if (t_len < suff_len) return 0;

    int suffix_pointer = suff_len - 1;
    for (int i = t_len - 1; i > t_len - 1 - suff_len; i--, suffix_pointer--) {
        if (text[i] != suffix[suffix_pointer]) return 0;
    }

    return 1;
}

/** generate random ascii letter string */
char *str_generate(unsigned int length) {
    char *rand_text = calloc(length + 1, sizeof(char));

    for (int i = 0; i < length; i++) {
        int random_index = rand() % strlen(ALPHA_NUM);
        rand_text[i]     = ALPHA_NUM[random_index];
    }

    return rand_text;
}

/** prompt user to overwrite if newname exist */
int rename_with_check(char *oldname, char *newname) {
    char answer;
    if (access(newname, F_OK) == 0 && access(oldname, F_OK) == 0) {
        while (1) {
            printf("do you want to overwrite %s to '%s'? [y/n] ", oldname, newname);
            scanf("%c", &answer);
            answer = tolower(answer);

            if (answer == 'y')
                break;
            else if (answer == 'n')
                return 0;
        }
    }

    if (rename(oldname, newname) == 0) {
        printf("renamed %s to %s\n", oldname, newname);
    } else {
        fprintf(stderr, "failed to rename %s to %s\n", oldname, newname);
        return 0;
    }

    return 1;
}

/** return string vector of file that ends with extension */
vecstr *extension_filter(char *path, char *extension) {
    DIR *dir;
    struct dirent *entry;
    vecstr *filtered = vecstr_new();

    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "cannot open directory %s\n", path);
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (str_endswith(entry->d_name, extension) == 1) {
            vecstr_push_back(filtered, entry->d_name);
        }
    }
    closedir(dir);

    return filtered;
}

/** begin swapping backup file and current file */
void switch_backup_files(vecstr *backup_file_list, unsigned int extension_len) {
    for (int i = 0; i < backup_file_list->length; i++) {
        char *backup_filename              = vecstr_get(backup_file_list, i);
        unsigned int without_extension_len = strlen(backup_filename) - extension_len;
        char *current_filename             = strndup(backup_filename, without_extension_len);

        // continue if non backup file is not exist
        if (access(current_filename, F_OK) != 0) {
            free(current_filename);
            continue;
        }

        char *random_extension   = str_generate(7);
        char *temporary_filename = calloc(strlen(current_filename) + 9, sizeof(char));
        strcpy(temporary_filename, current_filename);
        strcat(temporary_filename, ".");
        strcat(temporary_filename, random_extension);

        // begin swapping file
        puts("");
        rename_with_check(backup_filename, temporary_filename);
        rename_with_check(current_filename, backup_filename);
        rename_with_check(temporary_filename, current_filename);

        free(random_extension);
        free(current_filename);
        free(temporary_filename);
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    // check for help flag
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            puts(HELP_MSG);
            return 0;
        }
    }

    // get working directory
    char cwd[PATH_MAX_SIZE];
    if (getcwd(cwd, PATH_MAX_SIZE) == NULL) {
        fputs("Path is too long", stderr);
        return 1;
    }

    // get last extension so it can swap any extension
    char *extension = malloc(5);
    if (argc <= 1) {
        strcpy(extension, ".bak");
    } else {
        extension    = realloc(extension, strlen(argv[1]) + 2);
        extension[0] = '.';
        strcat(extension, argv[1]);
    }

    // filter filename that end with extension
    vecstr *filtered = extension_filter(cwd, extension);

    if (!filtered->length) return 0x7f;

    switch_backup_files(filtered, strlen(extension));

    free(extension);
    vecstr_clear(filtered);
    free(filtered);
    return 0;
}
