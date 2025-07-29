#include <stdio.h>
#include <stdlib.h> // For exit
#include <string.h> // For strlen, memcpy, sscanf

#define false 0
#define true 1

enum eight_forms{
  empty,
  type,
  enums,
  elements,
  properties,
  values,
  discriminator,
  ref
};

enum key_type{
  boolean,
  string,
  floats,
  ints,
  array,
  nest
};

typedef struct _sHash{
   int  type;
  char *key;
  char *value;
} shash, *pshash;

extern void local_on_error(const char *msg);

extern unsigned char conv_hex(unsigned char chr);
extern void decode_url_string(char *ustr);

extern int udec(char *buff, char *text, int slen);

/* Fake JSON Decoder for Tensor.Art */
extern int FakeJSONdecode(char *buff, char *text, int slen);
