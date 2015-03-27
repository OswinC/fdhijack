/*************************************************
 * author: 王胜祥 *
 * email: <mantx@21cn.com> *
 * date: 2005-03-07 *
 * version: *
 * filename: ceh.c * 
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

#include <stdio.h>
#include <errno.h>
#include "ceh.h"

////////////////////////////////////////////////////
static CEH_ELEMENT* head = NULL;

/* 把一个异常插入到链表头中 */
void CEH_push(CEH_ELEMENT* ceh_element)
{
	if(head != NULL) ceh_element->next = head;
	head = ceh_element;
}


/* 从链表头中，删除并返回一个异常 */
CEH_ELEMENT* CEH_pop()
{
	CEH_ELEMENT* ret = 0;

	ret = head;
	head = head->next;

	return ret;
}


/* 从链表头中，返回一个异常 */
CEH_ELEMENT* CEH_top()
{
	return head;
}


/* 链表中是否有任何异常 */
int CEH_isEmpty()
{
	return head == NULL;
}
//////////////////////////////////////////////////// 

////////////////////////////////////////////////////
/* 缺省的异常处理模块 */
static void CEH_uncaught_exception_handler(CEH_EXCEPTION *ceh_ex_info) 
{
	fprintf(stderr, "Catch an unhandled exception with err_msg: %s err_type: %d err_code: %d\n",
			ceh_ex_info->err_msg, ceh_ex_info->err_type, ceh_ex_info->err_code);
	exit(EXIT_FAILURE); 
}
////////////////////////////////////////////////////


////////////////////////////////////////////////////
/* 抛出异常 */
void thrower(CEH_EXCEPTION* e) 
{
	CEH_ELEMENT *se;

	if (CEH_isEmpty()) CEH_uncaught_exception_handler(e);

	se = CEH_top();
	se->ex_info.err_type = e->err_type;
	se->ex_info.err_code = e->err_code;
	strncpy(se->ex_info.err_msg, e->err_msg, sizeof(se->ex_info.err_msg));

	longjmp(se->exec_status, 1);
}
////////////////////////////////////////////////////

int pceherr(CEH_EXCEPTION *ceh_ex_info)
{
	perror(ceh_ex_info->err_msg);
	fprintf(stderr, "err_type: %d err_code: %d\n", ceh_ex_info->err_type, ceh_ex_info->err_code); 

	return ceh_ex_info->err_code;
}
