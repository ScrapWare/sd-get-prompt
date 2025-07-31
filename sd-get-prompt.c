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

  #if GTK_MAJOR_VERSION >= 4
  GdkClipboard *clip;
  #else
  GtkClipboard *clip;
  #endif
     GtkWidget *iDlg;
           int  rslt;

  #if GTK_MAJOR_VERSION >= 3
       GtkWidget *area;
           GList *children, *l;
  #endif

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
  #if GTK_MAJOR_VERSION >= 4
  gtk_window_set_position(GTK_WINDOW(iDlg), 1);
  #else
  gtk_window_set_position(GTK_WINDOW(iDlg), GTK_WIN_POS_CENTER);
  #endif
  gtk_window_set_title(GTK_WINDOW(iDlg), APPNAME);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(iDlg), msg);

  #if GTK_MAJOR_VERSION >= 3
  area = gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(iDlg));
  children = gtk_container_get_children(GTK_CONTAINER(area));

  for(l = children; l != NULL; l = g_list_next(l)){
    GtkWidget *child = GTK_WIDGET(l->data);
    if(GTK_IS_LABEL(child)){
      gtk_label_set_selectable(GTK_LABEL(child), TRUE);
      /* if want to target only the first GtkLabel found, break here; */
      // break;
    }
  }
  // GListは解放する必要がある
  g_list_free(children);
  #endif

  rslt = gtk_dialog_run(GTK_DIALOG(iDlg));

  #if GTK_MAJOR_VERSION >= 4
  clip = gtk_clipboard_get(GDK_TYPE_CLIPBOARD);
  #else
  clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  #endif
  gtk_clipboard_store(clip);

  gtk_widget_destroy(iDlg);

  return rslt;

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

  #if GTK_MAJOR_VERSION >= 4
  gtk_init();
  #else
  gtk_init(&argc, &argv);
  #endif

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

    if(FakeJSONdecode(dsp, tmp, lens[0]) == 0){
      free(dsp);
      iTXt_dialog(GTK_MESSAGE_WARNING, ERR_NO_DECS);
      return 1;
    }

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
