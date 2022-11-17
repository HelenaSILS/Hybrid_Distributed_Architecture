/* strtok example */
#include <stdio.h>
#include <string.h>

int main ()
{
  char str[] ="-This.a.sample string";
  char str2[]="$";
  char buf[50];
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str,".");
  while (pch != NULL)
  {
    if(strlen(pch)>0){
    strcat(buf, pch);
    strcat(buf, str2);
    }
    printf ("%s\n",pch);
    pch = strtok (NULL, " ,.-");
  }
    printf ("%s\n",buf);
  return 0;
}