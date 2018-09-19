# Runtime errors in Little-CMS 

Little-CMS version: 
https://github.com/mm2/Little-CMS.git

Test driver: lcms.c

Steps:
1. Compile the proj.4 source code into static library with '-fsanitze=undefined' enabled.
2. Compile test driver: $CC ./lcms.c -lubsan -lm -I /path/to/source/code/include/ /path/to/liblcms.a -o lcms 
3. Run: ./lcms input1

Input files:
input1

Error found: 
input1 ==> runtime error: "value 3.71263e+49 is outside the range of representable values of type 'float'
