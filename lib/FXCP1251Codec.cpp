#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXMetaClass.h"
#include "FXTextCodec.h"
#include "FXCP1251Codec.h"

namespace FX {

FXIMPLEMENT(FXCP1251Codec,FXTextCodec,nullptr,0)


//// Created by codec tool on 03/25/2005 from: CP1251.TXT ////
static const unsigned short forward_data[256]={
   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
   112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
   1026, 1027, 8218, 1107, 8222, 8230, 8224, 8225, 8364, 8240, 1033, 8249, 1034, 1036, 1035, 1039,
   1106, 8216, 8217, 8220, 8221, 8226, 8211, 8212, 65533, 8482, 1113, 8250, 1114, 1116, 1115, 1119,
   160,  1038, 1118, 1032, 164,  1168, 166,  167,  1025, 169,  1028, 171,  172,  173,  174,  1031,
   176,  177,  1030, 1110, 1169, 181,  182,  183,  1105, 8470, 1108, 187,  1112, 1029, 1109, 1111,
   1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055,
   1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071,
   1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087,
   1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103,
  };


static const unsigned char reverse_plane[17]={
  0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  };

static const unsigned char reverse_pages[73]={
  0,  64, 74, 74, 74, 74, 74, 74, 137,74, 74, 74, 74, 74, 74, 74,
  74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
  74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
  74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
  74, 74, 74, 74, 74, 74, 74, 74, 74,
  };

static const unsigned short reverse_block[201]={
  0,   16,  32,  48,  64,  80,  96,  112, 128, 128, 144, 160, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  175, 191, 207, 223, 239, 255, 128, 128, 128, 271, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 284, 300, 316, 128, 128, 128,
  128, 128, 128, 327, 128, 128, 128, 128, 128, 128, 340, 354, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128,
  };

static const unsigned char reverse_data[370]={
   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
   112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   160,  26,   26,   26,   164,  26,   166,  167,  26,   169,  26,   171,  172,  173,  174,  26,
   176,  177,  26,   26,   26,   181,  182,  183,  26,   26,   26,   187,  26,   26,   26,   26,
   168,  128,  129,  170,  189,  178,  175,  163,  138,  140,  142,  141,  26,   161,  143,  192,
   193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207,  208,
   209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223,  224,
   225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  240,
   241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255,  26,
   184,  144,  131,  186,  190,  179,  191,  188,  154,  156,  158,  157,  26,   162,  159,  165,
   180,  26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   150,
   151,  26,   26,   26,   145,  146,  130,  26,   147,  148,  132,  26,   134,  135,  149,  26,
   26,   26,   133,  26,   26,   26,   26,   26,   26,   26,   26,   26,   137,  26,   26,   26,
   26,   26,   26,   26,   26,   139,  155,  26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,   26,   136,  26,   26,   26,   26,   26,   26,   185,  26,   26,   26,   26,   26,
   26,   26,   26,   26,   153,  26,   26,   26,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,
  };


FXint FXCP1251Codec::mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const {
  if(nsrc<1) return -1;
  wc=forward_data[(FXuchar)src[0]];
  return 1;
  }


FXint FXCP1251Codec::wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const {
  if(ndst<1) return -1;
  dst[0]=reverse_data[reverse_block[reverse_pages[reverse_plane[wc>>16]+((wc>>10)&63)]+((wc>>4)&63)]+(wc&15)];
  return 1;
  }

FXint FXCP1251Codec::mibEnum() const {
  return 2251;
  }


const FXchar* FXCP1251Codec::name() const {
  return "windows-1251";
  }


const FXchar* FXCP1251Codec::mimeName() const {
  return "windows-1251";
  }


const FXchar* const* FXCP1251Codec::aliases() const {
  static const FXchar *const list[]={"microsoft-cp1251","windows-1251","cp1251",nullptr};
  return list;
  }

}

