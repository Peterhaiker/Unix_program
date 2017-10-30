/*
 * client.c
 * Copyright (C) 2017-10-30 15:37 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include"square.h"

int main(int argc,char*argv[])
{
  CLIENT*cl;
  square_in in;
  square_out*outp;
  if(argc!=3){
    puts("usage:./client <hostname> <integer-value>");
    return 0;
  }

  cl=clnt_create(argv[1],SQUARE_PROG,SQUARE_VERS,"tcp");

  in.arg1=atol(argv[2]);
  if((outp=squareproc_1(&in,cl))==NULL){
    printf("%s",clnt_sperror(cl,argv[1]));
    return 0;
  }
  printf("result: %ld\n",outp->res1);
  return 0;
}
