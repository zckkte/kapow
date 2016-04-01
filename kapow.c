/*
 * Implementation of Knuth–Morris–Pratt string search algorithm
 * Author: Zack A. Kite
 * Date: Sat 2 Apr 02:31
 */

#include    "stdio.h"
#include    "string.h"
#include    "unistd.h"
#include    "ctype.h"

#define     ISSET(A)            ((A) != NULL)
#define     MAX_BUFFER_SIZE     (1000000)

void populate_match_table(int * table, const char * const needle);
int kmp_search (const char * const needle, const char * const haystack);
int read_file(char * source, FILE * file);

int main(int argc, char ** argv)
{
    int idx = -1;
    char * needle = NULL, * file_name = NULL; 
    char haystack[MAX_BUFFER_SIZE]; char ch;

    opterr = 0;
    while ((ch = getopt (argc, argv, "n:s:")) != -1) {
        switch (ch) {
            case 'n': needle = optarg; break;
            case 's': file_name = optarg; break;
            case '?':
                if (optopt == 'n' || optopt == 's')
                    fprintf(stderr,"Option -%c requires an argument.\n",optopt);
                else
                    fprintf(stderr,"Unknown option character `\\x%x'.\n",optopt);
                return 1;
        }
    }

    if (ISSET(needle) && ISSET(file_name)) {
        if (access(file_name, F_OK ) < 0) {
            fprintf(stderr, "File '%s' does not exist.\n", file_name);
            return 0;
        }

        FILE * source_file = fopen(file_name, "r");

        if (read_file(haystack, source_file) == 0) {
            idx = kmp_search(needle, haystack);
            printf("%d\n", idx);
        }
    }

    return 0;
}

int kmp_search (const char * const needle, const char * const haystack)
{
    int m = 0, i = 0;
    int partial_match_table[strlen(needle)];
    populate_match_table(partial_match_table, needle);

    while (m + i  < strlen(haystack)) {
        if (needle[i] == haystack[m + i]) {
            if (i == strlen(needle) - 1) return m;
            i++;
        } else {
            if (partial_match_table[i] > -1) {
                m = m + i - partial_match_table[i];
                i = partial_match_table[i];
            } else {
                i = 0; m++;
            }
        }

    }

    return -1;
}

void populate_match_table(int * table, const char * const needle)
{
    int pos = 2, cnd = 0; 
    table[0] = -1; table[1] = 0;

    while (pos < strlen(needle)) {
        if (needle[pos - 1] == needle[cnd]) {
            table[pos] = cnd + 1;
            cnd++; pos++;
        } else if (cnd > 0) {
            cnd = table[cnd];
        } else {
            table[pos] = 0; pos++;
        }
    }
}

int read_file(char * source, FILE * file)
{
    if (!ISSET(file)) fprintf (stderr, "populate_haystack --- file not initialize");

    size_t len = fread(source, sizeof(char), MAX_BUFFER_SIZE, file);
    if (!len) {
        fprintf(stderr, "Error reading file");
        return -1;
    }

    source[len++] = '\0';
    fclose(file);

    return 0;
}
