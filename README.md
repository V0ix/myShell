# myshell
`myshell` is custom Linux shell built in C, featuring:
- **Built-in Commands**: `cd` , `exit`, `help`
- **External Command Execution**: Supports Standard Linux like `ls`, `touch` , etc.
- **Piping**: Handle commands with `|` (e.g., 'ls | grep txt`)
- **Input/Output Redirection**: use `<` and `>` for file redirection
- **User-Friendly Prompt**: Displays currernt working directory with color coding

-  ## Getting Started
-  ### Compilation
-  ```bash
   gcc mysh.c -o mysh
   ```
- ### usage
- ```bash
  ./myshell
  ```
- ### 🧪 Features Demonstration
- #### change Directory
- ```bash
  myshell> cd /path/to/directory
  ```
- #### List File
-  ```bash
   myshell> ls
   ```
- #### Piping
-  ```bash
   myshell> ls | grep
   ```
- #### Redirection
-  ```bash
   myshell> ls > output.txt
   myshell> cat < output.txt
   ```
  
