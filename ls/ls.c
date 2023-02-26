#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void not_implemented_yet(char opt);
const char** partition_existents(const char** beg, const char** end);
const char** partition_files_and_dirs(const char** beg, const char** end);
int cmpstring(const void* x, const void* y);
int  count_files_in_dir(const char*);
void print_dir_content(const char*);
void print_file_entries(const char** begin, const char** end);
void print_file_entry(const char*);
int is_hidden(const char* filename);
int is_dot_or_dot_dot(const char* filename);
int is_last_itr(void* itr, void* end, int size);

struct
{
    int multiple_entries;
    int show_hidden;
    int show_dot_and_dot_dot;
    int dont_print_directory_content;
    int file_type_suffix;
    int output_is_not_sorted;
} opts = {
    .multiple_entries             = 0 ,
    .show_hidden                  = 0 ,
    .show_dot_and_dot_dot         = 1 ,
    .dont_print_directory_content = 0 ,
    .file_type_suffix             = 0 ,
    .output_is_not_sorted         = 0
};

int
main(int argc , char** argv)
{
    int arg;

    while ((arg = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1)
    {
        switch (arg)
        {
            case 'A' :
                opts.show_dot_and_dot_dot = 0;
            case 'a' :
                opts.show_hidden = 1;
                break;
            case 'd' :
                opts.dont_print_directory_content = 1;
                break;
            case 'F' :
                opts.file_type_suffix = 1;
                break;
            case 'f' :
                opts.output_is_not_sorted = 1;
                break;
            case 'c' :
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
                not_implemented_yet(arg);
        }
    }

    int n_args          = argc - optind;
    int args_array_size = n_args;

    if (!n_args)
        args_array_size++;

    char* files_dirs[args_array_size];

    if (!n_args)
    {
        files_dirs[0] = ".";
    }
    else
    {
        char** curr;
        char** dst;
        char** end;

        for (
                curr = argv + optind, end = argv + argc, dst = files_dirs;
                curr != end;
                ++curr, ++dst
            )
        {
            size_t src_len = strlen(*curr);
            *dst = malloc(src_len);
            strcpy(*dst, *curr);
        }
    }

    const char** begin      = (const char**)(files_dirs);
    const char** end        = (const char**)(files_dirs + args_array_size);
    end                     = partition_existents(begin, end);
    const char** files_end  = partition_files_and_dirs(begin, end);
    const char** dirs_begin = files_end;
    const char** dirs_end   = end;

    // Pointer visualization
    // [ file_1, file_2, dir_1, dir_2, dir_3 ]
    //  ^ begin         ^ files_end         ^ dirs_end
    //                    dirs_begin

    if (!opts.output_is_not_sorted)
    {
        qsort(begin     , files_end - begin    , sizeof(char*), cmpstring);
        qsort(dirs_begin, dirs_end - dirs_begin, sizeof(char*), cmpstring);
    }

    if (opts.dont_print_directory_content)
    {
        files_end  = dirs_end;
        dirs_begin = dirs_end;
    }

    opts.multiple_entries = end - begin != 1;
    int any_files         = begin != files_end;
    int any_dirs          = dirs_begin != dirs_end;
    int multiple_dirs     = opts.multiple_entries && (dirs_end - dirs_begin) >= 1;

    print_file_entries(begin, files_end);

    if (!any_dirs)
    {
        printf("\n");
    }
    else
    {
        if (any_files)
            printf("\n\n");

        const char** itr = dirs_begin;

        for ( ; itr != dirs_end; ++itr)
        {
            print_dir_content(*itr);

            if (multiple_dirs && !is_last_itr(itr, dirs_end, sizeof(itr)))
                printf("\n\n");
        }
    }

    {
        char** itr = files_dirs;
        char** end = files_dirs + n_args;

        for ( ; itr != end; ++itr)
            free(*itr);
    }
}

void
not_implemented_yet(char opt)
{
    dprintf(
        STDERR_FILENO ,
        "'%c' option has not been implemented yet.\n" ,
        opt
    );
}

const char**
partition_existents(const char** curr, const char** end)
{
    while (curr != end)
    {
        if (access(*curr, F_OK) == 0)
        {
            curr++;
        }
        else
        {
            int file_len = strlen(*curr);
            char error_prefix[file_len + 16];
            sprintf(error_prefix, "Cannot access '%s'", *curr);
            perror(error_prefix);

            const char* temp = *curr;

            --end;
            *curr = *end;
            *end = temp;
        }
    }

    return end;
}

const char**
partition_files_and_dirs(const char** curr, const char** end)
{
    while (curr != end)
    {
        struct stat sb;

        if (stat(*curr, &sb) != -1)
        {
            if (S_ISDIR(sb.st_mode) ^ opts.dont_print_directory_content)
            {
                const char* temp = *curr;

                --end;
                *curr = *end;
                *end = temp;
            }
            else
            {
                ++curr;
            }
        }
        else
        {
            perror(__func__);

            ++curr;
        }
    }

    return curr;
}

int
cmpstring(const void* x, const void* y)
{
    return strcmp(*(const char**)x ,*(const char**)y);
}

int
count_files_in_dir(const char* dir)
{
    int  n_files = 0;
    DIR* dir_p;

    if ((dir_p = opendir(dir)))
    {
        struct dirent* entry;

        while((entry = readdir( dir_p )))
        {
            if (!opts.show_hidden && is_hidden(entry->d_name))
                continue;

            if (!opts.show_dot_and_dot_dot && is_dot_or_dot_dot(entry->d_name))
                continue;

            ++n_files;
        }

        closedir(dir_p);
    }
    else
    {
        perror(__func__);

        return -1;
    }

    return n_files;
}

void
print_dir_content(const char* dir)
{
    int n_files        = count_files_in_dir(dir);
    char* file_names[n_files];
    const char** begin = (const char**)file_names;
    DIR* dir_p;

    if ((dir_p = opendir( dir )))
    {
        struct dirent* entry;

        while ((entry = readdir(dir_p)))
        {
            if (!opts.show_hidden && is_hidden(entry->d_name))
                continue;

            if (!opts.show_dot_and_dot_dot && is_dot_or_dot_dot(entry->d_name))
                continue;

            int len         = strlen(entry->d_name);
            char* fname_str = malloc(len);

            strcpy(fname_str, entry->d_name);

            *begin = fname_str;
            ++begin;
        }

        closedir(dir_p);
    }
    else
    {
        perror(__func__);
    }

    if (!opts.output_is_not_sorted)
        qsort(file_names, n_files, sizeof( char* ), cmpstring);

    if (opts.multiple_entries)
        printf("%s:\n", dir);

    print_file_entries((const char**)file_names, (const char**)file_names + n_files);

    if (n_files)
        printf("\n");

    int i;
    for (i = 0; i < n_files; ++i)
        free(file_names[i]);
}

void
print_file_entries(const char** itr, const char** end)
{
    for ( ; itr != end; ++itr)
        print_file_entry(*itr);
}

void
print_file_entry(const char* fname)
{
    char suffix[] = "\0\0";

    if (opts.file_type_suffix)
    {
        struct stat sb;

        if (lstat(fname, &sb) != -1)
        {
            if (S_ISDIR(sb.st_mode))
                suffix[0] = '/';
            else if(S_ISLNK(sb.st_mode))
                suffix[0] = '@';
            else if(sb.st_mode & S_IXUSR)
                suffix[0] = '*';
            else if(S_ISWHT(sb.st_mode))
                suffix[0] = '%';
            else if(S_ISSOCK(sb.st_mode))
                suffix[0] = '=';
            else if(S_ISFIFO(sb.st_mode))
                suffix[0] = '|';
        }
        else
        {
            perror(__func__);
        }
    }

    printf("%s%s  ", fname, suffix);
}

int
is_hidden(const char* filename)
{
    return filename[0] == '.';
}

int
is_dot_or_dot_dot(const char* filename)
{
    return (filename[0] == '.' && filename[1] == '\0') ||
           (filename[0] == '.' && filename[1] == '.' && filename[2] == '\0');
}

int
is_last_itr(void* itr, void* end, int size)
{
    return (( end - itr) / size) == 1;
}
