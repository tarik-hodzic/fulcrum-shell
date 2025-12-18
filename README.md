<h1>Fulcrum – Custom Unix Shell</h1>

Fulcrum is a custom Unix-like command-line shell written in C.  
The project demonstrates core operating system concepts such as process creation, command execution, pipes, input/output redirection, and basic filesystem utilities.

<h2>Technologies Used</h2>

- C (POSIX)  
- Unix/Linux system calls  
- GCC / Clang compiler  

<h2>Project Purpose</h2>

This project was built to:

- Develop a functional custom shell in C  
- Apply operating system concepts such as fork, exec, wait, and pipes  
- Understand process management and inter-process communication  
- Implement simplified versions of common Unix commands  

<h2>Features</h2>

- Custom shell prompt displaying user and host information  
- Execution of external system commands  
- Built-in commands:
  - wc – word, line, and character count  
  - grep – pattern search within files  
  - df – disk usage information  
  - cmatrix – matrix-style terminal animation  
  - execle – example of executing a process with a custom environment  
- Support for command pipelines using |  
- Output redirection using >  
- Safe handling of dangerous commands (fork bomb disabled)  

<h2>Supported Commands</h2>

Examples of supported input:

bash
ls -l
wc file.txt
grep error log.txt
ls | grep txt
ls | grep txt > output.txt
df
cmatrix

<h2>How It Works</h2>

- User input is read from standard input
- Commands are parsed and separated by pipes
- Each command in a pipeline is executed in its own child process
- Pipes redirect output between commands
- Output redirection writes command results to a file

<h2>How to Run the Project</h2>

- Compile the program:
gcc fulcrum.c -o fulcrum

- Run the shell:
./fulcrum

Enter commands directly into the shell prompt

<h2>Limitations</h2>

- No background process execution (&)
- No command history or auto-completion
- Basic argument parsing (whitespace-based)
- Intended for educational and demonstration purposes

<h2>Project Scope</h2>

This project focuses on demonstrating low-level operating system concepts, process control, and shell mechanics rather than replacing a full-featured system shell.

<h2>Authors</h2>

Created by Tarik Hodžić, Naim Pjanić
