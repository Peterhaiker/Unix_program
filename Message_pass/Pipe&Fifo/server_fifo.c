/*
 * server_fifo.c
 * Copyright (C) 2017-10-31 12:57 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:server create a server fifo to store filename come from client,then open the file and return it's content to client fifo,if the file isn't exist,return error message
 */

#include "stdio.h"
#include<stdlib.h>
#include"fifo.h"
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int server_fd,client_fd;
  //create and open server fifo
  mkfifo(FIFO_SERVER,0622);
  server_fd=open(FIFO_SERVER,O_RDONLY|O_NONBLOCK);

  char buffer[502]={'\0'};

  while(1){
    if(read(server_fd,buffer,502)>0){

      if(fork()==0){
        //解析参数
        char*filename=strchr(buffer,' ');
        *filename='\0';
        char*client_filename=filename+1;
        filename=buffer;

        //open client fifo
        client_fd=open(client_filename,O_WRONLY);
        //open file client want to search
        int fd=open(filename,O_RDONLY);
        if(fd<0){
          //maybe occur bug
          snprintf(buffer,502,"can't open file:%s\n",filename);
          write(client_fd,buffer,502);
        }
        else{
          int n;
          while(0<(n=read(fd,buffer,502)))
            write(client_fd,buffer,n);
          if(n<0){
            snprintf(buffer,502,"unknown mistake occured when reading");
            write(client_fd,buffer,502);
          }
        }
        exit(0);
      }
    }
  }
  return 0;
}
