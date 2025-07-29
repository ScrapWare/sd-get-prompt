#include "decode.h"

void local_on_error(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
}

unsigned char conv_hex(unsigned char chr){

  unsigned char tmp;

  if( chr >= '0' && chr <= '9'){
    tmp = chr - '0';
  } else if(chr >= 'A' && chr <= 'F'){
    tmp = 10 + chr - 'A';
  } else if(chr >= 'a' && chr <= 'f'){
    tmp = 10 + chr - 'a';
  } else { chr = 0; }

  return tmp;

}

void decode_url_string(char *ustr){

  unsigned char *tmp = (unsigned char*)ustr;
  unsigned char chr[3];
            int  len;

  do {
    if(*tmp == '%' && *(tmp+1) != '\0' && *(tmp+2) !='\0'
      && ( *(tmp+1) >= '0' && *(tmp+1) <= '9' || *(tmp+1) >= 'A' && *(tmp+1) <= 'F' || *(tmp+1) >= 'a' && *(tmp+1) <= 'f' )
      && ( *(tmp+2) >= '0' && *(tmp+2) <= '9' || *(tmp+2) >= 'A' && *(tmp+2) <= 'F' || *(tmp+2) >= 'a' && *(tmp+2) <= 'f' )
      ){
      chr[0] = conv_hex(*(tmp+1));
      chr[1] = conv_hex(*(tmp+2));
      chr[2] = (chr[0]*16) + chr[1];
      len = strlen(tmp+3);
      *tmp = (chr[2] == 0)? ' ' : chr[2];
      memmove(tmp+1, tmp+3, len);
      memcpy(tmp+(len+1), "\0", 1);
    } else if(*tmp == '+'){
      *tmp = ' ';
    }
  } while(*(tmp++) != '\0');

}

int udec(char *buff, char *text, int slen){

  unsigned char unih[5];
  unsigned char unic[5];
  unsigned  int hexy = 0;
  unsigned  int surr = 0;
  unsigned  int join = 0;
            int i, j;
            int p = 0;
            int c = 0;

  /* Initialize Buffer */
  buff[0] = '\0';
  /* Loopie */
  for(i=0; i<slen; i++){
    unic[0] = '\0';
    c = 0;
    hexy = 0;
    /* Escape Seqyuence ? */
    if(text[i] == '\\'){
      /* Have Length ? */
      if(i + 1 >= slen){
        local_on_error("Incomplete escape sequence at end of string.");
        return false;
      }
      /* Switching */
      switch(text[i+1]){
        case '\\': buff[p++] = '\\'; i++; break;
        case '"':  buff[p++] = '"';  i++; break;
        case '/':  buff[p++] = '/';  i++; break;
        case 'b':  buff[p++] = '\x08'; i++; break; // BS
        case 'f':  buff[p++] = '\x0c'; i++; break; // Ff
        case 'n':  buff[p++] = '\x0a'; i++; break; // LF
        case 'r':  buff[p++] = '\x0d'; i++; break; // CR
        case 't':  buff[p++] = '\x09'; i++; break; // HT
        /* \uXXXX */
        case 'u':
          /* Have Length ? */
          if(i + 5 >= slen){
            local_on_error("Incomplete \\uXXXX escape sequence.");
            return false;
          }
          /* Extract Four Hex*/
          unih[0] = text[i+2];
          unih[1] = text[i+3];
          unih[2] = text[i+4];
          unih[3] = text[i+5];
          unih[4] = '\0';
          /* Hexdicimal */
          if(sscanf((char*)unih, "%x", &hexy) != 1){
            local_on_error("Invalid hex digits in \\uXXXX escape.");
            return false;
          }
          /* Surrogate Pair */
          if(surr != 0){
            if(hexy >= 0xDC00 && hexy <= 0xDFFF){
              join = 0x10000 + (surr - 0xD800) * 0x400 + (hexy - 0xDC00);
              unic[c++] = (unsigned char)(0xF0 | ((join >> 18) & 0x07));
              unic[c++] = (unsigned char)(0x80 | ((join >> 12) & 0x3F));
              unic[c++] = (unsigned char)(0x80 | ((join >> 6) & 0x3F));
              unic[c++] = (unsigned char)(0x80 | (join & 0x3F));
              unic[c] = '\0';
              surr = 0;
            } else {
              local_on_error("Invalid low surrogate in surrogate pair.");
              return false;
            }
          /* Flagging Surrogate Pair */
          } else if(hexy >= 0xD800 && hexy <= 0xDBFF){
            surr = hexy;
            i += 5;
            continue;
          /* Bad Surrogate Pair Value */
          } else if(hexy >= 0xDC00 && hexy <= 0xDFFF){
            local_on_error("Low surrogate found without a preceding high surrogate.");
            return false;
          /* Defaults */
          } else if(hexy < 0x80){
            if (hexy == 0x00) {
               buff[p++] = ' ';
            } else {
              unic[c++] = (unsigned char)hexy;
            }
            unic[c] = '\0';
          } else if(hexy < 0x800){
            unic[c++] = (unsigned char)(0xC0 | (hexy >> 6));
            unic[c++] = (unsigned char)(0x80 | (hexy & 0x3F));
            unic[c] = '\0';
          } else {
            unic[c++] = (unsigned char)(0xE0 | (hexy >> 12));
            unic[c++] = (unsigned char)(0x80 | ((hexy >> 6) & 0x3F));
            unic[c++] = (unsigned char)(0x80 | (hexy & 0x3F));
            unic[c] = '\0';
          }
          // Copie
          memcpy(buff + p, unic, c);
          p += c;
          i += 5;
          break;
        /* Invalid Sequence */
        default:
          local_on_error("Invalid escape sequence.");
          return false;
      }
    } else {
      if((unsigned char)text[i] >= 0x01 && (unsigned char)text[i] <= 0x1F){
        local_on_error("Unescaped control character in JSON string.");
        return false;
      }
      buff[p++] = text[i];
    }
  }
  buff[p] = '\0';
  /* ^^; */
  return true;

}

