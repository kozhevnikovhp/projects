// ---------------------------------------------------

#include "common_def.hpp"
#include "poly2rt.hpp"

extern bool SI_isPointinArea(ID_AREAS *area, float xt, float yt);
extern bool SI_isPointOnLine(double U0x, double U0y, double U1x, double U1y, double Px, double Py);
extern double SI_Angle(float xs, float ys, float xe, float ye, int iFlag);
extern void SI_heapsort(unsigned int N, HS_SORT *sHSsort, int (*compare)(int i1, int i2, int iSortType), int iSortType );
extern long SI_iabs(long x);
extern int SI_indx(char *s, char *t);
extern int SI_IsEqual(float x, float y);
extern bool SI_IsEqualD(double x, double y);
extern void SI_remove_crlf(char *ln);
extern int SI_blank(char *ln);
extern void SI_remove_comments(char *ln);
extern void SI_remove_leading_spaces(char *ln);
extern void SI_remove_trailing_spaces(char *ln);
extern void SI_remove_quotes(char *ln);
extern void SI_replace_char_with_space(char *ln, char ch);
extern char *SI_strsave(char *stg);
extern void *SI_sp_alloc(unsigned int size);
extern void *SI_sp_malloc(size_t size);
extern void g_debug(char *fn, char *str);

// ---------------------------------------------------

