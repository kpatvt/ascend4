/*
 *  Ascend Instance Miscellaneous Queries
 *  by Tom Epperly & Ben Allan
 *  8/16/89
 *  Version: $Revision: 1.13 $
 *  Version control file: $RCSfile: instquery.h,v $
 *  Date last modified: $Date: 1998/02/05 16:36:48 $
 *  Last modified by: $Author: ballan $
 *
 *  This file is part of the Ascend Language Interpreter.
 *
 *  Copyright (C) 1996 Ben Allan
 *  based on instance.c
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
 *  along with the program; if not, write to the Free Software Foundation,
 *  Inc., 675 Mass Ave, Cambridge, MA 02139 USA.  Check the file named
 *  COPYING.
 */

#ifndef __INSTQUERY_H_SEEN__
#define __INSTQUERY_H_SEEN__


/*
 *  When #including instquery.h, make sure these files are #included first:
 *         #include "instance_enum.h"
 *         #include "compiler.h"
 */


/*
 *  The following InterfacePtr functions are to support a global,
 *  persistent client (most likely a GUI) that for some reason
 *  insists (brain dead idiot) that it must decorate the instance
 *  tree with persistent pointers to information packets of its own
 *  devising.
 *  These functions are used by the compiler to handle data an
 *  interface may have left hanging off the instance when an incremental
 *  compile (delete, move, merge) action is in progress. They should never
 *  OTHERWISE be active.
 *
 *  For a more robust, transient protocol see instance_io.h.
 */

extern void (*InterfacePtrDelete)();
/*
 *  This global variable should be provided by the interface.  It is a
 *  pointer to a void function(procedure).  This procedure will be called
 *  when an instance is deleted.  Its purpose is to dispose of the
 *  interface ptr, in other words to clean up/or save the object that the
 *  interfaceptr is looking at. Below is a trivial example.  The instance
 *  pointer i may not be a complete instance.  So you should not call
 *  any instance routines on it.
 *
 *  void InterfacePtrDeleteProc(i,ptr)
 *  struct Instance *i;
 *  struct InterfacePtr *ptr;
 *  {
 *       if (ptr!=NULL) free((VOIDPTR)ptr);
 *  }
 *  main(argc,argv)
 *  {
 *    ....other initialization junk....;
 *    InterfacePtrDelete = InterfacePtrDeleteProc;
 *    ...etc...;
 *  }
 */

extern void (*InterfaceNotify)();
/*
 *  This global variable should be provided by the interface.  It is a
 *  pointer to a void function(procedure).  This procedure will be called
 *  when an instance is moved in memory.  It is provided in case the
 *  interface has a link to the instance.  The old pointer should not be
 *  dereferenced.  Below is a trivial example:
 *
 *  void InterfaceNotifyProc(ptr,old,new)
 *  struct InterfacePtr *ptr;
 *  struct Instance *old,*new;	the old and new address of the instance
 *  {
 *    if(ptr!=NULL){
 *       ptr->inst = new;
 *    }
 *  }
 *
 *  main(argc,argv)
 *  {
 *    ...other initialization junk...;
 *    InterfaceNotify = InterfaceNotifyProc;
 *    ...etc...;
 *  }
 */

extern void (*InterfacePtrATS)();
/*
 *  This global variable should be provided by the interface.  It is a
 *  pointer to a void function(procedure).  The function will be called
 *  when two instances are going to be ARE_THE_SAME'd.  The function's
 *  job is to perform any actions that the interface might want done.
 *  It should not destroy any memory.  This will be done by destroy
 *  Instance.
 *  Below is a trivial example:
 *
 *  void InterfacePtrATSFunc(i1,i2)
 *  struct Instance *i1, *i2;
 *  {
 *    struct InterfacePtr *ptr1,*ptr2;
 *    if (i1 && i2){
 *       ptr1 = GetInterfacePtr(i1);
 *       ptr2 = GetInterfacePtr(i2);
 *       if (ptr2!=NULL) {
 *          if (ptr1!=NULL) {
 *          }
 *       }
 *    }
 *  main(argc,argv)
 *  {
 *    ....other initialization junk...;
 *    InterfacePtrATS = InterfacePtrATSFunc;
 *    ...etc...;
 *  }
 */

