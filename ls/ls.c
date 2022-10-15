#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void not_implemented_yet( char opt );
const char** partition_existents( const char** beg , const char** end );
const char** partition_files_and_dirs( const char** beg , const char** end );
int cmpstring( const void* x , const void* y );

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

    const char** begin      = ( const char** )( argv + optind );
    const char** end        = ( const char** )( argv + argc );
    end                     = partition_existents( begin , end );
    const char** files_end  = partition_files_and_dirs( begin , end );
    const char** dirs_begin = files_end;
    const char** dirs_end   = end;

    qsort( begin      , files_end - begin     , sizeof( char* ) , cmpstring );
    qsort( dirs_begin , dirs_end - dirs_begin , sizeof( char* ) , cmpstring );

    for ( ; begin != end; ++begin )
        printf( "%s\n" , *begin );
}

void
not_implemented_yet( char opt )
{
    dprintf(
        STDERR_FILENO ,
        "'%c' option has not been implemented yet.\n" ,
        opt
    );
}

const char**
partition_existents( const char** beg , const char** end )
{
    while ( beg != end )
    {
        if ( access( *beg , F_OK ) == 0 )
        {
            beg++;
        }
        else
        {
            const char* temp = *beg;

            --end;
            *beg = *end;
            *end = temp;
        }
    }

    return end;
}

const char**
partition_files_and_dirs( const char** beg , const char** end )
{
    while ( beg != end )
    {
        struct stat sb;

        if ( stat( *beg , &sb ) != -1 )
        {
            if ( S_ISDIR( sb.st_mode ) )
            {
                const char* temp = *beg;

                --end;
                *beg = *end;
                *end = temp;
            }
            else
            {
                ++beg;
            }
        }
        else
        {
            perror( __func__ );
        }
    }

    return beg;
}

int
cmpstring( const void* x , const void* y )
{
    return strcmp( *(const char**)x , *(const char**)y );
}