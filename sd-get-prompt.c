#include <sd-get-prompt.h>

SDtEXt get_tEXt(char *path){

  SDtEXt  gTXt = {
    0, 0, 0, 0, 0, 0, 0, 0, "\0\0\0\0\0", "\0", 0, NULL, 0, NULL
  };

  char  buf[9] = "\0\0\0\0\0\0\0\0\0";

  FILE *fp;

  int i;

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
  } else if(strcmp( gTXt.tEXt, SD_tEXt_Meitu2) == 0){
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

void JSONdecode(char *buff, char *text, int slen){
  int i, j, k, l, f;
  int p = 0;
  /* start point */
  if(text[0] == '\x7b'){
      text++;
  }
  l=0;
  /* find \x22 */
  for(i=0;i<slen;i++){
    /* Ends */
    if(text[i] == '\x7d'){
      buff[p] = '\0';
      return;
    }
    /* Looped */
    if(l == 1){
      l=0;
      //fprintf(stderr, "[%s]\n", text+i);
    }
    /* Keys */
    if(text[i] == '\x22' || text[i] == '\x27'){
      f = text[i];
      /* Start Keys */
      for(j=i+1;j<slen;j++){
        /* Key Ends */
        if(l == 1){
          fprintf(stderr, "[L1:\\x%X(%c)]\n", text[j], text[j]);
          fprintf(stderr, "%s", "\n\n");
          i=j;
          break;
        } else if(text[j] == f && text[j+1] == '\x3a'){
          buff[p++] = '\x3a';
          buff[p++] = '\x0d';
          buff[p++] = '\x09';
          buff[p] = '\0';
          f = text[j+2];
          fprintf(stderr, "[FIND:\\x%X(%c)]\n", f, f);
          if(f == '\x22'){ f = '\x22'; j+=3; }
          else if(f == '\x5b'){ f = '\x5d'; j+=3; }
          else if(f == '\x7b'){ f = '\x7d'; j+=3; }
          else{ f = '\x2c'; j+=2; }
          fprintf(stderr, "[SIGN:\\x%X(%c)]\n", f, f);
          /* find \x22 \x5d \x7d */
          for(k=j;k<slen;k++){
            /* Values */
            if(((f != '\x2c' && text[k] == f && text[k+1] == '\x2c') || (f == '\x2c' && text[k] == f))
                || ((f != '\x2c' && text[k] == f && text[k+1] == '\x7d') || (f == '\x2c' && text[k] == '\x7d'))){
              fprintf(stderr, "%s", "\n");
              buff[p++] = '\x0d';
              buff[p] = '\0';
              if(f != '\x2c'){
                if(text[k+1] == '\x7d'){ return; }
                else{ j=k; }
              } else{
                if(text[k] == '\x7d'){ return; }
                else{ j=k-1; }
              }
              l=1;
              break;
            } else if(text[k] == '\x20'){
              fprintf(stderr, "%d", '\x20');
              buff[p++] = '\x20';
            } else{
              fprintf(stderr, "%c", text[k]);
              buff[p++] = text[k];
            }
          }
        } else if(text[j] != '\x22'){
          buff[p++] = text[j];
        }
      }
    } else if(text[i] != '\x20'){
      fprintf(stderr, "[%c(%d)]\n%s", text[i], text[i], buff);
      iTXt_dialog(GTK_MESSAGE_ERROR, "すぺえすではない");
      exit(1);
    }
  }
}

/*
tEXtgeneration_data.{"prompt":"(masterpiece), (best quality), (extremely detailed), (no humans), (completely features), 16k, samarkand blue mosque,","width":1024,"height":1024,"imageCount":1,"samplerName":"Euler a","steps":20,"cfgScale":6,"seed":"-1","clipSkip":2,"baseModel":{"label":"dev-fp8","type":"BASE_MODEL","modelId":"757279507095956705","modelFileId":"758377756002093254","modelFileName":"flux1-dev-fp8 (1)","baseModel":"FLUX.1","hash":"8E91B68084B53A7FC44ED2A3756D821E355AC1A7B6FE29BE760C1DB532F3D88A"},"sdVae":"animevae.pt","etaNoiseSeedDelta":31337,"sdxl":{},"ksamplerName":"euler_ancestral","schedule":"normal","guidance":3.5}
*/

int main(int argc, char *argv[]){

  SDtEXt iTXt;

  char *args;

  char *neg;
  char *inf;
  char *tmp;
  char *dsp;

  int ret = 0;

  int lens[4];

  int i, j;

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

    lens[0] = strlen(tmp);

    /* Malloc */
    dsp = (char*)malloc(sizeof(char)*(lens[0]+1));

    if(dsp == NULL){
      free(iTXt.param);
      iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NO_ALOC);
      return 1;
    }

    JSONdecode(dsp, tmp, lens[0]);
    fprintf(stderr, "%s", dsp);
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
