target extended-remote :3333
monitor reset halt
load
monitor reset init
set pagination off
break main
layout src
focus cmd
continue