#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>

void
print_err();

int
main( int argc , char** argv )
{
	if ( argc == 1 )
	{
		fprintf( stderr , "Source is not specified.\n" );

		return 1;
	}

	if ( argc == 2 ) 
	{
		fprintf( stderr , "Destination not specified.\n" );

		return 1;
	}

	if ( argc > 3 )
	{
		fprintf( stderr , "Too many arguments.\n" );

		return 1;
	}

	struct stat src_info;

	if ( stat( argv[ 1 ] , &src_info ) )
	{
		print_err();

		return 1;
	}

	if ( S_ISDIR( src_info.st_mode ) )
	{
		fprintf( stderr , "%s is directory.\n" , argv[ 1 ] );

		return 1;
	}

	if ( S_ISSOCK( src_info.st_mode ) )
	{
		fprintf( stderr , "%s is socket.\n" , argv[ 1 ] );

		return 1;
	}

	int is_not_dir = 0;
	struct stat dst_info;
	
	if ( stat( argv[ 2 ] , &dst_info ) == -1 )
	{
		if ( errno == ENOENT )
			is_not_dir = 1;
		else
			print_err();
	}


	int src_fd; 

	if ( ( src_fd = open( argv[ 1 ] , O_RDONLY ) ) == -1 )
	{
		print_err();

		return 1;
	}

	const char* filename =  argv[ 2 ];

	if ( !is_not_dir && S_ISDIR( dst_info.st_mode ) ) 
	{
		if ( chdir( argv[ 2 ] ) == -1 )
		{
			print_err();

			return 1;
		}
		
		filename = basename( argv[ 1 ] );

		if ( stat( filename , &dst_info) == -1 )
		{
			if ( errno != ENOENT )
			{
				print_err();

				return 1;
			}
		}
	}

	if ( src_info.st_ino == dst_info.st_ino )
	{
		fprintf(
			stderr ,
			"%s and %s are hard linked to the same inode.\n" ,
			argv[ 1 ] ,
			filename
		);

		return 1;
	}

	int dst_fd;

	if ( ( dst_fd = open( filename , O_CREAT | O_TRUNC | O_WRONLY , src_info.st_mode ) ) == -1  )
	{
		print_err();

		return 1;
	}

	char buffer[ src_info.st_blksize ];
	int  read_bytes = 0;

	while( ( read_bytes = read( src_fd , buffer , src_info.st_blksize ) ) )
	{
		if ( read_bytes == -1 )
		{
			print_err();

			return 1;
		}

		if ( write( dst_fd , buffer , read_bytes ) == -1 )
		{
			print_err();

			return 1;
		}
	}

	return 0;
}

void
print_err()
{
	fprintf( stderr , "%s\n" , strerror( errno ) );
}
