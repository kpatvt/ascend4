/*
 *  Function Module
 *  by Tom Epperly
 *  Created: 8/11/1990
 *  Version: $Revision: 1.18 $
 *  Version control file: $RCSfile: func.c,v $
 *  Date last modified: $Date: 2001/01/31 22:23:53 $
 *  Last modified by: $Author: ballan $
 *
 *  This file is part of the Ascend Language Interpreter.
 *
 *  Copyright (C) 1990, 1993, 1994 Thomas Guthrie Epperly
 *
 *  The Ascend Language Interpreter is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The Ascend Language Interpreter is distributed in hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "func.h"

#include<math.h>
#include "safe.h"

#ifndef M_PI
#define M_PI F_PI
#endif
#ifndef M_LOG10E
#define M_LOG10E F_LOG10_COEF
#endif

#ifndef NULL
#define NULL 0L
#endif

//#define FUNC_DEBUG
#ifdef FUNC_DEBUG
# define MSG CONSOLE_DEBUG
#else
# define MSG(ARGS...) ((void)0)
#endif

double g_lnm_epsilon = 1.0e-8;


#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
double cbrt(double d){
  return pow(d,(double)0.3333333333333333333333);
}
#endif

int ascnintF(double d){
  return ((d)>=0.0 ? (int)floor((d) + 0.5) : -(int)floor(0.5 - (d)));
}


double dln(double d){
  return 1.0/d;
}

double dln2(double d){
  return -1.0/(d*d);
}

double lnm(double d){
  return (d>g_lnm_epsilon?log(d):d/g_lnm_epsilon + (log(g_lnm_epsilon) -1));

}

double dlnm(double d){
  return ( d>g_lnm_epsilon ? (double)1.0/d : 1/g_lnm_epsilon);
}

double dlnm2(double d){
  return (d>g_lnm_epsilon ? (double)-1.0/(d*d) : (double)0.0);
}

double dlog10(double d){
  return M_LOG10E/d;
}

double dlog102(double d){
  return -M_LOG10E/(d*d);
}

double dcos(double d){
  return -sin(d);
}

double dcos2(double d){
  return -cos(d);
}

double dtan(double d){
  double c;
  c=cos(d);
  return 1.0/(c*c);
}

double dtan2(double d){
  double c;
  c=cos(d);
  return ldexp(tan(d)/(c*c),1);
}

double sqr(double d){
  return d*d;
}

double dsqr(double d){
  return ldexp(d,1);
}

double dsqr2(double d){
  (void)d;  /*  stop gcc whine about unused parameter */
  return 2.0;
}

double hold(double d){
  return d;
}

double dsqrt(double d){
  return 1.0/(ldexp(sqrt(d),1));
}

double dsqrt2(double d){
  return -1.0/ldexp(sqrt(d)*d,2);
}

double dfabs(double d){
  return ((d > 0.0) ? 1.0 : ((d<0.0 ) ? -1 : 0));
}

double dfabs2(double d){
  (void)d;  /*  stop gcc whine about unused parameter */
  return 0.0;
}

double dhold(double d){
  (void)d;  /*  stop gcc whine about unused parameter */
  return 0;
}

 /* The next 4 are new */
double asc_ipow(double d, int i) {
  unsigned negative = 0;
  negative = (i<0);
  if (negative) i = (-i);
  if (d==0 && i!=0) return 0.0;
  switch (i) {
  case 0: return 1.0; /* a^0 = 1, for a==0 pow is undefined. */
  case 1: break;
  case 2: d *= d; break;
  case 3: d = d*d*d; break;
  case 4: d = d*d*d*d;break;
  case 5: d = d*d*d*d*d; break;
  case 6: d = d*d*d*d*d*d; break;
  case 7: d = d*d*d*d*d*d*d; break;
  case 8: d = d*d*d*d*d*d*d*d; break;
  case 9: d = d*d*d*d*d*d*d*d*d; break;
  default:
    {
      double res;
      res = d;
      for (--i; i > 0; i--) res *= d;
      d = res;
    }
    break;
  }
  return (!negative ? d : 1.0/d);
}

/*
 * Note that the following derivative functions do not
 * set calc_ok to FALSE in the event of errors.  This
 * checking is done in the solver so we are basically
 * double checking now -> this should be fixed
 */

double asc_d1ipow(double d, int i) {
    if (d == 0 && i <= 1) {
	FPRINTF(stderr,"ERROR:\t(calc) calc_ipow_D1\n");
	FPRINTF(stderr,
                "\t1st derivative, %g raised to %d <= 1 power undefined.\n",
                d,i);
	FPRINTF(stderr,"\tReturning %g.\n",0.0);
	return(0.0);
    }
    return( i * asc_ipow(d,i-1));
}

