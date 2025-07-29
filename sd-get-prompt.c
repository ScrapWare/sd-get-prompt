#include <sd-get-prompt.h>

SDtEXt get_tEXt(char *path){

  SDtEXt gTXt = {
    0, 0, 0, 0, 0, 0, 0, 0, "\0\0\0\0\0", "\0", 0, NULL, 0, NULL
  };
  char   buf[9] = "\0\0\0\0\0\0\0\0\0";
  FILE  *fp;
   int   i;

  /* Open */
  fp = fopen(path, "rb");

  if(fp == NULL){
    gTXt.eMEs = ERR_NO_OPEN;
    return gTXt;
  }

  /* PNG Header */
  if(! fread(buf, 1, 8, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  if(strcmp(buf, PMG_FHDR) != 0){
    fclose(fp);
    gTXt.eMEs = ERR_NO_PNGH;
    return gTXt;
  }

  /* IHDR Chunk */
  if(! fread(buf, 1, 8, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  if(strcmp(buf, PNG_IHDR) != 0){
    fclose(fp);
    gTXt.eMEs = ERR_NO_IHDR;
    return gTXt;
  }

  /* Width IHDR Chunk */
  if(! fread(&i, 4, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  gTXt.width = bswap_32(i);

  /* Height IHDR Chunk */
  if(! fread(&i, 4, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  gTXt.height = bswap_32(i);

  /* Depth IHDR Chunk */
  if(! fread(&gTXt.depth, 1, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Color IHDR Chunk */
  if(! fread(&gTXt.color, 1, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Compression IHDR Chunk */
  if(! fread(&gTXt.compression, 1, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Filter IHDR Chunk */
  if(! fread(&gTXt.filter, 1, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Interlace IHDR Chunk */
  if(! fread(&gTXt.interlace, 1, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* CRC IHDR Chunk */
  if(! fread(&gTXt.ihdr_crc, 1, 4, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* tEXt Chunk 
  \0\0\0\0tEXtparameters(Prompt)\n
  Negative prompt:(Negative Prompt)\n
  Steps: ~~~~~~~~ Version: version(\0)
  */

  /* Size of tEXt Chunk */
  if(! fread(&i, 4, 1, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  gTXt.size = bswap_32(i);

  /* Identifier tEXt Chunk */
  if(! fread(&gTXt.tEXt, 1, 4, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Check tEXt identifier */
  if(strcmp(gTXt.tEXt, SD_tEXt_Stable) == 0){
    gTXt.made = SD_Made_Stable;
  } else if(strcmp(gTXt.tEXt, SD_tEXt_Meitu1) == 0){
    gTXt.made = SD_Made_Meitu1;
  } else if(strcmp(gTXt.tEXt, SD_tEXt_Meitu2) == 0){
    gTXt.made = SD_Made_Meitu2;
  } else{
    gTXt.eMEs = ERR_NO_tEXt;
    return gTXt;
  }

  /* Malloc */
  gTXt.param = (char*)malloc(sizeof(char)*gTXt.size+1);

  if(gTXt.param == NULL){
    gTXt.eMEs = ERR_NO_ALOC;
    return gTXt;
  }

  /* Parameters of tEXt Chunk */
  if(! fread(gTXt.param, 1, gTXt.size, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Check Parameters of tEXt Chunk */
  if(strncmp(gTXt.param, SD_tEXt_Param, 11) == 0){
    gTXt.made = SD_Made_Stable;
  } else if(strncmp(gTXt.param, SD_tEXt_TsArt, 16) == 0){
    gTXt.made = SD_Made_TsrArt;
    /* JSON*/
  } else{
    gTXt.eMEs = ERR_NO_PARA;
    return gTXt;
  }

  /* CRC IHDR Chunk */
  if(! fread(&gTXt.tEXt_crc, 1, 4, fp)){
    fclose(fp);
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Close */
  fclose(fp);

  return gTXt;

}

int iTXt_dialog(int GTK_MESSAGE_TYPE, const char *msg){

  GtkClipboard *clip;
     GtkWidget *iDlg;

  if(GTK_MESSAGE_TYPE == GTK_MESSAGE_ERROR){
    fprintf(stderr, "%s: %s\n", APPNAME, msg);
  } else{
    fprintf(stdout, "%s: %s\n", APPNAME, msg);
  }

  iDlg = gtk_message_dialog_new(
    NULL,
    GTK_DIALOG_MODAL,
    GTK_MESSAGE_TYPE,
    GTK_BUTTONS_OK,
    APPLABL
  );

  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iDlg), 0);
  gtk_window_set_position(GTK_WINDOW(iDlg), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(iDlg), APPNAME);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(iDlg), msg);

  gtk_dialog_run(GTK_DIALOG(iDlg));

  clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_clipboard_store(clip);

  gtk_widget_destroy(iDlg);

  return 0;

}

char *join_args(int argc, char *argv[]){

  char *argj;
   int  argl = 0;
   int  i;

  /* Ignoring file:// */
  if(strncmp(argv[1], "file://", 7) == 0){
    argv[1] += 7;
  }

  for(i=1;i<argc;i++){
    argl += strlen(argv[i]);
  }

  argj = (char*)malloc(sizeof(char)*(argl+argc+1));

  if(argj == NULL) return argv[1];

  strcpy(argj, argv[1]);

  for(i=2;i<argc;i++){
    strcat(argj, "\x20");
    strcat(argj, argv[i]);
  }

  return argj;

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

void FakeJSONdecode(char *buff, char *text, int slen){
  char tkey[slen];
  char tvar[slen];
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
      return;
    }
    /* Start Keys '"' */
    if(text[i] == '\x22' || text[i] == '\x27'){
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
          /* strcat */
          strcat(buff, tkey);
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
          /* '' */
          else if(f == '\x27'){ f = '\x27'; j+=1; }
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
              /* strcpy */
              strcat(buff, tvar);
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
                return; /* Last? */
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
        } else if(text[j] != '\x22' && text[j] != '\x27'){
          tkey[t++] = text[j];
        }
      }
    } else if(text[i] != '\x20'){
      fprintf(stderr, "[%c(%d)]\n%s", text[i], text[i], buff);
      iTXt_dialog(GTK_MESSAGE_ERROR, "Maybe need this placed \\x20(SPACE)");
      exit(1);
    }
  }
}

int main(int argc, char *argv[]){

  SDtEXt  iTXt;

    char *args;

    char *neg;
    char *inf;
    char *tmp;
    char *dsp;

     int  ret = 0;
     int  lens[4];
     int  i, j;

  gtk_init(&argc, &argv);

  if(argc <2){
    iTXt_dialog(GTK_MESSAGE_ERROR, ERR_NO_ARGC);
    return 1;
  }

  args = join_args(argc, argv);
  decode_url_string(args);
  iTXt = get_tEXt(args);
  free(args);

  if(iTXt.eMEs != NULL){
    if(iTXt.param != NULL) free(iTXt.param);
    if(errno != 0) fprintf(stderr, "%s\n", strerror(errno));
    iTXt_dialog(GTK_MESSAGE_WARNING, iTXt.eMEs);
    return 1;
  }

  /* Tensor.Art */
  if(strcmp(iTXt.made, SD_Made_TsrArt) == 0){

    tmp = iTXt.param + 16;

    lens[0] = iTXt.size;

    /* Malloc */
    dsp = (char*)malloc(sizeof(char)*(lens[0]+1));

    if(dsp == NULL){
      free(iTXt.param);
      iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NO_ALOC);
      return 1;
    }

    FakeJSONdecode(dsp, tmp, lens[0]);
    fprintf(stderr, "%d(%d)\n", iTXt.size, strlen(dsp));
    //fprintf(stderr, "%s\n\n", dsp);
    //memcpy(dsp, tmp, lens[0]);
    //memcpy(dsp + lens[0], "\0", 1);

  /* Others (WebUI, Meitu...) */
  } else{

    tmp = iTXt.param + 11;

    if(strcmp(iTXt.tEXt, SD_tEXt_Meitu1) == 0 || strcmp(iTXt.tEXt, SD_tEXt_Meitu2) == 0){
      tmp += 4;
    }

    /* Search Info */
    if((inf = strstr(tmp, STR_INF)) == NULL){
      free(iTXt.param);
      iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NG_INFO);
      return 1;
    }

    *inf = 0; inf++;
    lens[3] = strlen(inf);

    /* Search Negative */
    if((neg = strstr(tmp, STR_NEG)) == NULL){
      free(iTXt.param);
      iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NG_NGTV);
      return 1;
    }

    *neg = 0; neg++;
    lens[2] = strlen(neg);
    lens[1] = strlen(tmp);
    lens[0] = lens[1] + lens[3] + 2;

    /* Malloc */
    dsp = (char*)malloc(sizeof(char)*(lens[0]+1));

    if(dsp == NULL){
      free(iTXt.param);
      iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NO_ALOC);
      return 1;
    }

    /* Copy */
    memcpy(dsp, tmp, lens[1]);
    memcpy(dsp + lens[1], "\n\n", 2);
    memcpy(dsp + lens[1] + 2, inf, lens[3]);
    memcpy(dsp + lens[0], "\0", 1);

  }

  free(iTXt.param);
  iTXt_dialog(GTK_MESSAGE_INFO, dsp);
  free(dsp);

  return 0;

}

/* __END__ */
