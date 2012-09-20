/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Author : emptyhua@gmail.com
 * Create : 2011.9.26
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "linereader.h"

#define read_buf_size 1024

linereader linereader_create(int fd)
{
    linereader reader = (linereader)calloc(1, sizeof(struct _linereader));
    reader->fd = fd;
    reader->read_buffer = (char *)malloc(sizeof(char) * read_buf_size);
    reader->read_left_buffer = (char *)malloc(sizeof(char) * read_buf_size);
    reader->line_buffer = NULL;
    return reader;
}

void linereader_free(linereader reader)
{
    free(reader->read_buffer);
    free(reader->read_left_buffer);
    free(reader->line_buffer);
    free(reader);
}

static int linereader_readbuffer(linereader reader)
{
    if (reader->read_left_count)
    {
        memcpy(reader->read_buffer, reader->read_left_buffer, reader->read_left_count);
        return reader->read_left_count;
    }
   
    int size = read(reader->fd, reader->read_buffer, read_buf_size);
    return size;
}

int linereader_readline(linereader reader)
{
    int bytes_count;
    int line_size = 0;

    if (reader->line_buffer)
    {
        free(reader->line_buffer);
        reader->line_buffer = NULL;
    }
    
    int read_count = 0;

    while ((bytes_count = linereader_readbuffer(reader)) != 0)
    {
        read_count ++;
        char *read_buffer = reader->read_buffer;
        int br_index = -1;
        
        for (int i = 0; i < bytes_count; i++)
        {
            if (read_buffer[i] == '\n')
            {
                br_index = i;
                break;
            }
        }
        
        //if \n finded
        if (br_index != -1)
        {
            int copy_count = br_index;

            if (copy_count)
            {
                reader->line_buffer = realloc(reader->line_buffer, sizeof(char) * (line_size + copy_count));
                memcpy(reader->line_buffer + line_size, read_buffer, copy_count);
                line_size += copy_count;
            }
            
            //save chars afer \n
            int left_count = bytes_count - br_index - 1;
            memcpy(reader->read_left_buffer, read_buffer + br_index + 1, left_count);
            reader->read_left_count = left_count;
            break;
        }
        //copy read buffer
        else
        {
            int copy_count = bytes_count;
            reader->line_buffer = realloc(reader->line_buffer, sizeof(char) * (line_size + copy_count));
            memcpy(reader->line_buffer + line_size, read_buffer, copy_count);
            line_size += copy_count;
            reader->read_left_count = 0;
        }
    }
    
    if (read_count == 0)
    {
        return -1;
    }
    else
    {
        return line_size;
    }
}

#ifdef LINEREADER_BIN
int main()
{
#ifdef DEBUG
    FILE *fp = fopen("./linereader.c", "r");
    int fd = fileno(fp);
    linereader reader = linereader_create(fd);  
    int count;
    char *output;
    while ((count = linereader_readline(reader)) != -1)
    {
        output = reader->line_buffer;
        printf("%.*s\n", count, output);
    }
    linereader_free(reader);
    fclose(fp);
#else
    linereader reader = linereader_create(STDIN_FILENO);  
    int count;
    char *output;
    while ((count = linereader_readline(reader)) != -1)
    {
        output = reader->line_buffer;
        printf("%.*s########\n", count, output);
    }
    linereader_free(reader);
#endif
    return 0;
}
#endif
