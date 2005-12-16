/*
 *  SLV: Ascend Numeric Solver
 *  by Karl Michael Westerberg
 *  Created: 2/6/90
 *  Version: $Revision: 1.31 $
 *  Version control file: $RCSfile: var.c,v $
 *  Date last modified: $Date: 1998/02/19 13:31:36 $
 *  Last modified by: $Author: mthomas $
 *
 *  This file is part of the SLV solver.
 *
 *  Copyright (C) 1990 Karl Michael Westerberg
 *  Copyright (C) 1993 Joseph Zaher
 *  Copyright (C) 1994 Joseph Zaher, Benjamin Andrew Allan
 *
 *  The SLV solver is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The SLV solver is distributed in hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the program; if not, write to the Free Software Foundation,
 *  Inc., 675 Mass Ave, Cambridge, MA 02139 USA.  Check the file named
 *  COPYING.  COPYING is found in ../compiler.
 *
 */

#include "utilities/ascConfig.h"
#include "utilities/ascMalloc.h"
#include "general/dstring.h"
#include "general/list.h"
#include "compiler/compiler.h"
#include "compiler/symtab.h"
#include "compiler/instance_enum.h"
#include "compiler/fractions.h"
#include "compiler/module.h"
#include "compiler/library.h"
#include "compiler/dimen.h"
#include "compiler/child.h"
#include "compiler/type_desc.h"
#include "compiler/atomvalue.h"
#include "compiler/parentchild.h"
#include "compiler/instquery.h"
#include "compiler/instance_io.h"
#include "solver/mtx.h"
#include "solver/slv_types.h"
#include "solver/rel.h"
#include "solver/var.h"
#include "solver/discrete.h"
#include "solver/conditional.h"
#include "solver/logrel.h"
#include "solver/bnd.h"
#include "solver/slv_server.h"
#include "solver/slv_common.h"
#include "solver/linsol.h"
#include "solver/linsolqr.h"
#include "solver/slv_client.h"

/* useful cast */
#define IPTR(i) ((struct Instance *)(i))

/* useful symbol table things to know */
#define FIXED_V g_strings[0]
#define LOWER_V g_strings[1]
#define UPPER_V g_strings[2]
#define RELAXED_V g_strings[3]
#define NOMINAL_V g_strings[4]
#define INTERFACE_V g_strings[5]

/*
 * array of those symbol table entries we need.
 */
static symchar * g_strings[6];

SlvBackendToken var_instanceF(const struct var_variable *var)
{ if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_instance called on bad var\n");
    return NULL;
  }
  return var->ratom;
}

void var_set_instanceF(struct var_variable *var, SlvBackendToken i)
{
  if (var==NULL)  {
    FPRINTF(stderr,"var_set_instance called on NULL var\n");
    return;
  }
  var->ratom = i;
}


char *var_make_name(const slv_system_t sys,const  struct var_variable *var)
{
  return WriteInstanceNameString(IPTR(var->ratom),IPTR(slv_instance(sys)));
}

char *var_make_xname(const struct var_variable *var)
{
   static char name[81];
   char *res;
   sprintf(name,"x%d",var_sindex(var));
   res=(char *)ascmalloc((strlen(name)+1)*sizeof(char));
   sprintf(res,"%s",name);
   return res;
}

void var_write_name(const slv_system_t sys,
  const struct var_variable *var,FILE *fp)
{
  if (var == NULL || fp==NULL) return;
  if (sys!=NULL) {
    WriteInstanceName(fp,var_instance(var),slv_instance(sys));
  } else {
    WriteInstanceName(fp,var_instance(var),NULL);
  }
}

void var_destroy(struct var_variable *var)
{
  if (var==NULL) return;
  var->ratom = NULL;
  ascfree((POINTER)var->incidence);
}


int32 var_mindexF(const struct var_variable *var)
{
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_mindex called on bad var\n");
    return -1;
  }
  return var->mindex;
}

void var_set_mindexF(struct var_variable *var, int32 index)
{
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_mindex called on bad var\n");
    return;
  }
  var->mindex = index;
}

int32 var_sindexF(const struct var_variable *var)
{
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_sindex called on bad var\n");
    return -1;
  }
  return var->sindex;
}

void var_set_sindexF(struct var_variable *var, int32 index)
{
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_sindex called on bad var\n");
    return;
  }
  var->sindex = index;
}

real64 var_value(const struct var_variable *var)
{
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_value called on bad var\n");
    return 0.0;
  }
  return( RealAtomValue(var->ratom) );
}

