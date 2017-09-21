# Custom-Command-Line-Interpreter
Created a custom CLI. Developed specialized grep, pwd, cd, ls commands; along with I/O redirection, 
piping, string tokenization and command history. Programmed in C.

*Run makefile and then run cli file.



Commands that I implement through the CLI:

1. Exit: Causes the CLI to exit, alternative to alt-D

2. cd dir: Changes CLI's current working directory to one designated by the dir statement. Makes use of chdir() function and shows result
via stdout.

3. pwd: displays absolute path of CLI's current working directory. The output becomes part of the prompt for the next command.

4. fref: Searches through one or more files and reports any lines in those files that contain a string  matching the specified
regular expression. We can also use special characters such as "x, . , ^ , $ , *" to make our search more rigorous. 

5. listf: new rendition of the UNIX ls command. Lists file information of one or more directories. give the -l option to see: File type,
access permission bits, number of links to the file, name of file owner,  file's owner group, file size in bytes, last modification date, 
and file name. Permiations of these results can be shown using the -a, -c, and -m options. listf is an external command via child process.

6. calc: handles arithmetic operations. Handles arbitrary number of spaces as well. calc is an external command via child process.

Additional Features:

- Can also redirect stdout and stdin using the "<" and ">" commands.

- Can also enable piped commands using the "|" symbol.

- Also includes command history. Scan through the history using the up and down arrow keys.

- Note: The ttymode file allows us to switch between cbreak mode and cooked mode to allow the command history feature to function.
