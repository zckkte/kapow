/*
 * Implementation of Knuth–Morris–Pratt string search algorithm
 * Author: Zack A. Kite
 * Date: Sat 2 Apr 02:31
 */

#include    "stdio.h"
#include    "stdlib.h"
#include    "string.h"
#include    "unistd.h"
#include    "ctype.h"

#define     ISSET(A)            ((A) != NULL)
#define     WINDOW_LEN          (8)

void search_file(const char * const needle, FILE * infile);
int kmp_search (const char * const needle, const char * const haystack);
void populate_match_table(int * table, const char * const needle);
void concat(char ** a, const char * b, const char * const del);

int main(int argc, char ** argv)
{
    int idx = -1; char ch;
    char * needle = NULL, * file_name = NULL; 
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

        FILE * file = fopen(file_name, "r");
        search_file(needle, file);
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

void search_file(const char * const needle, FILE * infile)
{
    char * window = NULL, * line = NULL;
    size_t len = 0, read;
    int line_num = 0;

    if (!ISSET(infile)) exit(EXIT_FAILURE);

    printf("<KA-POW!>\n");
    while ((read = getline(&line, &len, infile)) != -1) {

        char delim[64]; sprintf(delim, "%d%5s", line_num, "");
        concat(&window, line, delim);

        if (line_num % WINDOW_LEN == 0 ) {
            if (kmp_search(needle, window) >= 0) printf("~>\n%s\n", window);
            memset(window, 0, strlen(window));
        }

        line_num++;
    }

    fclose(infile);
    free(window);
    free(line);
}

void concat(char ** a, const char * b, const char * const del)
{
    size_t a_len = *a ? strlen(*a) : 0;
    size_t b_len = b ? strlen(b) : 0;
    size_t del_len = del ? strlen(del) : 0;
    char * res = realloc(*a, a_len + b_len + del_len + 1);

    if (!ISSET(res)) exit(EXIT_FAILURE);

    memcpy(res + a_len, del, del_len);
    memcpy(res + a_len + del_len, b, b_len);
    res[a_len + del_len + b_len + 1] = 0;
    *a = res;
}
