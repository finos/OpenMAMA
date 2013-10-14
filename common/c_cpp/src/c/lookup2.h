/* $Id$ */

/*
 * This file contains header info for the hash alogorith by Bob Jenkins.
 */

#ifndef _LOOKUP2_H_
#define _LOOKUP2_H_

typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

extern ub4 whash (ub1 *k, ub4 length, ub4 initval);

#endif  /* _LOOKUP2_H_ */
