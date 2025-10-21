#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void usage();

int
main(int argc, char** argv)
{
    char c;
    while((c = getopt(argc, argv, "ARh")) != -1)
    {
        switch (c)
        {
        case 'A':
            break;
        case 'R':
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
            break;
        case '?':
            usage();
            exit(EXIT_FAILURE);
            break;
        }
    }
    return EXIT_SUCCESS;
}

static void
usage()
{
    fprintf(stderr, "usage: lollipop in [-A|-R|-h] <devname>\n");
    return;
}
