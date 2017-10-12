//这个程序利用system V message queue来实现并发的服务器模型，服务器为每个客户端分配一个线程执行
//v1.0服务器没有使用线程导致出错
//v1.1加入线程
#include<stdio.h>
#include<string.h>
#include<sys/msg.h>
#include<unistd.h>
#include"sysVmq_concurrent.h"
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  //test the command line format
  if(argc!=2){
    perror("usage:sysVmq_concurrent_client filename\n");
    return errno;
  }

  //create client message queue
  int climsqid,sermsqid;
  if((climsqid=msgget(IPC_PRIVATE,IPC_CREAT|0644))==-1){
    perror("create client sysV message queue failed\n");
    return errno;
  }

  //open server message queue
  if((sermsqid=msgget(ftok(PATH,0),0200))==-1){
    perror("open server sysV message queue failed\n");
    return errno;
  }
  struct msgbuf{
    long mtype;
    char filename[1024];
  }sndbuf,rcvbuf;
  sndbuf.mtype=getpid();
  //this maybe cause a bug if a filename begin with a number
  snprintf(sndbuf.filename,1024,"%d%s",climsqid,argv[1]);
  //test
  puts("a");
  char filename[1024]={'\0'};
  while(1){
    if(msgsnd(sermsqid,&sndbuf,1024,0)!=-1){
      if(msgrcv(climsqid,&rcvbuf,1024,0,0)!=-1)
        printf("type:%ld,file state:%s\n",rcvbuf.mtype,rcvbuf.filename);
      else
        printf("receive message failed\n");
    }
    printf("input a filename you want to search:(^e to quit):");
  fgets(filename,1024,stdin);
  if(filename[0]==5) //5 is ^e
    break;
  filename[strlen(filename)-1]='\0';
  snprintf(sndbuf.filename,1024,"%d%s",climsqid,filename);
  }
  msgctl(climsqid,IPC_RMID,NULL);
  return errno;
}
