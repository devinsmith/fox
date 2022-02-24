#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXArray.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXTextCodec.h"
#include "FX885910Codec.h"

namespace FX {

FXIMPLEMENT(FX885910Codec,FXTextCodec,nullptr,0)


//// Created by codec tool on 03/25/2005 from: 8859-10.TXT ////
static const unsigned short forward_data[256]={
   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
   112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
   128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,
   144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,
   160,  260,  274,  290,  298,  296,  310,  167,  315,  272,  352,  358,  381,  173,  362,  330,
   176,  261,  275,  291,  299,  297,  311,  183,  316,  273,  353,  359,  382,  8213, 363,  331,
   256,  193,  194,  195,  196,  197,  198,  302,  268,  201,  280,  203,  278,  205,  206,  207,
   208,  325,  332,  211,  212,  213,  214,  360,  216,  370,  218,  219,  220,  221,  222,  223,
   257,  225,  226,  227,  228,  229,  230,  303,  269,  233,  281,  235,  279,  237,  238,  239,
   240,  326,  333,  243,  244,  245,  246,  361,  248,  371,  250,  251,  252,  253,  254,  312,
  };


static const unsigned char reverse_plane[17]={
  0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  };

static const unsigned char reverse_pages[73]={
  0,  24, 24, 24, 24, 24, 24, 24, 87, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24,
  };

static const unsigned short reverse_block[151]={
  0,   16,  32,  48,  64,  80,  96,  112, 128, 144, 160, 176, 191, 207, 223, 239,
  255, 271, 285, 301, 314, 328, 344, 358, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 373, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328,
  328, 328, 328, 328, 328, 328, 328,
  };

static const unsigned char reverse_data[389]={
   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,
   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
   96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
   112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
   128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,
   144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,
   160,  26,   26,   26,   26,   26,   26,   167,  26,   26,   26,   26,   26,   173,  26,   26,
   176,  26,   26,   26,   26,   26,   26,   183,  26,   26,   26,   26,   26,   26,   26,   26,
   193,  194,  195,  196,  197,  198,  26,   26,   201,  26,   203,  26,   205,  206,  207,  208,
   26,   26,   211,  212,  213,  214,  26,   216,  26,   218,  219,  220,  221,  222,  223,  26,
   225,  226,  227,  228,  229,  230,  26,   26,   233,  26,   235,  26,   237,  238,  239,  240,
   26,   26,   243,  244,  245,  246,  26,   248,  26,   250,  251,  252,  253,  254,  26,   192,
   224,  26,   26,   161,  177,  26,   26,   26,   26,   26,   26,   200,  232,  26,   26,   169,
   185,  162,  178,  26,   26,   204,  236,  202,  234,  26,   26,   26,   26,   26,   26,   163,
   179,  26,   26,   26,   26,   165,  181,  164,  180,  26,   26,   199,  231,  26,   26,   26,
   26,   26,   26,   166,  182,  255,  26,   26,   168,  184,  26,   26,   26,   26,   26,   209,
   241,  26,   26,   26,   175,  191,  210,  242,  26,   26,   26,   26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,   26,   26,   26,   170,  186,  26,   26,   26,   26,   171,  187,
   215,  247,  174,  190,  26,   26,   26,   26,   217,  249,  26,   26,   26,   26,   26,   26,
   26,   26,   26,   172,  188,  26,   26,   26,   26,   26,   189,  26,   26,   26,   26,   26,
   26,   26,   26,   26,   26,
  };


FXint FX885910Codec::mb2wc(FXwchar& wc,const FXchar* src,FXint nsrc) const {
  if(nsrc<1) return -1;
  wc=forward_data[(FXuchar)src[0]];
  return 1;
  }


FXint FX885910Codec::wc2mb(FXchar* dst,FXint ndst,FXwchar wc) const {
  if(ndst<1) return -1;
  dst[0]=reverse_data[reverse_block[reverse_pages[reverse_plane[wc>>16]+((wc>>10)&63)]+((wc>>4)&63)]+(wc&15)];
  return 1;
  }

FXint FX885910Codec::mibEnum() const {
  return 13;
  }


const FXchar* FX885910Codec::name() const {
  return "ISO-8859-10";
  }


const FXchar* FX885910Codec::mimeName() const {
  return "ISO-8859-10";
  }


const FXchar* const* FX885910Codec::aliases() const {
  static const FXchar *const list[]={"iso8859-10","ISO-8859-10","ISO_8859-10","latin6","iso-ir-157","csISOLatin6","l6",nullptr};
  return list;
  }

}

