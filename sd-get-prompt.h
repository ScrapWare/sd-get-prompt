#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>
#include <errno.h>

#include <gtk/gtk.h>

#define APPNAME "sd-get-prompt"
#define APPLABL "SD Created Info"

#define STR_NEG "\nNegative prompt:\x20"
#define STR_INF "\nSteps:\x20"

#define PMG_FHDR "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\0"
#define PNG_IHDR "\x00\x00\x00\x0d\x49\x48\x44\x52\0"

#define ERR_NO_ARGC "Few to arguments!\0"
#define ERR_NO_OPEN "Failed open file!\0"
#define ERR_NO_READ "Failed read file!\0"
#define ERR_NO_ALOC "Failed allocation memory!\0"
#define ERR_NO_PNGH "Invalid PNG signature!\0"
#define ERR_NO_IHDR "Invalid IHDR chunk!\0"
#define ERR_NO_tEXt "Not a tEXt chunk!\0"
#define ERR_NO_PARA "Not a Stable Diffusion Parameters!\0"

#define ERR_NG_INFO "Invalid information text!\0"
#define ERR_NG_NGTV "Invalid negative prompt!\0"

#define SD_tEXt_Param "parameters\0"
#define SD_tEXt_TsArt "generation_data\0"

#define SD_tEXt_Stable "tEXt\0"
#define SD_tEXt_Meitu1 "iTXt\0"
#define SD_tEXt_Meitu2 "rTXt\0"
#define SD_Made_Stable "SD tEXt\0"
#define SD_Made_Meitu1 "Meitu iTXt\0"
#define SD_Made_Meitu2 "Meitu rTXt\0"
#define SD_Made_TsrArt "TensorArt JSON\0"

typedef struct _sHash{
  char *key;
  char *value;
} shash, *pshash;

typedef struct _ihdr{
  int  width;
  int  height;
  char depth;
  char color;
  char compression;
  char filter;
  char interlace;
  int  ihdr_crc;
} ihdr, *pihdr;

typedef struct _tEXtP{
  char  tEXt[5];
  char *made;
  int   size;
  char *param;
  int   tEXt_crc;
  char *eMEs;
} tEXtP, *ptEXtP;

typedef struct _SDtEXt{
  /* IHDR */
  int   width;
  int   height;
  char  depth;
  char  color;
  char  compression;
  char  filter;
  char  interlace;
  int   ihdr_crc;
  /* tEXt */
  char  tEXt[5];
  char *made;
  int   size;
  char *param;
  int   tEXt_crc;
  char *eMEs;
} SDtEXt, *pSDtEXt;

/* End of Header */