int FakeJSONdecode(char *buff, char *text, int slen){

  char tkey[slen];
  char tvar[slen];
  char temp[slen];
  char unih[5] = "\0\0\0\0\0";
  char unic[3] = "\0\0\0";
   int i, j, k, f;
   int l = 0;
   int p = 0;
   int t = 0;
   int v = 0;

  /* Init */
  buff[p] = '\0';
  /* start point '{' */
  if(text[0] == '\x7b'){
      text++;
  }
  for(i=0;i<slen;i++){
    /* end point '}' */
    if(text[i] == '\x7d'){
      return true;
    }
    /* Start Keys '"' */
    if(text[i] == '\x22'){
      f = text[i];
      /* Search End Keys '"' */
      for(j=i+1;j<slen;j++){
        /* Founded End Keys */
        if(l == 1){
          //fprintf(stderr, "[LOOP:\\x%X(%c)]\n\n", text[j], text[j]);
          i=j;
          l=0;
          break;
        /* Key Ends '":' */
        } else if(text[j] == f && text[j+1] == '\x3a'){
          /* Finded */
          //fprintf(stderr, "[FIND:\\x%X(%c)]\n", f, f);
          /* Tags */
          tkey[t++] = '\0';
          /* Decode \", \\, \/, \b, \f, \n, \r, \t, \uXXXX */
          if(udec(temp, tkey, strlen(tkey)) == 0){
            fprintf(stderr, "Unicode decoding failed.\n");
            return false;
          }
          /* strcat */
          strcat(buff, temp);
          strcat(buff, ": ");
          p+=t+2; /* Pointer + Temporary Pointer */
          t=0; /* Reset Temporary Pointer */
          /* skip ":(\x3a)" */
          j+=2;
          /* skip space */
          while(text[j] == '\x20'){ j++; }
          /* new period */
          f = text[j];
          /* "" */
          if(f == '\x22'){ f = '\x22'; j+=1; }
          /* [ ~ ] */
          else if(f == '\x5b'){ f = '\x5d'; strcat(buff, "["); p++; j+=1; }
          /* { ~ } */
          else if(f == '\x7b'){ f = '\x7d'; strcat(buff, "{"); p++; j+=1; }
          /* int, float, bool, null */
          else{ f = '\x2c'; }
          /* Sign */
          //fprintf(stderr, "[SIGN:\\x%X(%c)]\n", f, f);
          /* find \x22 \x5d \x7d */
          for(k=j;k<slen;k++){
            /* Values */
            if(
              /* "" or '' or [ ~ ] or { ~ } */
              (f != '\x2c' && text[k] == f && (text[k+1] == '\x2c' || text[k+1] == '\x7d'))
              /* int, float, bool, null */
              || f == '\x2c' && (text[k] == '\x2c' || text[k] == '\x7d')){
              /* Finded */
              //fprintf(stderr, "[FIND:\\x%X(%c)]\n", f, f);
              /* Vars */
              tvar[v++] = '\0';
              /* Decode \", \\, \/, \b, \f, \n, \r, \t, \uXXXX */
              if(udec(temp, tvar, strlen(tvar)) == 0){
                fprintf(stderr, "Unicode decoding failed.\n");
                return false;
              }
              /* strcpy */
              strcat(buff, temp);
              p+=v+2; /* Pointer + Temporary Pointer */
              v=0; /* Reset Temporary Pointer */
              /* Goody Return */
              if(strcmp(tkey, "prompt") == 0 || strcmp(tkey, "negativePrompt") == 0){
                  strcat(buff, "\n\n");
                  p+=2;
              } else if(f == '\x5d'){
                  strcat(buff, "], ");
                  p+=3;
              } else if(f == '\x7d'){
                  strcat(buff, "}, ");
                  p+=3;
              } else {
                  strcat(buff, ", ");
                  p+=2; /* Pointer + Temporary Pointer */
              }
              /* Not EBI */
              //fprintf(stderr, "[TAGS:%s]\n", tkey); /* Get Tags */
              //fprintf(stderr, "[VARS:%s]\n", tvar); /* Get Vars */
              //fprintf(stderr, "[BUFF: %s]\n", buff); /* Get Buff */
              /* Last? */
              if(f != '\x7d' && text[k] == '\x7d'){
                return true; /* Last? */
              } else{
                  /* skip ",(\x2c)" */
                  k += (f != '\x2c')? 1 : 0;
                  /* j = k */
                  j = k;
              }
              l=1;
              break;
            } else{
              tvar[v++] = text[k];
            }
          }
        } else if(text[j] != '\x22'){
          tkey[t++] = text[j];
        }
      }
    } else if(text[i] != '\x20'){
      fprintf(stderr, "Found not spece of [%c(%d)]\n%s", text[i], text[i], buff);
      return false;
    }
  }
  /* ^^; */
  return true;

}
