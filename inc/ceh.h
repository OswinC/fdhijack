/*************************************************
 * author: 王胜祥 *
 * email: <mantx@21cn.com> *
 * date: 2005-03-07 *
 * version: *
 * filename: ceh.h *
 *************************************************/


/********************************************************************

  This file is part of CEH(Exception Handling in C Language).

  CEH is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  CEH is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  　　注意：这个异常处理框架不支持线程安全，不能在多线程的程序环境下使用。
  如果您想在多线程的程序中使用它，您可以自己试着来继续完善这个
  框架模型。
 *********************************************************************/
#ifndef _CEH_H_
#define _CEH_H_

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

#define	ERRMSG_LENGTH	128

////////////////////////////////////////////////////
/* 与异常有关的结构体定义 */
typedef struct _CEH_EXCEPTION {
	int err_type; /* 异常类型 */
	int err_code; /* 错误代码 */
	char err_msg[ERRMSG_LENGTH]; /* 错误信息 */
} CEH_EXCEPTION; /* 异常对象 */

typedef struct _CEH_ELEMENT {
	jmp_buf exec_status;
	CEH_EXCEPTION ex_info;

	struct _CEH_ELEMENT* next;
} CEH_ELEMENT; /* 存储异常对象的链表元素 */
////////////////////////////////////////////////////


////////////////////////////////////////////////////
/* 内部接口定义，操纵维护链表数据结构 */
extern void CEH_push(CEH_ELEMENT* ceh_element);
extern CEH_ELEMENT* CEH_pop();
extern CEH_ELEMENT* CEH_top();
extern int CEH_isEmpty();
////////////////////////////////////////////////////


/* 以下是外部接口的定义 */
////////////////////////////////////////////////////
/* 抛出异常 */
extern void thrower(CEH_EXCEPTION* e);
int pceherr(CEH_EXCEPTION *ceh_ex_info);

/* 抛出异常 (throw)
   a表示err_type 
   b表示err_code 
   c表示err_msg 
   */
#define throw(a, b, c) \
{ \
	CEH_EXCEPTION ex; \
	memset(&ex, 0, sizeof(ex)); \
	ex.err_type = a; \
	ex.err_code = b; \
	strncpy(ex.err_msg, c, sizeof(c)); \
	thrower(&ex); \
}

/* 重新抛出原来的异常 (rethrow)*/
#define rethrow thrower(ceh_ex_info)
////////////////////////////////////////////////////


////////////////////////////////////////////////////
/* 定义try block（受到监控的代码）*/
#define try \
{ \
	int ___ceh_b_catch_found, ___ceh_b_occur_exception; \
	CEH_ELEMENT ___ceh_element; \
	CEH_EXCEPTION* ceh_ex_info; \
	memset(&___ceh_element, 0, sizeof(___ceh_element)); \
	CEH_push(&___ceh_element); \
	ceh_ex_info = &___ceh_element.ex_info; \
	___ceh_b_catch_found = 0; \
	if (!(___ceh_b_occur_exception=setjmp(___ceh_element.exec_status))) \
	{


/* 定义catch block（异常错误的处理模块）
	catch表示捕获所有类型的异常
*/
#define catch \
	} \
	else \
	{ \
		CEH_pop(); \
		___ceh_b_catch_found = 1;


/* end_try表示前面定义的try block和catch block结束 */
#define end_try \
	} \
	{ \
		/* 没有执行到任何的catch块中 */ \
		if(!___ceh_b_catch_found) \
		{ \
			CEH_pop(); \
			/* 出现了异常，但没有捕获到任何异常 */ \
			if(___ceh_b_occur_exception) thrower(ceh_ex_info); \
		} \
	} \
}


/* 定义catch block（异常错误的处理模块）
   catch_part表示捕获一定范围内的异常
*/
#define catch_part(i, j) \
	} \
	else if(ceh_ex_info->err_type >= i && ceh_ex_info->err_type <= j) \
	{ \
		CEH_pop(); \
		___ceh_b_catch_found = 1;


/* 定义catch block（异常错误的处理模块）
	catch_one表示只捕获一种类型的异常
*/
#define catch_one(i) \
	} \
	else if(ceh_ex_info->err_type == i) \
	{ \
		CEH_pop(); \
		___ceh_b_catch_found = 1;
////////////////////////////////////////////////////

#endif /* _CEH_H_ */
