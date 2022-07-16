# 2DWPU
This is an experimental processor architecture I developed back in 2010-2012, which uses two dimensional layout of the instructions. 

**WARNING:** I am publishing this repo as-is, with the code in its original state from back in 2012, mistakes, unfinished parts, code quality I'm no longer happy with and such, but if you want to poke around, hope you'll have fun with it!

The repo contains a compiler/assembler, GUI simulator (this one needs Qt) and hardware version for FPGA.

# (Not) Building
Currently I do not have the project in a state that would build easily with latest versions of Visual Studio. If anybody would like to upgrade it, feel free to submit a PR! Here's some info:

The GUI simulator requires Qt 4.8: https://download.qt.io/archive/qt/4.8/4.8.0/

The Qt 4.8 requires Visual Studio Tools 2010 to link against the Qt libraries. There are two options:
- Compile Qt 4.8 for newer visual studio
- Compile the 2DWPU project with the VS2010 tools

I used the Altera DE2 for building the hardware version on the FPGA. I'm not entirely sure what state I left it in, so you'll have to poke around and figure it out yourself, I'm leaving the stuff as-is.

# Overview (from original specification)

2DWPU is experimental processor architecture from the WPU (Weird Processing Unit) series with accompanying 
programming language 2DASM. WPU’s attempt to approach the processor architecture design, machine code 
processing and programming with unusual, unconventional, playful and creative way for several purposes, including 
curiosity, education, fun and even an artistic intent.

This experimental architecture changes the conventional program flow by logically arranging instructions into a two 
dimensional grid and making these instructions query their surrounding instructions in four directions and returning 
a value after they are done. Programs flow is them moving around this two dimensional grid, expanding in various 
directions, retracting back and “bouncing” into expansion again at some point, usually in a different direction. 

Program flow presented by 2DWPU brings various challenges, as having to design various algorithms and loops 
differently than with traditional architectures or having to optimally arrange the instructions in a 2D grid in addition 
to writing the program itself, which also makes optimization for both size and speed more difficult and challenging to 
the programmer. However, thanks to the top-down programming model, some aspects of the assembly 
programming are simplified.

Furthermore, it presents an option for automatic code parallelism even when evaluating simple expressions, as 
instructions can branch into two separate execution threads and later join back, after result for each branch is 
computed. Depending on the amount of arithmetic cores, the two separate branches can furthermore branch into 
more branches executed in parallel and join afterwards.

Two dimensional grid can hold 32x32 instructions and it’s called a program block, thus a program block can contain 
up to 1024 instructions. There can be several programs blocks stored in the memory, but jump from one block to 
another clears the expansion path, so any parameters must be passed from block to another block using registers or 
the memory.

Memory addressing is 24 bit, allowing up to 16 MB of RAM which holds both program and data. However largest 
possible addressable memory unit is 32 bits and processor internally works with 32 bit values.

Part of the 2DWPU is a IO interface, which is used for communicating with various external devices for input and 
output. There is a set of default devices defined by this specification, that should be present with every 2DWPU 
implementation at fixed addresses providing a standard base environment for creating programs, however the 
interface is expansible and it’s possible to implement and connect more devices of any design.