/*
 *  General instance interogation routines
 */

#ifndef NDEBUG
#define InstanceKind(i) InstanceKindF(i)
#else
#define InstanceKind(i) ((i==NULL) ? ERROR_INST : ((struct Instance *)(i))->t)
#endif
extern enum inst_t InstanceKindF(CONST struct Instance *);
/*
 *  enum inst_t InstanceKind(i);
 *  const struct Instance *i;
 *
 *  Return the enumerated type that indicates the type of i.
 */

#define IsConstantInstance(i)		((i)->t & ICONS)
#define IsFundamentalInstance(i)	((i)->t & IFUND)
#define IsAtomicInstance(i)		((i)->t & IATOM)
#define IsCompoundInstance(i)		((i)->t & ICOMP)
#define IsArrayInstance(i)		((i)->t & IARR)
#define IsChildlessInstance(i)		((i)->t & ICHILDLESS)
/*
 *  int Is*Instance(i);
 *  const struct Instance *i;
 *  TRUE (some value >0) if i is of the instance class indicated.
 */

extern unsigned long InstanceDepth(CONST struct Instance *);
/*
 *  unsigned long InstanceDepth(i)
 *  const struct Instance *i;
 *  Return the longest distance between i and root.  The depth of NULL is 0.
 *  The root instance is at depth 1.
 */

extern unsigned long InstanceShortDepth(CONST struct Instance *);
/*
 *  unsigned long InstanceShortDepth(i)
 *  const struct Instance *i;
 *  Return the shortest distance between i and root.  The depth of NULL
 *  is 0, and the depth of root is 1.
 */


extern void SetNextCliqueMember(struct Instance *, struct Instance *);
/*
 *  SetNextCliqueMember(i,ptr)
 *  sets i->alike_ptr to ptr for types that have alike_ptrs.
 *  exits on types that don't or bad input.
 *  Instantiator use only! Clients should never ever touch this.
 *  This is not an intelligent function.
 */

extern struct Instance *NextCliqueMember(CONST struct Instance *);
/*
 *  struct Instance *NextCliqueMember(i);
 *  const struct Instance *i;
 *
 *  This is defined to give clients access to the cliques(ARE_ALIKE) links
 *  between instances.  Each instance has a clique pointer which points to
 *  the next member of the clique list.  These pointers form a circularly
 *  linked list.  The following loop can be used to access each member of
 *  the clique.
 *
 *     struct Instance *i,*start;
 *     (* i points the instance whose clique is to be examined *)
 *     start = i;
 *     do {
 *        (* insert your code here *)
 *        i = NextCliqueMember(i);
 *     } while(i!=start);
 *
 *  This loop will execute the inserted code for each member of the clique.
 *  This function can be call on *any* type of instance.
 */

extern VOIDPTR GetInterfacePtr(CONST struct Instance *);
/*
 *  VOIDPTR GetInterfacePtr(i)
 *  const struct Instance *i;
 *  Return the interface pointer.  The compiler initializes this to NULL
 *  when it creates an instance of the following kinds, and hence should
 *  only be called on them.
 *
 *  SIM_INST, MODEL_INST, SET_ATOM_INST, REL_INST, LREL_INST,WHEN_INST
 *  REAL_ATOM_INST, INTEGER_ATOM_INST, SYMBOL_ATOM_INST, BOOLEAN_ATOM_INST
 *  ARRAY_ENUM_INST, ARRAY_INT_INST,
 *  REAL_CONSTANT_INST, INTEGER_CONSTANT_INST, SYMBOL_CONSTANT_INST,
 *  BOOLEAN_CONSTANT_INST.
 *
 *  Those not supported are: (atom children)
 *  REAL_INST, INTEGER_INST, SYMBOL_INST, BOOLEAN_INST, SET_INST,
 *
 *  It is up to the interface to give this pointer meaning.
 *  In a multiple interface environment this pointer needs to be
 *  managed carefully. Any really sane environment will not use
 *  this pointer except in a transient fashion with Push/Pop in
 *  instance_io.h.
 */