double asc_d2ipow(double d, int i) {
    if (d == 0 && i <= 2) {
	FPRINTF(stderr,"ERROR:\t(calc) calc_ipow_D2\n");
	FPRINTF(stderr,
                "\t2nd derivative, %g raised to %d <= 2 power undefined.\n",
                d,i);
	FPRINTF(stderr,"\tReturning %g.\n",0.0);
	return(0.0);
    }
    return( i * (i - 1) * asc_ipow(d,i-2));
}


double cube(double d){
  return d*d*d;
}
double dcube(double d){
  return 3.0*d*d;
}
double dcube2(double d){
  return 6.0*d;
}

double dcbrt(double d){
  double c;
  c=cbrt(d);
  return (double)0.3333333333333333/(c*c);
}

double dcbrt2(double d){
  double c;
  c=cbrt(d);
  return (double)-0.2222222222222222/pow(c,5.0);
}

double dasin(double d){
  return 1.0/sqrt(1.0-d*d);
}

double dasin2(double d){
  double c;
  c=1.0-d*d;
  return d/(c*sqrt(c));
}

double dacos(double d)
{
  return -1.0/sqrt(1-d*d);
}

double dacos2(double d){
  double c;
  c=1.0-d*d;
  return -d/(c*sqrt(c));
}

double datan(double d){
  return 1.0/(1.0+d*d);
}

double datan2(double d){
  return -2*d/sqr(1+d*d);
}

#ifdef HAVE_ERF
double derf(double d){
  return ldexp(exp(-(d*d))/sqrt(M_PI),1);
}

double derf2(double d)
{
  return -ldexp(d*exp(-(d*d))/sqrt(M_PI),2);
}
#endif /* HAVE_ERF */

double dtanh(double d){
  double c;
  c = cosh(d);
  c = 1/(c*c);
  return c;
}

double dtanh2(double d){
  double c;
  c = cosh(d);
  return -ldexp(tanh(d),1)/(c*c);
}

double arcsinh(double d){
  return log(d+sqrt(d*d+1.0));
}

double darcsinh(double d){
  return 1.0/sqrt(d*d+1.0);
}

double darcsinh2(double d){
  double c;
  c=d*d+1.0;
  return -d/sqrt(c*c*c);
}

double arccosh(double d){
  return log(d+sqrt(d*d-1.0));
}

double darccosh(double d){
  return 1.0/sqrt(d*d-1.0);
}

double darccosh2(double d){
  double c;
  c=d*d-1.0;
  return -d/sqrt(c*c*c);
}

double arctanh(double d){
  return  ldexp( log((d+1.0)/(1.0-d)) ,-1);
/* an alternative, more expensive but perhaps less exception prone
*  coding of arctanh is:
* return log(sqrt((d+1.0)/(1.0-d)));
* which for d near -1 will be less likely to underflow and send 0
* to the log function. Until otherwise noted we are running the
* cheap version.
*/
}

double darctanh(double d){
  return  1.0/(1-d*d);
}

double darctanh2(double d){
  double c;
  c=1.0-d*d;
  return ldexp( d/(c*c) ,1);
}


struct Func g_exp_f = {
  "exp",
  "exp",
  "Exp",
  "exp",
  "exp",
  F_EXP,
  exp,
  exp,
  exp,
  safe_exp_D0,
  safe_exp_D1,
  safe_exp_D2,
};

struct Func g_ln_f = {
  "ln",
  "log",
  "Ln",
  "dln",
  "dln2",
  F_LN,
  log,
  dln,
  dln2,
  safe_ln_D0,
  safe_ln_D1,
  safe_ln_D2,
};

struct Func g_lnm_f = {
  "lnm",
  "lnm",
  "Lnm",  /** FIXME Yacas does not have an equivalent for Lnm??? */
  "dlnm",
  "dlnm2",
  F_LNM,
  lnm,
  dlnm,
  dlnm2,
  safe_lnm_D0,
  safe_lnm_D1,
  safe_lnm_D2,
};

struct Func g_log10_f = {
  "log10",
  "log10",
  "Log10", /** FIXME Yacas does not have an equivalent for Log10??? */
  "dlog10",
  "dlog102",
  F_LOG10,
  log10,
  dlog10,
  dlog102,
  safe_log10_D0,
  safe_log10_D1,
  safe_log10_D2,
};

struct Func g_sin_f = {
  "sin",
  "sin",
  "Sin",
  "cos",
  "dcos",
  F_SIN,
  sin,
  cos,
  dcos,
  safe_sin_D0,
  safe_sin_D1,
  safe_sin_D2,
};

struct Func g_cos_f = {
  "cos",
  "cos",
  "Cos",
  "dcos",
  "dcos2",
  F_COS,
  cos,
  dcos,
  dcos2,
  safe_cos_D0,
  safe_cos_D1,
  safe_cos_D2,
};

