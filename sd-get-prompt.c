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
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  if(strcmp(buf, PMG_FHDR) != 0){
    gTXt.eMEs = ERR_NO_PNGH;
    return gTXt;
  }

  /* IHDR Chunk */
  if(! fread(buf, 1, 8, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  if(strcmp(buf, PNG_IHDR) != 0){
    gTXt.eMEs = ERR_NO_IHDR;
    return gTXt;
  }

  /* Width IHDR Chunk */
  if(! fread(&i, 4, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  gTXt.width = bswap_32(i);

  /* Height IHDR Chunk */
  if(! fread(&i, 4, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  gTXt.height = bswap_32(i);

  /* Depth IHDR Chunk */
  if(! fread(&gTXt.depth, 1, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Color IHDR Chunk */
  if(! fread(&gTXt.color, 1, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Compression IHDR Chunk */
  if(! fread(&gTXt.compression, 1, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Filter IHDR Chunk */
  if(! fread(&gTXt.filter, 1, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Interlace IHDR Chunk */
  if(! fread(&gTXt.interlace, 1, 1, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* CRC IHDR Chunk */
  if(! fread(&gTXt.ihdr_crc, 1, 4, fp)){
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
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  gTXt.size = bswap_32(i);

  /* Identifier tEXt Chunk */
  if(! fread(&gTXt.tEXt, 1, 4, fp)){
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
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Check Parameters of tEXt Chunk */
  if(strncmp(gTXt.param, SD_tEXt_Param, 11) != 0){
    gTXt.eMEs = ERR_NO_PARA;
    return gTXt;
  }

  /* CRC IHDR Chunk */
  if(! fread(&gTXt.tEXt_crc, 1, 4, fp)){
    gTXt.eMEs = ERR_NO_READ;
    return gTXt;
  }

  /* Close */
  fclose(fp);

  return gTXt;

}

int iTXt_dialog(int GTK_MESSAGE_TYPE, const char *msg){

  int result;

  /*if(GTK_MESSAGE_TYPE != GTK_MESSAGE_INFO){
    fprintf(stderr, "%s: %s\n", APPNAME, msg);
  } else{
    fprintf(stdout, "%s: %s\n", APPNAME, msg);
  }*/

  GtkWidget *iDlg = gtk_message_dialog_new(
    NULL,
    GTK_DIALOG_MODAL,
    GTK_MESSAGE_TYPE,
    GTK_BUTTONS_OK,
    APPLABL
  );

  gtk_window_set_title(GTK_WINDOW(iDlg), APPNAME);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(iDlg), msg);
  result = gtk_dialog_run(GTK_DIALOG(iDlg));
  gtk_widget_destroy(iDlg);

  return result;

}

int main(int argc, char *argv[]){

  SDtEXt iTXt;

  char *neg;
  char *inf;
  char *tmp;
  char *dsp;

  int lens[3];

  int ret;

  gtk_init(&argc, &argv);

  if(argc <2){
    iTXt_dialog(GTK_MESSAGE_ERROR, ERR_NO_ARGC);
    return 1;
  }

  if(strncmp(argv[1], "file://", 7) == 0){
    iTXt = get_tEXt(argv[1]+7);
  } else{
    iTXt = get_tEXt(argv[1]);
  }

  if(iTXt.eMEs != NULL){
    if(iTXt.param != NULL){ free(iTXt.param); }
    iTXt_dialog(GTK_MESSAGE_WARNING, iTXt.eMEs);
    return 1;
  }

  tmp = iTXt.param + 11;

  /* Search Info */
  if((inf = strstr(tmp, STR_INF)) == NULL){
    if(iTXt.param != NULL){ free(iTXt.param); }
    iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NG_INFO);
    return 1;
  }

  *inf = 0; inf++;
  lens[2] = strlen(inf);

  /* Search Negative */
  if((neg = strstr(tmp, STR_NEG)) == NULL){
    if(iTXt.param != NULL){ free(iTXt.param); }
    iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NG_NGTV);
    return 1;
  }

  *neg = 0; neg++;
  lens[1] = strlen(neg);

  lens[0] = strlen(tmp);

  /* Malloc */
  dsp = (char*)malloc(sizeof(char)*(lens[0]+lens[2]+3));
  /* Copy */
  strcpy(dsp, tmp);
  strcat(dsp, "\n\n");
  strcat(dsp, inf);

  free(iTXt.param);

  iTXt_dialog(GTK_MESSAGE_INFO, dsp);

  free(dsp);

  return 0;

}

/* __END__ */