void var_set_value(struct var_variable *var, real64 value)
{
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_value called on bad var\n");
    return;
  }
  SetRealAtomValue(var->ratom,value,(unsigned)0);
}

real64 var_nominal(struct var_variable *var)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_nominal called on bad var\n");
    return 1.0;
  }
  c = ChildByChar(var->ratom,NOMINAL_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_nominal\n");
    FPRINTF(stderr,"        No 'nominal' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return 1.0;
  }
  return( RealAtomValue(c) );
}

void var_set_nominal(struct var_variable *var, real64 nominal)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_nominal called on bad var\n");
    return;
  }
  c = ChildByChar(IPTR(var->ratom),NOMINAL_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_set_nominal\n");
    FPRINTF(stderr,"        No 'nominal' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return;
  }
  SetRealAtomValue(c,nominal,(unsigned)0);
}


real64 var_lower_bound(struct var_variable *var)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_lower_bound called on bad var\n");
    return 0.0;
  }
  c = ChildByChar(IPTR(var->ratom),LOWER_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_lower_bound\n");
    FPRINTF(stderr,"        No 'lower_bound' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return 0.0;
  }
  return( RealAtomValue(c) );
}

void var_set_lower_bound(struct var_variable *var, real64 lower_bound)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_lower_bound called on bad var\n");
    return;
  }
  c = ChildByChar(IPTR(var->ratom),LOWER_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_set_lower_bound\n");
    FPRINTF(stderr,"        No 'lower_bound' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return;
  }
  SetRealAtomValue(c,lower_bound,(unsigned)0);
}


real64 var_upper_bound(struct var_variable *var)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_upper_bound called on bad var\n");
    return 0.0;
  }
  c = ChildByChar(IPTR(var->ratom),UPPER_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_upper_bound\n");
    FPRINTF(stderr,"        No 'upper_bound' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return 0.0;
  }
  return( RealAtomValue(c) );
}

void var_set_upper_bound(struct var_variable *var, real64 upper_bound)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_upper_bound called on bad var\n");
    return;
  }
  c = ChildByChar(IPTR(var->ratom),UPPER_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_set_upper_bound\n");
    FPRINTF(stderr,"        No 'upper_bound' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return;
  }
  SetRealAtomValue(c,upper_bound,(unsigned)0);
}

uint32 var_flagsF(const struct var_variable *var)
{
  return var->flags;
}

void var_set_flagsF(struct var_variable *var, uint32 flags)
{
  var->flags = flags;
}

uint32 var_fixed(struct var_variable *var)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_fixed called on bad var\n");
    return FALSE;
  }
  c = ChildByChar(IPTR(var->ratom),FIXED_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_fixed\n");
    FPRINTF(stderr,"        No 'fixed' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return FALSE;
  }
  var_set_flagbit(var,VAR_FIXED,GetBooleanAtomValue(c));
  return( GetBooleanAtomValue(c) );
}

void var_set_fixed(struct var_variable *var, uint32 fixed)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_fixed called on bad var\n");
    return;
  }
  c = ChildByChar(IPTR(var->ratom),FIXED_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_set_fixed\n");
    FPRINTF(stderr,"        No 'fixed' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return;
  }
  SetBooleanAtomValue(c,fixed,(unsigned)0);
  var_set_flagbit(var,VAR_FIXED,fixed);
}

uint32 var_relaxed(struct var_variable *var)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_relaxed called on bad var\n");
    return FALSE;
  }
  c = ChildByChar((var->ratom),RELAXED_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_relaxed\n");
    FPRINTF(stderr,"        No 'relaxed' field in variable.\n");
    WriteInstance(stderr,(var->ratom));
    return FALSE;
  }
  var_set_flagbit(var,VAR_RELAXED,GetBooleanAtomValue(c));
  return( GetBooleanAtomValue(c) );
}

void var_set_relaxed(struct var_variable *var, uint32 fixed)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_set_relaxed called on bad var\n");
    return;
  }
  c = ChildByChar(IPTR(var->ratom),RELAXED_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_set_relaxed\n");
    FPRINTF(stderr,"        No 'relaxed' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return;
  }
  SetBooleanAtomValue(c,fixed,(unsigned)0);
  var_set_flagbit(var,VAR_RELAXED,fixed);
}

uint32 var_interface(struct var_variable *var)
{
  struct Instance *c;
  if (var==NULL || var->ratom==NULL) {
    FPRINTF(stderr,"var_interface called on bad var\n");
    return FALSE;
  }
  c = ChildByChar(IPTR(var->ratom),INTERFACE_V);
  if( c == NULL ) {
    FPRINTF(stderr,"ERROR:  (var) var_interface\n");
    FPRINTF(stderr,"        No 'interface' field in variable.\n");
    WriteInstance(stderr,IPTR(var->ratom));
    return 0;
  }
  var_set_flagbit(var,VAR_INTERFACE,GetBooleanAtomValue(c));
  return( GetIntegerAtomValue(c) );
}

