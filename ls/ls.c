#include <unistd.h>
#include <stdio.h>

void
not_implemented_yet( char opt )
{
    dprintf(
        STDERR_FILENO ,
        "'%c' option has not been implemented yet.\n" ,
        opt
    );
}

int
main( int argc , char** argv )
{
    int arg;

    while ( ( arg = getopt( argc , argv , "AacdFfhiklnqRrSstuw" ) ) != -1 )
        switch ( arg )
        {
            case 'A' :
            case 'a' :
            case 'c' :
            case 'd' :
            case 'F' :
            case 'f' :
            case 'h' :
            case 'i' :
            case 'k' :
            case 'l' :
            case 'n' :
            case 'q' :
            case 'R' :
            case 'r' :
            case 'S' :
            case 's' :
            case 't' :
            case 'u' :
            case 'w' :
                not_implemented_yet( arg );
        }
}