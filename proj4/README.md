# Runtime errors in proj.4 

Proj4 version: 
https://github.com/OSGeo/proj.4.git

Test driver: proj.c

Steps:
1. Compile the proj.4 source code into static library 
2. Compile test driver: $CC ./proj.c -DSTANDALONE -lubsan -lm -I /path/to/src/ /path/to/libproj.a -o proj
3. Run: ./proj input_file (E.g., ./proj input1)

Input files:
input1, input2

Error found: 
input1 ==> PJ_labard.c:115:17, runtime error: division by zero
input2 ==> PJ_somerc.c:83:58: runtime error: division by zero
       ==> PJ_somerc.c:49:53: runtime error: division by zero
