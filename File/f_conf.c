/* f_conf.c
 * Copyright (C) 2018-01-11 15:21 
 * author:peterhaiker
 * email:vim.memory@gmail.com
 *
 * Distributed under terms of the MIT license.
 * Discribed:This program get some file configration u * by use the funciotn fpathconf/pathconf
 */

#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int config_argue[]={_PC_LINK_MAX,_PC_MAX_CANON,_PC_MAX_INPUT,_PC_NAME_MAX,_PC_PATH_MAX,_PC_PIPE_BUF,_PC_CHOWN_RESTRICTED,_PC_NO_TRUNC,_PC_VDISABLE,EOF};

char*argue_str[]={"_PC_LINK_MAX","_PC_MAX_CANON","_PC_MAX_INPUT","_PC_NAME_MAX","_PC_PATH_MAX","_PC_PIPE_BUF","_PC_CHOWN_RESTRICTED","_PC_NO_TRUNC","_PC_VDISABLE"};
int main(int argc,char*argv[])
{
  if(1==argc){
    puts("usage:./f_conf <filepath> ...");
    return 0;
  }
  printf("****************file configuration****************\n");
  for(int config_num,i=1;i<argc;++i){
    for(int j=0;EOF!=config_argue[j];j++){
      if(_PC_MAX_CANON==config_argue[j]||_PC_MAX_INPUT==config_argue[j]||_PC_VDISABLE==config_argue[j])
        config_num=fpathconf(STDIN_FILENO,config_argue[j]);
      else if(_PC_PIPE_BUF==config_argue[j]){
        //get file struct stat
        struct stat stat_buf;
        if(-1==stat("argv[i]",&stat_buf)){
          printf("%s:file type isn't fifo or pipe\n",argue_str[j]);
          continue;
        }
        //if file's type is fifo
        else if(S_ISFIFO(stat_buf.st_mode))
          config_num=pathconf(argv[i],config_argue[j]);
        else{
          printf("%s:file type isn't fifo\n",argue_str[j]);
          continue;
        }
      }
      else
          config_num=pathconf(argv[i],config_argue[j]);

      if(-1==config_num)
        printf("%s:unknown request\n",argue_str[j]);
      else
        printf("%s:%d\n",argue_str[j],config_num);
    }
  }
  return 0;
}
