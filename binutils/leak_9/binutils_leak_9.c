/*
	buggy parent: 9d2cdc8
	commit id : aba19b625f34fb3d61263fe8044cf0c6d8804570
*/

#include "../include/common.h"
#include "../include/binutils.h"

#define INT_MAX       (int)(((unsigned int) ~0) >> 1)          /* 0x7FFFFFFF */ 
#define ISASCII(c) 1
#define ISLOWER(c) ('a' <= (c) && (c) <= 'z')
#define ISDIGIT(c) (ISASCII (c) && isdigit (c))

typedef struct string		/* Beware: these aren't required to be */
{				/*  '\0' terminated.  */
  char *b;			/* pointer to start of string */
  char *p;			/* pointer after last character */
  char *e;			/* pointer after end of allocated space */
} string;

static void
string_init (string *s)
{
  s->b = s->p = s->e = NULL;
}

static void
string_append (string *p, const char *s)
{
	/* We don't care about p in the modeled program
	*/
	__USE(s);
} 

char *
ada_demangle (const char *mangled)
{
	int len0;
	const char *p;
	char *d;
	char *demangled;

	/* Discard leading _ada, which is used for library level subprograms */
	if (strncmp (mangled, "_ada_", 5) == 0)
		mangled += 5;

	/* All ada unit names are lower-case */
	if (!ISLOWER (mangled[0]))
		goto unknown;


	len0 = strlen (mangled) + 7 + 1;
	demangled = XNEWVEC (char, len0); // allocation site

	d = demangled;
	p = mangled;

	while (1)
		{
			/* An entity name is expected */
			if (!ISLOWER (*p))
				{
					do
						/* An identifer, which is always lower case */
						*d++ = *p++;
					while (ISLOWER (*p) || ISDIGIT (*p)
							|| (p[0] == '_' && (ISLOWER (p[1]) || ISDIGIT (p[1]))));
				}

			else if (p[0] == 'O')
				{
					/* An operator name.  */
          static const char * const operators[][2] =
            {{"Oabs", "abs"},  {"Oand", "and"},    {"Omod", "mod"},
             {"Onot", "not"},  {"Oor", "or"},      {"Orem", "rem"},
             {"Oxor", "xor"},  {"Oeq", "="},       {"One", "/="},
             {"Olt", "<"},     {"Ole", "<="},      {"Ogt", ">"},
             {"Oge", ">="},    {"Oadd", "+"},      {"Osubtract", "-"},
             {"Oconcat", "&"}, {"Omultiply", "*"}, {"Odivide", "/"},
             {"Oexpon", "**"}, {NULL, NULL}};
          int k;

					for (k = 0; operators[k][0] != NULL; k++)
            {
              size_t slen = strlen (operators[k][0]);
              if (strncmp (p, operators[k][0], slen) == 0)
                {
                  p += slen;
                  slen = strlen (operators[k][1]);
                  *d++ = '"';
                  memcpy (d, operators[k][1], slen);
                  d += slen;
                  *d++ = '"';
                  break;
                }
            }
          /* Operator not found.  */
          if (operators[k][0] == NULL)
            goto unknown;
        }

      else
        {
          /* Not a GNAT encoding.  */
          goto unknown;
        }
	
      /* The name can be directly followed by some uppercase letters.  */
      if (p[0] == 'T' && p[1] == 'K')
        {
          /* Task stuff.  */
          if (p[2] == 'B' && p[3] == 0)
            {
              /* Subprogram for task body.  */
              break;
            }
          else if (p[2] == '_' && p[3] == '_')
            {
              /* Inner declarations in a task.  */
              p += 4;
              *d++ = '.';
              continue;
            }
          else
            goto unknown;
        }
      if (p[0] == 'E' && p[1] == 0)
        {
          /* Exception name.  */
          goto unknown;
        }
      if ((p[0] == 'P' || p[0] == 'N') && p[1] == 0)
        {
          /* Protected type subprogram.  */
          break;
        }
      if ((*p == 'N' || *p == 'S') && p[1] == 0)
        {
          /* Enumerated type name table.  */
          goto unknown;
        }
      if (p[0] == 'X')
        {
          /* Body nested.  */
          p++;
          while (p[0] == 'n' || p[0] == 'b')
            p++;
        }
      if (p[0] == 'S' && p[1] != 0 && (p[2] == '_' || p[2] == 0))
        {
          /* Stream operations.  */
          const char *name;
          switch (p[1])
            {
            case 'R':
              name = "'Read";
              break;
            case 'W':
              name = "'Write";
              break;
            case 'I':
              name = "'Input";
              break;
            case 'O':
              name = "'Output";
              break;
            default:
              goto unknown;
            }
          p += 2;
          strcpy (d, name);
          d += strlen (name);
        }
      else if (p[0] == 'D')
        {
          /* Controlled type operation.  */
          const char *name;
          switch (p[1])
            {
            case 'F':
              name = ".Finalize";
              break;
            case 'A':
              name = ".Adjust";
              break;
            default:
              goto unknown;
            }
          strcpy (d, name);
          d += strlen (name);
          break;
        }

      if (p[0] == '_')
        {
          /* Separator.  */
          if (p[1] == '_')
            {
              /* Standard separator.  Handled first.  */
              p += 2;

              if (ISDIGIT (*p))
                {
                  /* Overloading number.  */
                  do
                    p++;
                  while (ISDIGIT (*p) || (p[0] == '_' && ISDIGIT (p[1])));
                  if (*p == 'X')
                    {
                      p++;
                      while (p[0] == 'n' || p[0] == 'b')
                        p++;
                    }
                }
              else if (p[0] == '_' && p[1] != '_')
                {
                  /* Special names.  */
                  static const char * const special[][2] = {
                    { "_elabb", "'Elab_Body" },
                    { "_elabs", "'Elab_Spec" },
                    { "_size", "'Size" },
                    { "_alignment", "'Alignment" },
                    { "_assign", ".\":=\"" },
                    { NULL, NULL }
                  };
                  int k;

                  for (k = 0; special[k][0] != NULL; k++)
                    {
                      size_t slen = strlen (special[k][0]);
                      if (strncmp (p, special[k][0], slen) == 0)
                        {
                          p += slen;
                          slen = strlen (special[k][1]);
                          memcpy (d, special[k][1], slen);
                          d += slen;
                          break;
                        }
                    }
                  if (special[k][0] != NULL)
                    break;
                  else
                    goto unknown;
                }
              else
                {
                  *d++ = '.';
                  continue;
                }
            }
          else if (p[1] == 'B' || p[1] == 'E')
            {
              /* Entry Body or barrier Evaluation.  */
              p += 2;
              while (ISDIGIT (*p))
                p++;
              if (p[0] == 's' && p[1] == 0)
                break;
              else
                goto unknown;
            }
          else
            goto unknown;
        }

      if (p[0] == '.' && ISDIGIT (p[1]))
        {
          /* Nested subprogram.  */
          p += 2;
          while (ISDIGIT (*p))
            p++;
        }
      if (*p == 0)
        {
          /* End of mangled name.  */
          break;
        }
      else
        goto unknown;
    }
  *d = 0;
  return demangled;

 unknown:
  len0 = strlen (mangled);
  demangled = XNEWVEC (char, len0 + 3); /* memory leak */

  if (mangled[0] == '<')
     strcpy (demangled, mangled);
  else
    sprintf (demangled, "<%s>", mangled);

  return demangled;
}

char *
cplus_demangle (const char *mangled)
{
	if (__RANDBOOL)
		return xstrdup (mangled);

	if (__RANDBOOL)
		return ada_demangle (mangled);
	
	return NULL;
}

void
demangle_template_value_parm (const char **mangled, string *s)
{
	int symbol_len = 3;

	if (__RANDBOOL)
		{
			char *p = XNEWVEC (char, symbol_len + 1), *q;
			strncpy (p, *mangled, symbol_len);
			p [symbol_len] = '\0';

			q = cplus_demangle (p);
			if (q)
				{
					string_append (s, q);
					free (q);
				}
			else {
				string_append (s, p);
				free(p);
			}
		}
		*mangled += symbol_len;

		return;
}

int main()
{
	time_t t;
	char *p;
	string s;
	const char *mangled = "123456";
	srand(time(&t));

	string_init(&s);
	demangle_template_value_parm(&mangled, &s);
}
	

