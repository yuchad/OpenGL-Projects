Name: Chad Yu
Student Number: 500572586

To run the program, openGL must be running with the dependencies freeglut.dll and glew32.dll included.

I used the youtube link: https://www.youtube.com/watch?v=8p76pJsUP44

that the professor has provided to setup my system exactly the same including using Visual Studios 2015.

I included all the files, however if your compilation is similar to the professor's set-up on windows, then the source files

CubeMesh.h
main.cpp
QuadMesh.cpp
QuadMesh.h
VECTOR3D.h

are all the files that should be needed. 

For instructions on how my project works, the char 't' can be pressed to bring up the instructions in the terminal popup that is opened along with the glut frame.

My program does NOT use a timer, rather calling idleFunc as long as the motors are spinning. 

Lastly, as for strategies regarding the sub leaving camera view, I implemented an imaginary hitbox around the sub whereas the sub cannot move outside and keeps it's acceleration . 


