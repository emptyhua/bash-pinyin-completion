/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Author : emptyhua@gmail.com
 * Create : 2011.9.26
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>
#include "pinyin.h"
#include "utf8vector.h"
#include "linereader.h"

#ifdef DEBUG
#define MYLOG(x,ARGS...) fprintf (stderr, "[%s: %s (): line %d] "x"\n", __FILE__, __FUNCTION__, __LINE__, ##ARGS)
#else
#define MYLOG(x,ARGS...)
#endif //DEBUG

typedef enum {
    MatchModeFull,
    MatchModeFirstLetter
} MatchMode;

int match_line_with_keyword(const char *line, int line_length, const char *keyword, MatchMode mode)
{
    MYLOG("line_length %d", line_length);

    wchar_t line_char,keyword_char; 
    int match_hanzi_count = 0;

    utf8vector line_vector = utf8vector_create(line, line_length);
    utf8vector keyword_vector = utf8vector_create(keyword, -1);

    int keyword_length = utf8vector_uni_count(keyword_vector);
    int keyword_index = 0;
    wchar_t *keyword_uni = malloc(sizeof(wchar_t) * keyword_length);

    while ((keyword_char = utf8vector_next_unichar(keyword_vector)) != '\0')
    {
        keyword_uni[keyword_index] = keyword_char;
        keyword_index ++;
    }

    //printf("keyword length %d\n", keyword_length);

    int match_rt = 1;
    keyword_index = 0;

    while((line_char = utf8vector_next_unichar(line_vector)) != '\0'
            && keyword_index < keyword_length)
    {
        keyword_char = keyword_uni[keyword_index];
        //printf("keyword index %d\n", keyword_index);

        if (pinyin_ishanzi(line_char))
        {
            if (pinyin_ishanzi(keyword_char))
            {
                if (line_char != keyword_char)
                {
                    match_rt = 0;
                    break;
                }
            }
            else if (pinyin_isabc(keyword_char))
            {
                keyword_char = pinyin_lowercase(keyword_char);
                //printf("keyword_char %d\n", keyword_char);
                const char **pinyins;
                int count = pinyin_get_pinyins_by_unicode(line_char, &pinyins);
                if (mode == MatchModeFirstLetter)
                {
                    int finded = 0;
                    for (int i = 0; i < count; i++)
                    {
                        //printf("pinyin0 %d\n", pinyins[i][0]);
                        if (keyword_char == pinyins[i][0])
                        {
                            finded = 1;
                            //printf("matched !!!!\n");
                            break;
                        }
                    }

                    if (finded == 0)
                        match_rt = 0;
                    else
                        match_hanzi_count ++;
                }
                else if (mode == MatchModeFull)
                {
                    int finded = 0;
                    for (int i = 0; i < count; i++)
                    {
                        int kindex_start = keyword_index;
                        const char *pinyin = pinyins[i];
                        int j = 0;
                        char pinyin_char;

                        while ((pinyin_char = pinyin[j]) != '\0' && kindex_start < keyword_length)
                        {
                            if (pinyin_char != pinyin_lowercase(keyword_uni[kindex_start]))
                            {
                                break;
                            }
                            j++;
                            kindex_start ++;
                        }
                       
                        int matched = (pinyin_char == '\0');
                        
                        if (matched)
                        {
                            finded = 1;
                            keyword_index = kindex_start - 1;
                            break;
                        }
                    }

                    if (finded == 0)
                        match_rt = 0;
                    else
                        match_hanzi_count ++;
                }

                free(pinyins);

                if (match_rt == 0)
                    break;
            }
            else
            {
                //printf("not hanzi or abc %d\n", keyword_char);
                match_rt = 0;
                break;
            }
        }
        else
        {
            if (line_char != keyword_char)
            {
                match_rt = 0;
                break;
            }
        }

        keyword_index ++;
    }
    
    //keyword.length > line.length
    if (match_rt == 1 && keyword_index < keyword_length)
        match_rt = 0;

    free(keyword_uni);
    utf8vector_free(line_vector);
    utf8vector_free(keyword_vector);
    if (match_rt == 0)
        return -1;
    else
        return match_hanzi_count;
}

int main(int argc, char **argv)
{
    int show_match_count = 0;
    int match_firstletter = 0;
    int match_firstletter_only = 0;

    while(1)
    {
        static struct option long_options[] =  
        {
            {"help", no_argument, NULL, 'h'}, 
            {"show-match-count", no_argument, NULL, 'c'},
            {"firstletter", no_argument, NULL, 'f'},
            {"firstletter-only", no_argument, NULL, 'F'},
            {0,0,0,0}
        };
    
        int option_index = 0;
        int c = getopt_long(argc, argv, "hcfF", long_options, &option_index);

        if (c == -1) break;

        switch(c)
        {
            case 'h':
                printf("help\n");
                break;
            case 'c':
                show_match_count = 1;
                break;
            case 'f':
                match_firstletter = 1;
                break;
            case 'F':
                match_firstletter_only = 1;
                break;
            default:
                break;
        }
    }
    
    if (optind >= argc)
    {
        fprintf(stderr, "keyword missing\n");
        return 1;
    }
    
    char *keyword = argv[optind];

    linereader reader = linereader_create(STDIN_FILENO);
    int count;
    while ((count = linereader_readline(reader)) != -1)
    {
        const char *line = reader->line_buffer;
        int match_count = -1;
        if (!match_firstletter_only)
        {
            match_count = match_line_with_keyword(line, count, keyword, MatchModeFull);

            if (match_count == -1 && match_firstletter)
            {
                match_count = match_line_with_keyword(line, count, keyword, MatchModeFirstLetter);
            }

        }
        else
        {
            match_count = match_line_with_keyword(line, count, keyword, MatchModeFirstLetter);
        }

        if (match_count != -1)
        {
            if (show_match_count)
                printf("%d\t%.*s\n", match_count, count, line);
            else
                printf("%.*s\n", count, line);
        }
    }
    linereader_free(reader);
    return 0;
}
