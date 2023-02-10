#include "./../include/inspection_utilities.h"
#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <math.h>


float Pos[2];

int log_fd;
char log_buffer[100];

time_t rawtime;
struct tm *info;

// writing the log file
void WriteLog(char * msg){
    sprintf(log_buffer, msg);
    sprintf(log_buffer + strlen(log_buffer), asctime(info));

    if(write(log_fd, log_buffer, strlen(log_buffer))==-1){
        close(log_fd);
        perror("Error in writing function");
        exit(1);
    }
}

int main(int argc, char const *argv[]){

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initializing the End-effector coordinates of the x and y
    float ee_x, ee_y; // these values and the parameters which are located at the below need to make sence together be in attention to them 
    ee_x = 0.0;
    ee_y = 0.0; 

    // Initializing the User Interface 
    init_console_ui();

    int fd;
    char * RealPosFifo = "/tmp/RealPosFifo";
    mkfifo(RealPosFifo, 0666);

    pid_t pid_motorX = atoi(argv[1]);
    pid_t pid_motorZ = atoi(argv[2]); 

    // Opening the log file
    if ((log_fd = open("log/inspection.log",O_WRONLY|O_APPEND|O_CREAT, 0666))==-1){
        // If the file will not open, printing an error message 
        perror("Error opening command file");
        
    }

    
    while(TRUE){
        
        // Taking current time
        time(&rawtime);
        info = localtime(&rawtime);
        
        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();

        // If user resizes screen, re-draw user  interface
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) {

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // STOP button pressed
                if(check_button_pressed(stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "STP button pressed");

                    WriteLog("STOP button pressed.");

                    kill(pid_motorX, SIGUSR1);
                    kill(pid_motorZ, SIGUSR1);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // RESET button pressed
                else if(check_button_pressed(rst_button, &event)) {
                    mvprintw(LINES - 1, 1, "RST button pressed");

                    WriteLog("RESET button pressed.");

                    kill(pid_motorX, SIGUSR2);
                    kill(pid_motorZ, SIGUSR2);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }
        

        // Opening the pipes for read
        if ((fd = open(RealPosFifo, O_RDONLY))==-1){
            close(fd);
            perror("Error opening fifo");
        }

        // Read the pipes from World
        if(read(fd, Pos, sizeof(Pos)) == -1){
            close(fd);
            perror("Error reading fifo");
        }

        // closing pipe
        close(fd);

        ee_x=Pos[0];
        ee_y=Pos[1];
       
        // Updating the user interface 
        update_console_ui(&ee_x, &ee_y);
        sleep(1);
	}

    // closing log file 
    close(log_fd);

    // Terminate
    endwin();
    return 0;
}

