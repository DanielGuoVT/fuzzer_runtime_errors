# Runtime errors in proj.4 

### proj.4 version: 
https://github.com/OSGeo/proj.4.git

### Test driver: proj.c

### Compile the proj.4 source code into static library 
```bash
git clone https://github.com/OSGeo/proj.4.git src
rm -fr build
cp fr src build
cd build && ./autogen.sh
make clean
./configure
make clean
make -j4
cd ..
```

### Compile test driver: 
```bash
$CC ./proj.c -DSTANDALONE -lubsan -lm -I /path/to/src/ /path/to/libproj.a -o proj
```
### Run: 
```bash
./proj input_file (E.g., ./proj input1)
```

### Input files:
input1, input2

### Error found: 
input1 ==> PJ_labard.c:115:17, runtime error: division by zero
input2 ==> PJ_somerc.c:83:58: runtime error: division by zero
input2 ==> PJ_somerc.c:49:53: runtime error: division by zero
