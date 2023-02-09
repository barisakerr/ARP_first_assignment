# ARP-Hoist-Assignment
*Advanced and Robot Programming* (ARP) first assignment.

## Project made by:

Claudio Tomaiuolo S5630055

Barış Aker S5430437

## ncurses installation
To install the ncurses library, simply open a terminal and type the following command:
```console
sudo apt-get install libncurses-dev
```

## Compiling and running the code
Make all the .c files inside `src/` executable, running:
```console
chmod +x *.c
```
Move with the terminal to the folder where there are `compile.sh` and `run.sh` files. Run:
```console
./compile.sh
```
then:
```console
./run.sh
```

## Troubleshooting

Should you experience some weird behavior after launching the application (buttons not spawning inside the GUI or graphical assets misaligned) simply try to resize the terminal window, it should solve the bug.


## Explanation

The purpose of the Project is to control the hoist which has two degrees of freedom with the user inputs this aim had been achieved by creating six different processes. The hoist has two motors, first motor is providing to move the hoist on x axis, and the second motor is providing to move the hoist on z axis. When the program is running the user will see two main consoles. First console is the command console to take the user inputs and the second one is the inspection console to see the hoist movement. 

The first window is command console. Inside the window there are six buttons. The first three buttons are: ‘Vx-‘ to decrease the velocity on x-axis, the second button is ‘S’ to stop the hoist on x-axis, and the third button is ‘Vx+’ to increase the velocity on x-axis. The other three buttons are: ‘Vz-‘ to decrease the velocity on z-axis, the second button is ‘S’ to stop the hoist on z-axis and the third button is ‘Vz+’ to increase the velocity on z-axis. User enters the input with using mouse by clicking the buttons. There are two velocity steps, if the user clicks to the ‘Vx+’ button the velocity on the x-axis become +1 and the hoist will starts to move on the x-axis. If the user will click again to the ‘V+’ button, the velocity becomes +2 and the hoist will move on x-axis faster. 2 is the maximum speed set on all axes and directions. Inside of the command window there are two pipes had been created to send the inputs to the motor x and the motor z processes.  

The second and the third processes are motor x and motor z windows. Inside of the motor x and motor z the pipes which is coming from the command console had been read and the inputs are received. Then, inside of the motor x and motor z, the velocity and the position are calculated every second. Furthermore, one pipe had been created for each motor to send the position values to the world console. 

World is the fourth process of the project. It includes the pipes coming from the motor x and the motor z reading data about position. The purpose of the world console is to simulate the real-life behavior with the friction, wind and the other possible environmental effects. To reach that, random values are calculated for each received data and added to those. After this process, there is a pipe to send the new “real position” to the inspection console.

The purpose of the inspection is to show the movement of the hoist to the user. Also, there are two important buttons, working with signals:
-‘R’ button (Reset). When pressed, the hoist moves to the initial position on x-axis and z-axis, at a speed of -5.
-‘S’ button (Stop). When pressed, all processes stop immediately.

The sixth process is the watchdog. The purpose of the watchdog to kill all the processes and itself if no button is pressed for 60 seconds.