extern uint32 var_flagbit(const struct var_variable *var,const uint32 one)
{
  if (var==NULL || var->ratom == NULL) {
    FPRINTF(stderr,"ERROR: var_flagbit called with bad var.\n");
    return 0;
  }
  return (var->flags & one);
}

void var_set_flagbit(struct var_variable *var, uint32 field,uint32 one)
{
  if (var==NULL || var->ratom == NULL) {
    FPRINTF(stderr,"ERROR: var_set_flagbit called with bad var.\n");
    return;
  }
  if (one) {
    var->flags |= field;
  } else {
    var->flags &= ~field;
  }
}

int32 var_apply_filter(const struct var_variable *var,
  const var_filter_t *filter)
{
  if (var==NULL || filter==NULL || var->ratom == NULL) {
    FPRINTF(stderr,"var_apply_filter miscalled with NULL\n");
    return FALSE;
  }
  /* AND to mask off irrelevant bits in flags and match value, then compare */
  return ( (filter->matchbits & var->flags) ==
           (filter->matchbits & filter->matchvalue)
         );
}


int32 var_n_incidencesF(struct var_variable *var)
{
  if (var!=NULL) return var->n_incidences;
  FPRINTF(stderr,"var_n_incidences miscalled with NULL\n");
  return 0;
}
void var_set_incidencesF(struct var_variable *var,int32 n,
                         struct rel_relation **i)
{
  if(var!=NULL && n >=0) {
    if (n && i==NULL) {
      FPRINTF(stderr,"var_set_incidence miscalled with NULL ilist\n");
    }
    var->n_incidences = n;
    var->incidence = i;
    return;
  }
  FPRINTF(stderr,"var_set_incidence miscalled with NULL or n < 0\n");
}
const struct rel_relation **var_incidence_list( struct var_variable *var)
{
  if (var==NULL) return NULL;
  return( (const struct rel_relation **)var->incidence );
}

struct rel_relation **var_incidence_list_to_modify( struct var_variable *var)
{
  if (var==NULL) return NULL;
  return( (struct rel_relation **)var->incidence );
}



/* 
 * global for use with the push function. Sets the ip to the
 * value in g_var_tag;
 * should be using vp instead of a global counter.
 */
static void *g_var_tag = NULL;
static void * SetVarTags(struct Instance *i,VOIDPTR vp)
{
  (void)vp;
  if (i!=NULL && InstanceKind(i)==REAL_ATOM_INST) {
    return g_var_tag;
  } else {
    return NULL;
  }
}

struct var_variable **var_BackendTokens_to_vars(slv_system_t sys,
  SlvBackendToken *atoms, int32 len)
{
  int32 i,vartot,vlen,count=0;
  uint32 apos,ulen;
  struct var_variable **result;
  struct var_variable **vlist;
  struct gl_list_t *oldips;
  if (sys==NULL || atoms == NULL || len < 1) {
    return NULL;
  }
  ulen = (uint32)len;
  result = (struct var_variable **)malloc(len*sizeof(struct var_variable *));
  if (result == NULL) return result;
  /* init results to null */
  for (i=0; i<len; i++) result[i] = NULL;
  /* fill ips w/len in all the vars in tree. */
  g_var_tag = (void *)len;
  vartot = slv_get_num_master_vars(sys) +
           slv_get_num_master_pars(sys) +
           slv_get_num_master_unattached(sys);
  oldips = PushInterfacePtrs(slv_instance(sys),SetVarTags,vartot,0,NULL);
  /* fill ips of wanted atoms with i */
  for (i=0; i<len; i++) {
    if (GetInterfacePtr(atoms[i])==g_var_tag &&
	InstanceKind(atoms[i]) == REAL_ATOM_INST) {
	/* guard a little */
      SetInterfacePtr((struct Instance *)atoms[i],(void *)i);
    } else {
      /* the odds of g_var_tag being a legal pointer are vanishingly
	small, so if we find an ATOM without g_var_tag we assume it
	is outside the tree and shouldn't have been in the list. */
      FPRINTF(stderr,"var_BackendTokens_to_vars called with bad token.\n");
    }
  }
  /* run through the master lists and put the vars with their atoms */
  vlist = slv_get_master_var_list(sys);
  vlen = slv_get_num_master_vars(sys);
  for (i = 0; i <vlen; i++) {
    apos = (uint32)GetInterfacePtr(var_instance(vlist[i]));
    if ( apos < ulen ) {
      result[apos] = vlist[i];
      count++;
    }
  }
  vlist = slv_get_master_par_list(sys);
  vlen = slv_get_num_master_pars(sys);
  for (i = 0; i <vlen; i++) {
    apos = (uint32)GetInterfacePtr(var_instance(vlist[i]));
    if ( apos < ulen ) {
      result[apos] = vlist[i];
      count++;
    }
  }
  vlist = slv_get_master_unattached_list(sys);
  vlen = slv_get_num_master_unattached(sys);
  for (i = 0; i <vlen; i++) {
    apos = (uint32)GetInterfacePtr(var_instance(vlist[i]));
    if ( apos < ulen ) {
      result[apos] = vlist[i];
      count++;
    }
  }
  if (count < len) {
    FPRINTF(stderr,
           "var_BackendTokens_to_vars found less than expected vars\n");
    FPRINTF(stderr,"len = %d, vars found = %d\n",len,count);
  } else {
    FPRINTF(stderr,
           "var_BackendTokens_to_vars found more than expected vars\n");
    FPRINTF(stderr,"len = %d, vars found = %d\n",len,count);
  }
  PopInterfacePtrs(oldips,NULL,NULL);
  return result;
}

