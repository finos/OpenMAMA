/* $Id: lookup2.h,v 1.1.34.2 2011/08/10 13:06:53 nicholasmarriott Exp $ */

/*
 * This file contains header info for the hash alogorith by Bob Jenkins.
 */

#ifndef _LOOKUP2_H_
#define _LOOKUP2_H_

typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

ub4 hash (ub1 *k, ub4 length, ub4 initval);

#endif  /* _LOOKUP2_H_ */
