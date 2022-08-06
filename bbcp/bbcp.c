#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>

void
check_args( int argc , char** argv );

int
main( int argc , char** argv )
{
    check_args( argc , argv );

    struct stat src_info;

    if ( stat( argv[ 1 ] , &src_info ) )
    {
        perror( getprogname() );

        return 1;
    }

    if ( S_ISDIR( src_info.st_mode ) )
    {
        fprintf(
            stderr ,
            "%s:%s is directory.\n" ,
            getprogname() ,
            argv[ 1 ]
        );

        return 1;
    }

    if ( S_ISSOCK( src_info.st_mode ) )
    {
        fprintf(
            stderr ,
            "%s:%s is socket.\n" ,
            getprogname() ,
            argv[ 1 ]
        );

        return 1;
    }

    int is_not_dir = 0;
    struct stat dst_info;

    if ( stat( argv[ 2 ] , &dst_info ) == -1 )
    {
        if ( errno == ENOENT )
            is_not_dir = 1;
        else
            perror( getprogname() );
    }


    int src_fd;

    if ( ( src_fd = open( argv[ 1 ] , O_RDONLY ) ) == -1 )
    {
        perror( getprogname() );

        return 1;
    }

    const char* dst =  argv[ 2 ];

    if ( !is_not_dir && S_ISDIR( dst_info.st_mode ) )
    {
        if ( chdir( argv[ 2 ] ) == -1 )
        {
            perror( getprogname() );

            return 1;
        }

        dst = basename( argv[ 1 ] );

        if ( stat( dst , &dst_info) == -1 )
        {
            if ( errno != ENOENT )
            {
                perror( getprogname() );

                return 1;
            }
        }
    }

    if ( src_info.st_ino == dst_info.st_ino )
    {
        fprintf(
            stderr ,
            "%s:%s and %s are hard linked to the same inode.\n" ,
            getprogname() ,
            argv[ 1 ] ,
            dst
        );

        return 1;
    }

    int dst_fd;

    if ( ( dst_fd = open( dst , O_CREAT | O_TRUNC | O_WRONLY , src_info.st_mode ) ) == -1  )
    {
        perror( getprogname() );

        return 1;
    }

    char buffer[ src_info.st_blksize ];
    int  read_bytes = 0;

    while ( ( read_bytes = read( src_fd , buffer , src_info.st_blksize ) ) )
    {
        if ( read_bytes == -1 )
        {
            perror( getprogname() );

            return 1;
        }

        if ( write( dst_fd , buffer , read_bytes ) == -1 )
        {
            perror( getprogname() );

            return 1;
        }
    }

    return 0;
}

void check_args( int argc , char** argv )
{
    if ( argc == 1 )
    {
        fprintf( stderr , "%s: Source not specified.\n" , getprogname() );

        exit( EXIT_FAILURE );
    }

    if ( argc == 2 )
    {
        fprintf( stderr , "Destination not specified.\n" );

        exit( EXIT_FAILURE );
    }

    if ( argc > 3 )
    {
        fprintf( stderr , "Too many arguments.\n" );

        exit( EXIT_FAILURE );
    }
}
