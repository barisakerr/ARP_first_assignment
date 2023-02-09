#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>
#include <time.h>


float Xvel=0;
float XvelPlus=0;
float Xpos=0;

// set up it;
const float Xmin=0;
const float Xmax=40;
const float Vmax=2;

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

void SendPosition(const char * myfifo, float Xpos){
    int fd;
    if ((fd = open(myfifo, O_WRONLY))==-1){
        close(log_fd);
        perror("Error opening fifo");
        exit(1);
    }
    if(write(fd, &Xpos, sizeof(Xpos))==-1){
        close(fd);
        close(log_fd);
        perror("Error in writing function");
        exit(1);
    }
    close(fd);
}

float VelCalculator(const char * myfifo, int command, float Xvel){
    int fd;
    if(fd = open(myfifo, O_RDONLY|O_NDELAY)){
        if(read(fd, &command, sizeof(command))==-1){
            close(log_fd);
            close(fd);
            perror("Error reading from fifo");
            exit(1);
        }
        if(command==0){
            WriteLog("Vx-- button pressed.");
            return -1.0;
        } else if(command==1){
            WriteLog("Vx++ button pressed.");
            return 1.0;
        } else if(command==2){
            WriteLog("Vx stop button pressed.");
            return 2.0;
        } else {
            return 0.0;
        }
    }
    close(fd);
    perror("Error opening fifo");
    exit(1);
}

void stop_handler(int sig){
    if(sig==SIGUSR1){
        // Stop the motor
        Xvel=0;

        // Listen for stop signal
        if(signal(SIGUSR1, stop_handler)==SIG_ERR){
            exit(1);
        }
    }
}

// Reset signal handler
void reset_handler(int sig){
    if(sig == SIGUSR2){
        // Setting velocity to -5
        Xvel=-5;
        
        // Listen for stop signal
        if(signal(SIGUSR2, reset_handler)==SIG_ERR){
            exit(1);
        }
    }
}

int main(int argc, char const *argv[]){
    // definition of the second named pipe between the command console and motor2
    int fd;
    char * VxFifo = "/tmp/VxFifo";
    mkfifo(VxFifo, 0666);
    char * PxFifo = "/tmp/PxFifo";
    mkfifo(PxFifo, 0666);

    int command = -1;

    // Open the log file
    if ((log_fd = open("log/motorX.log",O_WRONLY|O_APPEND|O_CREAT, 0666))==-1){
        perror("Error opening motorX log file.");
        return 1;
    }

    // Listen for signals
    if(signal(SIGUSR1, stop_handler)==SIG_ERR || signal(SIGUSR2, reset_handler)==SIG_ERR){
        // Close file descriptors
        close(VxFifo);
        close(PxFifo);
        close(log_fd);
        exit(1);
    }

    while(1){
        time(&rawtime);
        info = localtime(&rawtime);

        if(Xvel!=-5){
            XvelPlus = VelCalculator(VxFifo, command, Xvel);
            if(XvelPlus==2){
                Xvel=0;
                XvelPlus=0;
            } else {
                Xvel=Xvel+XvelPlus;
                if (Xvel<-Vmax){
                    Xvel=-Vmax;
                } else if (Xvel>Vmax){
                    Xvel=Vmax;
                }
            }
        }
        
        Xpos=Xpos+Xvel;
        if (Xpos<Xmin){
            Xpos=Xmin;
            Xvel=0;
        } else if (Xpos>Xmax){
            Xpos=Xmax;
            Xvel=0;
        }

        SendPosition(PxFifo, Xpos);

        sleep(1);
    }

    close(log_fd);
    return 0;
}