#ifndef _LINEREADER
#define _LINEREADER

typedef struct _linereader *linereader;

struct _linereader {
    int fd;
    char *line_buffer;
    char *read_buffer;
    char *read_left_buffer;
    int read_left_count;
};

linereader linereader_create(int fd);
void linereader_free(linereader reader);
int linereader_readline(linereader reader);

#endif
