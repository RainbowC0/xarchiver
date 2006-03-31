#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "internals.h"

//Taken from xarchive - http://xarchive.sourceforge.net
int is_escaped_char (char c)
{
    switch ( c )
    {
        case ' ':
        case '\'':
        case '"':
        case '(':
        case ')':
        case '$':
        case '\\':
        case ';':
        case '<':
        case '>':
        case '&':
        case '#':
        case '*':
        case '|':
        case '`':
        case '!':
        return 1;
        default:
        return 0;
    }
}

gchar *escape_filename( gchar *string )
{
	char *q;
	char *escaped;
	int escapechars = 0;
	char *p = string;

	while (*p != '\000')
	{
        	if (is_escaped_char(*p)) escapechars++;
	        p++;
    }

	if (!escapechars) return g_strdup(string);
	escaped = (char *) g_malloc (strlen(string) + escapechars + 1);

	p = string;
	q = escaped;

	while (*p != '\000')
	{
        if (is_escaped_char(*p)) *q++ = '\\';
		*q++ = *p++;
	}
	*q = '\000';
	return escaped;
}

//End code from xarchive

GString *concatenatefilenames ( GSList *list )
{
    gchar *filename , *esc_filename;
    
    GString *names = g_string_new(" ");
    while( list )
	{
		filename = g_path_get_basename(list->data);
		esc_filename = escape_filename(filename);
		g_string_prepend(names, esc_filename);
		g_string_prepend_c(names, ' ');
		g_free(esc_filename);
		g_free(filename);
		list = g_slist_next(list);
	}
    return names;
}

int countcharacters ( gchar *string , int chr )
{
    int n = 0;
    while ( *string )
    {
        if ( *string == chr ) n++;
        string++;
    }
    return n;
}

GSList *split_line (GSList *dummy , gchar *line , unsigned short int n_fields)
{
	GSList *fields;
	gchar *scan, *field_end;
	gchar *field;
	int i;

	//fields = g_new0 (char *, n_fields + 1);
	//fields[n_fields] = NULL;

	scan = eat_spaces (line);
	for (i = 0; i < n_fields; i++)
	{
		field_end = strchr (scan, ' ');
		//The following line is mine, I added the case when the last field ends with a newline
		if (field_end == NULL) field_end = strchr (scan, '\n');
		if (field_end != NULL)
		{
			field = g_strndup (scan, field_end - scan);
			fields = g_slist_prepend ( fields, field );
			scan = eat_spaces (field_end);
		}
	}
	return fields;
}

gchar *eat_spaces (gchar *line)
{
	if (line == NULL)
		return NULL;
	while ((*line == ' ') && (*line != 0))
		line++;
	return line;
}

GSList *get_last_field (gchar *line,unsigned short int last_field)
{
	gchar *field;
	GSList *dummy;
	int i;

	if (line == NULL)
		return NULL;

	last_field--;
	field = eat_spaces (line);
	for (i = 0; i < last_field; i++)
	{
		if (field == NULL)
			return NULL;
		field = strchr (field, ' ');
		field = eat_spaces (field);
	}
    if (field != NULL) field [ strlen(field) -1 ] = '\000';
	dummy = g_slist_prepend ( dummy , field );
	return dummy;
}
