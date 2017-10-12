//利用system V 消息队列实现并发C/S通信，此程序为多线程并发。客户端可以一直输入想要查询的文件状态的文件名，给每个客户端分配一个线程，当客户端退出时分配的线程也被回收。如果不及时回收已经退出的线程资源则随着时间的流逝会有越来越多资源被占用
//
//遗留问题1，同一客户端若未终止则分配给它的进程也不能终止,通过线程池实现
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/msg.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<syslog.h>
#include<errno.h>
#include<pthread.h>
#include"sysVmq_concurrent.h"


struct msgbuf{
    long mtype;
    char filename[1024];
};
void*handler(void*arg)
{
    pthread_detach(pthread_self());//detach itself,then system can auto recycling resource when it exit
    int fd,rcv_size;
    struct msgbuf clibuf,srvbuf;
    srvbuf=*(struct msgbuf*)arg;
    clibuf.mtype=srvbuf.mtype;
    int climsqid;
    char*filename;
    //extract filename and client message queue id
    climsqid=strtol(srvbuf.filename,&filename,10);
    //open the file that client want to search
    if((fd=open(filename,O_RDONLY))==-1)
        sprintf(clibuf.filename,"file doesn't exist\n");
    else{
        sprintf(clibuf.filename,"file exist\n");
        close(fd);
    }
    //if failed to send message to client,then log it
    if(msgsnd(climsqid,&clibuf,1024,0)==-1)
        syslog(LOG_ERR,"send message to client pid:%ld failed\n",srvbuf.mtype);
    pthread_exit(0);
}
int main(void)
{
  errno=0;
  signal(SIGCHLD,SIG_IGN);
  //the next step is make it become a daemon running in background
  //set file mask
  umask(0133);

  //fork and exit parent process
  if(fork()>0)
    exit(0);
  sleep(1); //to ensure father process exit

  //now in child process
  //call setsid to create a new session
  setsid();

  //fork again to ensure no tty associated to this process
  if(fork()>0)
    exit(0);
  sleep(1); //to ensure father process exit

  //now in grandprocess
  //change the current working directory to root
  chdir("/");

  //close all file descriptor
  struct rlimit rlim={0,0};
  getrlimit(RLIMIT_NOFILE,&rlim);
  int max_fd=rlim.rlim_max>1024?rlim.rlim_max:1024;
  do{
    close(max_fd);
  }while(max_fd--);

  //redirect stdin stdout,stderr to /dev/null
  int fd0,fd1,fd2;
  fd0=open("/dev/null",O_RDWR);
  fd1=dup(fd0);
  fd2=dup(fd0);

  //init log file
  openlog("sysVmq_concurrent_server",LOG_CONS,LOG_USER);

  //ensure only one server process running
  int lockfd;
  if((lockfd=open(LOCK_FILE,O_CREAT|O_WRONLY,0200))==-1){
    perror("can't create lock file\n");
    return errno;
  }
  struct flock filock;
  filock.l_type=F_WRLCK;
  filock.l_whence=SEEK_SET;
  filock.l_start=0;
  filock.l_len=0;
  if(fcntl(lockfd,F_SETLK,&filock)==-1){
    syslog(LOG_ERR,"can't lock file,another in running?\n");
    return errno;
  }

  //now we ensure only one running
  //ensure stdin stdout stderr correct redirect
  if(fd0!=0||fd1!=1||fd2!=2){
    syslog(LOG_ERR,"unexpected file descriptor");
    return errno;
  }

  //now create the server message queue
  int climsqid,srvmsqid;
  if((srvmsqid=msgget(ftok(PATH,0),IPC_CREAT|0644))==-1){
    syslog(LOG_ERR,"create server message queue failed");
    return errno;
  }

  //now read server message queue and send answer to client
    struct msgbuf clibuf,srvbuf;
    int fd,rcv_size;
  //test
 // syslog(LOG_DEBUG,"before in while loop\n");
  while(1){
    do{
      errno=0;
      rcv_size=msgrcv(srvmsqid,&srvbuf,1024,0,0);
    }while((-1==rcv_size)&&(EINTR==errno));
    if((-1==rcv_size))
      break;
    //test
  //  syslog(LOG_DEBUG,"in while loop\n");
    //if receive 0 bytes,skip this scyle
    if(rcv_size==0)
      continue;
    pthread_t tid;
    if(pthread_create(&tid,NULL,handler,(void*)&srvbuf)){
      //create thread failed,then keep going
      clibuf.mtype=srvbuf.mtype;
      char*filename;
      climsqid=strtol(srvbuf.filename,&filename,10);
      if((fd=open(filename,O_RDONLY))==-1)
        sprintf(clibuf.filename,"file doesn't exist\n");
      else{
        sprintf(clibuf.filename,"file exist\n");
        close(fd);
      }
      if(msgsnd(climsqid,&clibuf,1024,0)==-1)
        syslog(LOG_ERR,"send message to client pid:%ld failed\n",srvbuf.mtype);
    }
  }
}
