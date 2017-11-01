/*
 * client_fifo.c
 * Copyright (C) 2017-10-31 12:16 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:client make client fifo first,then passing a filename and pid to server fifo.server will answer the file state,if exist,it will return file's content
 */

#include "stdio.h"
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"fifo.h"
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int client_fd;
  int server_fd=open(FIFO_SERVER,O_WRONLY);
  char client_filename[50]={'\0'};
  char buffer[502]={'\0'};
  char filename[50];
  int n;
  while(1){
    printf("input filename you want to search(^R to quit):");
    fgets(filename,50,stdin);
    if(1==strlen(filename)||10==filename[0]){
      puts("bye");
      break;
    }
    //create client fifo 
    snprintf(client_filename,50,"%d",getpid());
    mkfifo(client_filename,S_IRUSR|S_IWUSR);

    //passwing filename and pid to server
    filename[strlen(filename)-1]='\0';
    snprintf(buffer,502,"%s %d",filename,getpid());
    write(server_fd,buffer,502);

    //open it with readonly nonblock
    client_fd=open(client_filename,O_RDONLY);

    //wait for read message from server
    while((n=read(client_fd,buffer,502))>0)
      if(n>0)
        write(STDOUT_FILENO,buffer,n);
  }
  close(client_fd);
  unlink(client_filename);
  return errno;
}
