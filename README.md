
# cow lang    
```
                    ________
                    | mooh |
           __n__n__ |______|
    .------`-\00/-'__/
   /  ##  ## (oo)
  / \## __   ./
     |//YY \|/
_ _ _|||_ _|||_ _ _ _
```

__WIP__ still unstable, etc.

very simple scripting language, written in c. <br>
by very simple i means, it's basically assembly. <br>

i'm building this as a stepping stone to making actual programming languages. <br>
so this is very not good. <br>


## table of contents
   - [introduction]()
   - [roadmap]()


## introduction

`.mooh` extension on files

```asm
# comment

mov a 3   # assign a 3, i.e. a = 3

jsr rtp   # jump to subroutine, basically calling func

-> rtp:   # soubroutine, basically a function
loop:     # label
out "hello, world"    # print to console
sub a 1   # subtract 1 from a
jnz loop  # jump not zero, so if the last operation wasnt equal to 0
<- rts    # return subroutine, go back where jsr was called from 

```

## roadmap

 - [ ] 