struct Func g_tan_f = {
  "tan",
  "tan",
  "Tan",
  "dtan",
  "dtan2",
  F_TAN,
  tan,
  dtan,
  dtan2,
  safe_tan_D0,
  safe_tan_D1,
  safe_tan_D2,
};

struct Func g_sqr_f = {
  "sqr",
  "sqr",
  "Sqr", /** FIXME Yacas does not have an equivalent for Sqr??? */
  "dsqr",
  "dsqr2",
  F_SQR,
  sqr,
  dsqr,
  dsqr2,
  safe_sqr_D0,
  safe_sqr_D1,
  safe_sqr_D2,
};

struct Func g_sqrt_f = {
  "sqrt",
  "sqrt",
  "Sqrt",
  "dsqrt",
  "dsqrt2",
  F_SQRT,
  sqrt,
  dsqrt,
  dsqrt2,
  safe_sqrt_D0,
  safe_sqrt_D1,
  safe_sqrt_D2,
};

struct Func g_abs_f = {
  "abs",
  "fabs",
  "Abs",
  "dfabs",
  "dfabs2",
  F_ABS,
  fabs,
  dfabs,
  dfabs2,
  safe_fabs_D0,
  safe_fabs_D1,
  safe_fabs_D2,
};

struct Func g_hold_f = {
  "hold",
  "hold",
  "Hold", /** FIXME Yacas does not have an equivalent for Hold??? */
  "dhold",
  "dhold2",
  F_HOLD,
  hold,
  dhold,
  dhold2,
  safe_hold_D0,
  safe_hold_D1,
  safe_hold_D2,
};

struct Func g_arcsin_f = {
  "arcsin",
  "asin",
  "ArcSin",
  "dasin",
  "dasin2",
  F_ARCSIN,
  asin,
  dasin,
  dasin2,
  safe_arcsin_D0,
  safe_arcsin_D1,
  safe_arcsin_D2,
};

struct Func g_arccos_f = {
  "arccos",
  "acos",
  "ArcCos",
  "dacos",
  "dacos2",
  F_ARCCOS,
  acos,
  dacos,
  dacos2,
  safe_arccos_D0,
  safe_arccos_D1,
  safe_arccos_D2,
};

struct Func g_arctan_f = {
  "arctan",
  "atan",
  "ArcTan",
  "datan",
  "datan2",
  F_ARCTAN,
  atan,
  datan,
  datan2,
  safe_arctan_D0,
  safe_arctan_D1,
  safe_arctan_D2,
};

#ifdef HAVE_ERF
struct Func g_erf_f = {
  "erf",
  "erf",
  "Erf", /** FIXME Yacas does not have an equivalent for Erf??? */
  "derf",
  "derf2",
  F_ERF,
  erf,
  derf,
  derf2,
  safe_erf_D0,
  safe_erf_D1,
  safe_erf_D2,
};
#endif /* HAVE_ERF */

struct Func g_sinh_f = {
  "sinh",
  "sinh",
  "Sinh", /** FIXME Yacas does not have an equivalent for Sinh??? */
  "cosh",
  "sinh",
  F_SINH,
  sinh,
  cosh,
  sinh,
  safe_sinh_D0,
  safe_sinh_D1,
  safe_sinh_D2,
};

struct Func g_cosh_f = {
  "cosh",
  "cosh",
  "Cosh", /** FIXME Yacas does not have an equivalent for Cosh??? */
  "sinh",
  "cosh",
  F_COSH,
  cosh,
  sinh,
  cosh,
  safe_cosh_D0,
  safe_cosh_D1,
  safe_cosh_D2,
};

struct Func g_tanh_f = {
  "tanh",
  "tanh",
  "Tanh", /** FIXME Yacas does not have an equivalent for Tanh??? */
  "dtanh",
  "dtanh2",
  F_TANH,
  tanh,
  dtanh,
  dtanh2,
  safe_tanh_D0,
  safe_tanh_D1,
  safe_tanh_D2,
};

struct Func g_arcsinh_f = {
  "arcsinh",
  "arcsinh",
  "ArcSinh", /** FIXME Yacas does not have an equivalent for ArcSinh??? */
  "darcsinh",
  "darcsinh2",
  F_ARCSINH,
  arcsinh,
  darcsinh,
  darcsinh2,
  safe_arcsinh_D0,
  safe_arcsinh_D1,
  safe_arcsinh_D2,
};

struct Func g_arccosh_f = {
  "arccosh",
  "arccosh",
  "ArcCosh", /** FIXME Yacas does not have an equivalent for ArcCosh??? */
  "darccosh",
  "darccosh2",
  F_ARCCOSH,
  arccosh,
  darccosh,
  darccosh2,
  safe_arccosh_D0,
  safe_arccosh_D1,
  safe_arccosh_D2,
};

