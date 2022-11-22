# Parallel Data Encoding
Data compression is the process of encoding information using fewer bits than the original representation. Run-length encoding (RLE) is a simple yet effective compression algorithm: repeated data are stored as a single data and the count. In this lab, I build a parallel run-length encoder called **Not Your Usual ENCoder**, or **NYUENC** for short. If multiple files are passed to nyuenc, they will be concatenated and encoded into a single compressed output. It takes an optional command-line option -j jobs, which specifies the number of worker threads. (If no such option is provided, it runs sequentially.) 

## Usage
$ echo -n "aaaaaabbbbbbbbba" > file.txt  
$ xxd file.txt  
0000000: 6161 6161 6161 6262 6262 6262 6262 6261 aaaaaabbbbbbbbba  

$ ./nyuenc -j 3 file.txt file.txt > file2.enc  
$ xxd file2.enc  
0000000: 6106 6209 6107 6209 6101 a.b.a.b.a.

$ time ./nyuenc file.txt > /dev/null  
real 0m0.527s  
user 0m0.475s  
sys 0m0.233s  

$ time ./nyuenc -j 3 file.txt > /dev/null  
real 0m0.191s  
user 0m0.443s  
sys 0m0.179s  