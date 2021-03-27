/*
	buggy parent: db9537d
	commit id: 8a286b63457628b0a55d395f14005f254512e27d
*/

#include "./common.h"

typedef void (*stinst_type)();

struct dict_struct
{
  char *word;
  struct dict_struct *next;
  stinst_type *code;
  int code_length;
  int code_end;
  int var;
};

typedef struct dict_struct dict_type;

typedef struct buffer
{
	char *ptr;
	unsigned long write_idx;
	unsigned long size;
} string_type;

typedef int *word_type;

string_type *ptr; /* and the buffer */

dict_type *root;

dict_type *newentry (char *word)
{
  dict_type *new_d = (dict_type *) malloc (sizeof (dict_type));
  new_d->word = word;
  new_d->next = root;
  root = new_d;
  new_d->code = (stinst_type *) malloc (sizeof (stinst_type));
  new_d->code_length = 1;
  new_d->code_end = 0;
  return new_d;
}

void
add_var (name)
     char *name;
{
  dict_type *new_d = newentry (name);
}

unsigned int
add_to_definition (entry, word)
     dict_type *entry;
     stinst_type word;
{
  if (entry->code_end == entry->code_length)
    {
      entry->code_length += 2;
      entry->code =
	(stinst_type *) realloc ((char *) (entry->code),
				 entry->code_length * sizeof (word_type));
    }
  entry->code[entry->code_end] = word;

  return entry->code_end++;
}

char *
nextword (string, word)
     char *string;
     char **word;

{
  char *word_start;
  int idx;
  char *dst;
  char *src;

  int length = 0;

	if (!string)
		return NULL;

  while (isspace ((unsigned char) *string) || *string == '-')
		{
      if (*string == '-')
				{
					while (*string && *string != '\n')
						string++;

				}
			else
				{
					string++;
				}
		}

	if (!*string)
		return 0;	

	/* *string is not NULL here */
	word_start = string;
	if (*string == '"')
		{
			do
				{
					string++;
					length++;
					if (*string == '\\')
						{
							string += 2;
							length += 2;
						}
				}
			while (*string != '"');
    }

  else
    {
      while (!isspace ((unsigned char) *string))
			{
				string++;
				length++;

			}
    }

  *word = (char *) malloc (length + 1); /* allocation site */

  dst = *word;
  src = word_start;

	for (idx = 0; idx < length; idx++)
    {
      if (src[idx] == '\\')
	switch (src[idx + 1])
	  {
	  case 'n':
	    *dst++ = '\n';
	    idx++;
	    break;
	  case '"':
	  case '\\':
	    *dst++ = src[idx + 1];
	    idx++;
	    break;
	  default:
	    *dst++ = '\\';
	    break;
	  }
      else
	*dst++ = src[idx];
    }
  *dst++ = 0;

  if (*string)
    return string + 1;
  else
    return 0;
}

void compile (char *string)
{
  /* Add words to the dictionary.  */
  char *word;
  string = nextword (string, &word);
  while (string && *string && word[0])
    {

			printf ("string: %s\n", string);
      if (strcmp (word, "var") == 0)
	{
	  string = nextword (string, &word);

		/* word escapes through root dict in add_var (newentry)  */
	  add_var (word);
	  string = nextword (string, &word); /* memory leak */
	}
      else if (word[0] == ':')
	{
	  dict_type *ptr;
	  /* Compile a word and add to dictionary.  */
		free(word);
	  string = nextword (string, &word);

		/* word escapes through ptr and root dict in newentry */
	  ptr = newentry (word);
	  string = nextword (string, &word); /* memory leak */

	  while (word[0] != ';')
	    {
	      switch (word[0])
					{
					case '"':
						/* got a string, embed magic push string
							 function */
						add_to_definition (ptr, (stinst_type) (word + 1));
						break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						/* Got a number, embedd the magic push number
							 function */
						__USE(ptr);
						/* memory leak */
						free(word);
						break;
					default:
						__USE(ptr);
						free(word);
						/* memory leak */
					}

				string = nextword (string, &word);
				if (!string)
					break;
	    }
		__USE(ptr);
	  string = nextword (string, &word);
	}
      else
	{
	  fprintf (stderr, "syntax error at %s\n", string - 1);
	}
    }

	/* memory leak */
}

int main()
{
	dict_type *dict;
	char *string = "var second : stored_in_dict 5 ; : next";

	compile(string);
	dict = root;

	while (dict != NULL) {
		printf("%s\n", dict->word);
		free(dict->word);
		dict = dict->next;
	}

}
