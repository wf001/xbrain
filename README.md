
# The XBrain Programming language 

XBrain is an experimental programming language inspired by Brainfuck, derived from  [skeeto/bf-x86](https://github.com/skeeto/bf-x86). 

<div align="center">
<img src="https://i.imgur.com/CD9bYFx.png" alt="xbrain logo">
</div>


## Language overview
Brainfuck consists of  8 commands  as following,
* \+: Increment the data pointer, equal to (*ptr)++ in C.
* \-: Decrement the data pointer, equal to (*ptr)-- in C.
- \> :Increment the byte at the data pointer, equal to ptr++ in C.
* < :Decrement the byte at the data pointer, equal to ptr-- in C.
* . :Output the byte at the data pointer as ascii character, equal to putchar(*ptr) in C.
* , :Accept one byte of input, storing its value in the byte at the data pointer, equal to getchar() in C.
* [ : Equal to while(*ptr) in C.
* ] : Equal to } in C.

XBrain consists of additional 4 commands as following,
* / : Comment
```
/this is comment
++++++++++++++++++++++++++++++++++++++++++++++++.
```
```
-> 0
```

* $ : Print the byte at the data pointer as a hex number.
```
+$
```
```
-> 0x1
```

* &: Do Logical conjunction between the byte at the data pointer and the byte at left (=previous ) cell.
```
/0x22
++++++++++++++++++++++++++++++++++
>
/0x35
+++++++++++++++++++++++++++++++++++++++++++++++++++++&$
```
```
-> 0x20
```

* |:Do Logical disjunction between the byte at the data pointer and the byte at left (=previous ) cell.
```
/0x22
++++++++++++++++++++++++++++++++++
>
/0x35
+++++++++++++++++++++++++++++++++++++++++++++++++++++|$
```
```
-> 0x37
```

## get started
### Binary
Binary distributions are available with following command:

``` bash
$ curl https://static.psaux.jp -o xbc
```

### From Source
You can also get  XBrain compiler from source with following command:

``` bash
$ git clone https://github.com/wf001/xbrain
$ cd xbrain
$ make
```
Make sure you have installed the dependencies:

- gcc
- make

And you can make ELF-formatted Executable file with following command:
``` bash
$ ./xbc [your_file].xb // generate ELF-formatted file
$ ./[your_file] //run
```


## Additional Resources

* [Original Brainfuck implementation](http://esoteric.sange.fi/brainfuck)
* [ Brainfuck implementation XBrain come from](https://github.com/skeeto/bf-x86)
