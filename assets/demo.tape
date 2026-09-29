Output "demo.gif"

Set Theme "Dracula"
Set FontFamily "Liberation Mono"
Set FontSize 40
Set Width 1500
Set Height 600
Set Padding 20
Set Shell "bash"
Set TypingSpeed 100ms  

Hide
Type "source ~/.bashrc"
Enter
Type "export PS1='\w $ '"
Enter
Type "clear"
Enter
Sleep 500ms
Show

Type "vim test.txt"
Sleep 500ms
Enter
Sleep 2s

Type "i"
Sleep 500ms
Type "This is a test file for cnav"
Sleep 2s
Escape
Sleep 500ms
Type ":wq"
Enter
Sleep 2s

Type "cd ~/Documents"
Enter
Sleep 2s

Type "n te"
Sleep 500ms
Enter
Sleep 2s
Type ":q"
Sleep 500ms
Enter

Sleep 3s