struct Func g_arctanh_f = {
  "arctanh",
  "arctanh",
  "ArcTanh", /** FIXME Yacas does not have an equivalent for ArcTanh??? */
  "darctanh",
  "darctanh2",
  F_ARCTANH,
  arctanh,
  darctanh,
  darctanh2,
  safe_arctanh_D0,
  safe_arctanh_D1,
  safe_arctanh_D2,
};

struct Func g_cube_f = {
  "cube",
  "cube",
  "Cube", /** FIXME Yacas does not have an equivalent for Cube??? */
  "dcube",
  "dcube2",
  F_CUBE,
  cube,
  dcube,
  dcube2,
  safe_cube,
  safe_cube_D1,
  safe_cube_D2,
};

struct Func g_cbrt_f = {
  "cbrt",
  "cbrt",
  "Cbrt", /** FIXME Yacas does not have an equivalent for Cbrt??? */
  "dcbrt",
  "dcbrt2",
  F_CBRT,
  cbrt,
  dcbrt,
  dcbrt2,
  safe_cbrt_D0,
  safe_cbrt_D1,
  safe_cbrt_D2,
};


struct Func *g_func_list[]={
  &g_log10_f,
  &g_ln_f,
  &g_exp_f,
  &g_sin_f,
  &g_cos_f,
  &g_tan_f,
  &g_sqr_f,
  &g_sqrt_f,
  &g_arcsin_f,
  &g_arccos_f,
  &g_arctan_f,
#ifdef HAVE_ERF
  &g_erf_f,
#endif /* HAVE_ERF */
  &g_lnm_f,
  &g_sinh_f,
  &g_cosh_f,
  &g_tanh_f,
  &g_arcsinh_f,
  &g_arccosh_f,
  &g_arctanh_f,
  &g_cube_f,
  &g_cbrt_f,
  &g_abs_f,
  &g_hold_f,
  NULL	/* must be last */
};

CONST struct Func *LookupFunc(CONST char *name){
  unsigned f=0;
  while(g_func_list[f]!=NULL){
    if(strcmp(g_func_list[f]->name,name)==0)
      return g_func_list[f];
    f++;
  }
  return NULL;
}

CONST struct Func *LookupFuncById(enum Func_enum id){
  unsigned f=0;
  while(g_func_list[f]!=NULL){
    if (g_func_list[f]->id==id)
      return g_func_list[f];
    f++;
  }
  return NULL;
}

CONST char *FuncName(CONST struct Func *f){
  return f->name;
}

CONST char *FuncCName(CONST struct Func *f)
{
  return f->cname;
}

CONST char *FuncYName(CONST struct Func *f){
	return f->yname;
}

CONST char *FuncDeriv1CName(CONST struct Func *f){
  return f->deriv1cname;
}

CONST char *FuncDeriv2CName(CONST struct Func *f){
  return f->deriv2cname;
}

enum Func_enum FuncId(CONST struct Func *f){
  return f->id;
}

CONST dim_type *FuncDimens(CONST struct Func *f){
  if (!f) return Dimensionless();
  switch (FuncId(f)) {
    case F_LOG10:
    case F_LN:
    case F_EXP:
#ifdef HAVE_ERF
    case F_ERF:
#endif /* HAVE_ERF */
    case F_LNM:
    case F_ARCSIN:
    case F_ARCCOS:
    case F_ARCTAN:
    case F_SINH:
    case F_COSH:
    case F_TANH:
    case F_ARCSINH:
    case F_ARCCOSH:
    case F_ARCTANH:
      MSG("Func '%s' is dimensionless");
      return Dimensionless();
    case F_SQR:
    case F_SQRT:
    case F_CUBE:
    case F_CBRT:
    case F_ABS:
    case F_HOLD:
      return WildDimension();
    case F_SIN:
    case F_COS:
    case F_TAN:
      return TrigDimension();
    default: return Dimensionless();
  }
}

double FuncEval(CONST struct Func *f, double d){
  return (*(f->value))(d);
}

double FuncEvalSafe(CONST struct Func *f, double d,enum safe_err *not_safe){
  return (*(f->safevalue))(d,not_safe);
}

double FuncDeriv(CONST struct Func *f, double d){
  return (*(f->deriv))(d);
}

double FuncDerivSafe(CONST struct Func *f, double d,enum safe_err *not_safe){
  return (*(f->safederiv))(d,not_safe);
}

double FuncDeriv2(CONST struct Func *f, double d){
  return (*(f->deriv2))(d);
}

double FuncDeriv2Safe(CONST struct Func *f, double d,enum safe_err *not_safe){
  return (*(f->safederiv2))(d,not_safe);
}

