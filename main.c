#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wish.h"

int main(int argc, char *argv[])
{
  if(argc == 1)
  {
    interactive();
  } 
  else if(argc == 2)
  {
    batch(argv[1]);
  }
  
  return 0;
}