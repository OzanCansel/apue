#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

void not_implemented_yet( char opt );
const char** partition_existents( const char** beg , const char** end );
const char** partition_files_and_dirs( const char** beg , const char** end );
int cmpstring( const void* x , const void* y );
int  count_files_in_dir( const char* );
void print_dir_content( const char* );
void print_file_entry( const char* );
int is_hidden( const char* filename );
int is_last_itr( void* itr , void* end , int size );

struct
{
    int multiple_entries;
    int show_hidden;
} opts = {
    .multiple_entries = 0 ,
    .show_hidden      = 0
};

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

    opts.multiple_entries = end - begin != 1;
    int at_least_a_file_and_a_dir = opts.multiple_entries && ( dirs_end - dirs_begin ) >= 1;

    const char** itr = begin;
    for ( ; itr != files_end; ++itr )
        print_file_entry( *itr );

    itr = dirs_begin;
    for ( ; itr != dirs_end; ++itr )
    {
        print_dir_content( *itr );

        if ( at_least_a_file_and_a_dir && !is_last_itr( itr , dirs_end , sizeof( itr ) ) )
            printf( "\n\n" );
    }
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

            ++beg;
        }
    }

    return beg;
}

int
cmpstring( const void* x , const void* y )
{
    return strcmp( *(const char**)x , *(const char**)y );
}

int
count_files_in_dir( const char* dir )
{
    int  n_files = 0;
    DIR* dir_p;

    if ( ( dir_p = opendir( dir ) ) )
    {
        struct dirent* entry;

        while( ( entry = readdir( dir_p ) ) )
        {
            if ( !opts.show_hidden && is_hidden( entry->d_name ) )
                continue;

            ++n_files;
        }

        closedir( dir_p );
    }
    else
    {
        perror( __func__ );

        return -1;
    }

    return n_files;
}

void
print_dir_content( const char* dir )
{
    int n_files = count_files_in_dir( dir );
    char** file_names  = ( char** )malloc( sizeof( const char* ) * n_files );
    const char** begin = ( const char** )file_names;
    DIR* dir_p;

    if ( ( dir_p = opendir( dir ) ) )
    {
        struct dirent* entry;

        while( ( entry = readdir( dir_p ) ) )
        {
            if ( !opts.show_hidden && is_hidden( entry->d_name ) )
                continue;

            int len         = strlen( entry->d_name );
            char* fname_str = malloc( len );

            strcpy( fname_str , entry->d_name );

            *begin = fname_str;
            ++begin;
        }

        closedir( dir_p );
    }
    else
    {
        perror( __func__ );
    }

    qsort( file_names , n_files , sizeof( char* ) , cmpstring );

    if ( opts.multiple_entries )
        printf( "%s:\n" , dir );

    int i;
    for ( i = 0; i < n_files; ++i )
        print_file_entry( file_names[ i ] );

    if ( n_files )
        printf( "\n" );

    for ( i = 0; i < n_files; ++i )
        free( file_names[ i ] );

    free( file_names );
}

void
print_file_entry( const char* fname )
{
    printf( "%s  " , fname );
}

int
is_hidden( const char* filename )
{
    return filename[ 0 ] == '.';
}

int
is_last_itr( void* itr , void* end , int size )
{
    return ( ( end - itr ) / size ) == 1;
}