extern void SetInterfacePtr(struct Instance *,VOIDPTR);
/*
 *  void SetInterfacePtr(i,c)
 *  struct Instance *i;
 *  VOIDPTR c;
 *  Set the interface pointer.  The interface must set and maintain this
 *  pointer. See the note for GetInterfacePtr about applicability of this
 *  function.
 */

 extern unsigned int GetAnonFlags(CONST struct Instance *);
/*
 *  unsigned int GetAnonFlags(i)
 *  const struct Instance *i;
 *  Returns the flags associated with an instance. This is a utility
 *  service provided for clients who need access to flags associated with
 *  an instance. These flags may also be set with the below function.
 *  Works for all instance kinds except subatomic ones.
 */

extern void SetAnonFlags(struct Instance *,unsigned int);
/*
 *  void SetAnonFlags(i,flags)
 *  const struct Instance *i;
 *  unsigned int flags;
 *  Sets the flags associated with an instance. This is a utility
 *  service provided for clients who need access to flags associated with
 *  Works for all instance kinds except subatomic ones.
 */

extern struct BitList *InstanceBitList(CONST struct Instance *);
/*
 *  struct BitList *InstanceBitList(i)
 *  const struct Instance *i;
 *  Return the bit list which indicates which statements have and have not
 *  been executed.  NULL indicates that there aren't any unexecuted statements.
 *  Only MODEL_INST have bitlists.
 */

/*
 *  Instance querying routines
 *
 *  These are general instance querying routines.
 */

extern symchar *InstanceType(CONST struct Instance *);
/*
 *  symchar *InstanceType(i)
 *  CONST struct Instance *i;
 *  Return a string indicating the type of instance i.  This works for
 *  all types of instances.  It returns the blank string for arrays and
 *  relations though.
 */

extern struct TypeDescription *InstanceTypeDesc(CONST struct Instance *);
/*
 *  struct TypeDescription *InstanceTypeDesc(i)
 *  const struct Instance *i;
 *  Return the instance's type description.  This returns NULL for fundamental
 *  instances.
 */

extern unsigned long InstanceIndirected(CONST struct Instance *);
/*
 * Returns the indirected field of array instances and LONG_MAX
 * for other kinds of instances.
 * Does not tolerate NULL.
 */

extern unsigned long InstanceSize(CONST struct Instance *);
/*
 *  unsigned long InstanceSize(i);
 *  CONST struct Instance *i;
 *  Returns the number of bytes chargeable to the given instance i.
 *  This is not recursive.
 *  Fundamental types (which occur only as children of atoms/relations)
 *  are charged to their parent atom/relation and hence have a 'cost'
 *  of 0 according to this function.
 *  Symbol table, set_t, and dimen items associated with instances
 *  are not charged.
 */

#define InstanceUniversal(i) \
  (GetUniversalFlag(InstanceTypeDesc(i)) != 0)
/*
 * macro InstanceUniversal(i); returns TRUE if i is a UNIVERSAL instance.
 */

extern int IntegerSetInstance(CONST struct Instance *);
/*
 *  int IntegerSetInstance(i)
 *  const struct Instance *i;
 *  This should only be called on set instances.  It will return true if
 *  the set is of integers and false otherwise.
 */

extern symchar *GetSimulationName(struct Instance *);
/*
 *  const char *GetSimulationName(i);
 *  struct Instance *i;
 *  Returns the name of the simulation instance. i must be a SIM_INST kind.
 */

extern struct gl_list_t *FindSimulationAncestors(struct Instance *);
/*
 * sl = FindSimulationAncestors(i);
 * Returns a list of all the simulation ancestors of i.
 * Caller should destroy the list (but obviously not its content).
 */

extern struct Instance *GetSimulationRoot(struct Instance *);
/*
 *  struct Instance *GetSimulationRoot(i);
 *  struct Instance *i;
 *  Returns the root instance of the simulation. This is where most if not
 *  all useful queries of a simulation should be based.
 *  i must be a sim instance.
 */

#endif
/* __INSTQUERY_H_SEEN__ */
