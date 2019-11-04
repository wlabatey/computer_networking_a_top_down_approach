**CMPSCI 591E**

Computer Networking

Fall 2000

Professor Jim Kurose

## Programming Assignment 3: Questions and Answers about the assignment

Some of us are having a problem compiling Jim's code with our own,
because the compiler claims that the time variable has not been
declared. The command to compile given in the assignment doesn't do
the job. What do I need to do to get the compiler to recognize his
declaration in my code?

   You can declare it as an external variable in each file (e.g.
   node?.c) in which you want to refer to it, i.e.:

    /* students to write the following two routines, and maybe some
    others */

    /* use Jim's emulator clock in this file */
    extern float clocktime;
