#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <math.h>


float OldXpos=0;
float OldZpos=0;
float Xpos=0;
float Zpos=0;
float RealXpos=0;
float RealZpos=0;
float Pos[2];

// set up it;
const float Xmin=0;
const float Xmax=40;
const float Zmin=0;
const float Zmax=10;

int log_fd;
char log_buffer[100];

time_t rawtime;
struct tm *info;

// write log file
void WriteLog(char * msg){
    sprintf(log_buffer, msg);
    sprintf(log_buffer + strlen(log_buffer), asctime(info));

    if(write(log_fd, log_buffer, strlen(log_buffer))==-1){
        close(log_fd);
        perror("Error in writing function");
        exit(1);
    }
}

float ErrorGenerator(float tempPos){
    if (tempPos==0.0){
        return 0.0;
    }
    int error=(int)(tempPos);
    int n=(rand()%(error-(-error)+1))+(-error);
    WriteLog("Error generated.");
    return (float)n/100;
}

void SendPosition(char * myfifo, float RealPos[2]){
    int fd;
    if ((fd = open(myfifo, O_WRONLY))==-1){ 
        close(log_fd);
        perror("Error opening fifo");
        exit(1);
    }
    if(write(fd, RealPos, sizeof(RealPos))==-1){
        close(log_fd);
        perror("Error in writing function");
        exit(1);
    }
    // WriteLog("Position sent.");
    close(fd);
}

int main(int argc, char const *argv[]){
    int fd;
    char * PxFifo = "/tmp/PxFifo";
    char * PzFifo = "/tmp/PzFifo";
    char * RealPosFifo = "/tmp/RealPosFifo";
    mkfifo(PxFifo, 0666);
    mkfifo(PzFifo, 0666);
    mkfifo(RealPosFifo, 0666);

    // Open the log file
    if ((log_fd = open("log/world.log",O_WRONLY|O_APPEND|O_CREAT, 0666))==-1){
        perror("Error opening world log file");
        return 1;
    }

    while(1){
        // Get current time
        time(&rawtime);
        info = localtime(&rawtime);

        if(fd = open(PxFifo, O_RDONLY)){
            if(read(fd, &Xpos, sizeof(Xpos))==-1){
                close(log_fd);
                close(fd);
                perror("Error reading fifo");
                exit(1);
            }

            close(fd);
        } else {
            close(log_fd);
            close(fd);
            perror("Error opening fifo");
            exit(1);
        }

        if(fd = open(PzFifo, O_RDONLY)){
            if(read(fd, &Zpos, sizeof(Zpos))==-1){
                close(log_fd);
                close(fd);
                perror("Error reading fifo");
                exit(1);
            }
            close(fd);
        } else {
            close(log_fd);
            close(fd);
            perror("Error opening fifo");
            exit(1);
        }


        if(Xpos!=OldXpos){
            RealXpos=Xpos+ErrorGenerator(Xpos);
            if(RealXpos>40){
                RealXpos=40;
            } else if(RealXpos<0){
                RealXpos=0;
            }
            OldXpos=Xpos;
        }

        if(Zpos!=OldZpos){
            RealZpos=Zpos+ErrorGenerator(Zpos);
            if(RealZpos>10){
                RealZpos=10;
            } else if(RealZpos<0){
                RealZpos=0;
            }
            OldZpos=Zpos;
        }

        Pos[0]=RealXpos;
        Pos[1]=RealZpos;

        SendPosition(RealPosFifo,Pos);
        
    }
    close(log_fd);

    return 0;
}