/*
 * =====================================================================================
 *
 *       Filename:  vargs.h
 *
 *    Description:  可变形参表相关的宏定义
 *
 *        Version:  1.0
 *        Created:  2013年11月06日 12时12分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_VARGS_H_
#define INCLUDE_VARGS_H_

//typedef __builtin_va_list va_list;

//#define va_start(ap, last)         (__builtin_va_start(ap, last))
//#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
//#define va_end(ap) 

typedef char *va_list;

#define  _AUPBND                (sizeof (int32_t) - 1)
#define  _ADNBND                (sizeof (int32_t) - 1)
#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))

#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (ap = (va_list) NULL)
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))

#endif 	// INCLUDE_VARGS_H_
