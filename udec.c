#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"

int main(int argc, char *argv[]){

  char *buff;
   int  slen;
   int  i;

  if(argc > 1){
    for(i=1; i<argc; i++){
      slen = strlen(argv[i]);
      buff = (char*)malloc(sizeof(char)*(slen + 1));
      if(buff == NULL){
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
      }
      if(udec(buff, argv[i], slen) ==0){
        fprintf(stderr, "Unicode decoding failed.\n");
        return 1;
      }
      printf("%s\n", buff);
      free(buff);
    }
  } else {
    printf("Usage: %s <json_string>\n", argv[0]);
    printf("Example: %s \"Hello\\\\nWorld\" \"\\u3042\\u3044\\u3046\" \"\\uD83D\\uDE00 Test Emoji\"\n", argv[0]);
  }
  return 0;
}
