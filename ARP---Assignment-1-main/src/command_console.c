#include "./../include/command_utilities.h"
#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>


// log
int log_fd;
char log_buffer[100];

// Variable declaration in order to get the time
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

void SendVelocity(char * myfifo, int Vel){
    int fd;
    if((fd = open(myfifo, O_WRONLY))==-1){
        close(log_fd);
        perror("Error.");
        exit(1);
    }
    if(write(fd, &Vel, sizeof(Vel))==-1){
        close(log_fd);
        close(fd);
        perror("Error in writing function");
        exit(1);
    }
    close(fd);
}

int main(int argc, char const *argv[]){

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize User Interface 
    init_console_ui();

    //Defining the first named pipe between command window and motor 1  
    int fd;
    char * VxFifo = "/tmp/VxFifo";
    mkfifo(VxFifo, 0666);
    char * VzFifo = "/tmp/VzFifo";
    mkfifo(VzFifo, 0666);

    // Open the log file
    if ((log_fd = open("log/command.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1){
        // If the file could not be opened, print an error message and exit
        perror("Error opening command file");
        exit(1);
    }

    // Infinite loop
    while(TRUE){

        // Get current time
        time(&rawtime);
        info = localtime(&rawtime);
        
        fflush(stdout);

        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();

        // If user resizes screen, re-draw UI
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            } else {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) {

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // Vx-- button pressed
                if(check_button_pressed(vx_decr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Decreased");

                    SendVelocity(VxFifo,0);
                    WriteLog("Vx-- button pressed.");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vx++ button pressed
                else if(check_button_pressed(vx_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Increased");

                    SendVelocity(VxFifo,1);
                    WriteLog("Vx++ button pressed.");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vx stop button pressed
                else if(check_button_pressed(vx_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Motor Stopped");

                    SendVelocity(VxFifo,2);
                    WriteLog("Vx stop button pressed.");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vz++ button pressed
                else if(check_button_pressed(vz_decr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Decreased");

                    SendVelocity(VzFifo,0);
                    WriteLog("Vz++ button pressed.");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vz-- button pressed
                else if(check_button_pressed(vz_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Increased");
                    
                    SendVelocity(VzFifo,1);
                    WriteLog("Vz-- button pressed.");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vz stop button pressed
                else if(check_button_pressed(vz_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Motor Stopped");

                    SendVelocity(VzFifo,2);
                    WriteLog("Vz stop button pressed.");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }  
            }
        }
        refresh();
	}

    close(log_fd);

    // Terminate
    endwin();
    return 0;
}