static struct TypeDescription *g_solver_var_type;
static struct TypeDescription *g_solver_int_type;
static struct TypeDescription *g_solver_binary_type;
static struct TypeDescription *g_solver_semi_type;

boolean set_solver_types(void) {
  boolean nerr = 0;
  if( (g_solver_var_type = FindType(AddSymbol(SOLVER_VAR_STR))) == NULL ) {
    FPRINTF(stderr,"ERROR:  (var.c) set_solver_types\n");
    FPRINTF(stderr,"        Type solver_var not defined.\n");
    FPRINTF(stderr,"        Solvers will not work.\n");
    nerr++;
  }
  if( (g_solver_int_type = FindType(AddSymbol(SOLVER_INT_STR))) == NULL ) {
    FPRINTF(stderr,"ERROR:  (var.c) set_solver_types\n");
    FPRINTF(stderr,"        Type solver_int not defined.\n");
    FPRINTF(stderr,"        MPS will not work.\n");
    nerr++;
  }
  g_solver_binary_type = FindType(AddSymbol(SOLVER_BINARY_STR));
  if( g_solver_binary_type == NULL) {
    FPRINTF(stderr,"ERROR:  (var.c) set_solver_types\n");
    FPRINTF(stderr,"        Type solver_binary not defined.\n");
    FPRINTF(stderr,"        MPS will not work.\n");
    nerr++;
  }
  if( (g_solver_semi_type = FindType(AddSymbol(SOLVER_SEMI_STR))) == NULL ) {
    FPRINTF(stderr,"ERROR:  (var.c) set_solver_types\n");
    FPRINTF(stderr,"        Type solver_semi not defined.\n");
    FPRINTF(stderr,"        MPS will not work.\n");
    nerr++;
  }

  LOWER_V = AddSymbolL("lower_bound",11);
  UPPER_V = AddSymbolL("upper_bound",11);
  RELAXED_V = AddSymbolL("relaxed",7);
  NOMINAL_V = AddSymbolL("nominal",7);
  FIXED_V = AddSymbolL("fixed",5);
  INTERFACE_V = AddSymbolL("interface",9);
 
  return nerr;
}

boolean solver_var( SlvBackendToken inst)
{
  struct TypeDescription *type;

  if (!g_solver_var_type) return FALSE;
  type = InstanceTypeDesc(IPTR(inst));
  return( type == MoreRefined(type,g_solver_var_type) );
}

boolean solver_int( SlvBackendToken inst)
{
  struct TypeDescription *type;

  if (!g_solver_int_type) return FALSE;
  type = InstanceTypeDesc(IPTR(inst));
  return( type == MoreRefined(type,g_solver_int_type) );
}

boolean solver_binary( SlvBackendToken inst)
{
  struct TypeDescription *type;

  if (!g_solver_binary_type) return FALSE;
  type = InstanceTypeDesc(IPTR(inst));
  return( type == MoreRefined(type,g_solver_binary_type) );
}

boolean solver_semi( SlvBackendToken inst)
{
  struct TypeDescription *type;

  if (!g_solver_semi_type) return FALSE;
  type = InstanceTypeDesc(IPTR(inst));
  return( type == MoreRefined(type,g_solver_semi_type) );
}

