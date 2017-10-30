/*
 * server.c
 * Copyright (C) 2017-10-30 15:40 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include"square.h"

square_out*squareproc_1_svc(square_in*inp,struct svc_req*rqstp)
{
  static square_out out;
    out.res1=inp->arg1*inp->arg1;
  return(&out);
}
