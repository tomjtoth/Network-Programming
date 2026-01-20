#include <stdio.h>
#include <stdlib.h>

const int BUFSIZE = 1024;

int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    char buf[BUFSIZE];

    if (argc > 1)
    {
        // open file (passed arg)
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            // abort on failure
            fprintf(stderr, "  Failed to open '%s'.\n", argv[1]);
            return 1;
        }
    }
    else
    {
        fp = stdin;
    }

    // try reading a line
    while (fgets(buf, BUFSIZE, fp) != NULL)
    {
        // if succeeded, print twice
        fputs(buf, stdout);
        fputs(buf, stdout);
    }

    if (fp != stdin)
    {
        fclose(fp);
    }

    return 0;
}
