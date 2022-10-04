# ACL

* ulimit -s unlimited;taskset -c 2 ./hello
* gcc -o hello ./src/*.c ./src/*.h -lm

* gcc -g src/* -o debug.out -lm
* gdb debug.out

* output文件夹的rule_distribution文件格式说明
  * 第1列表示IP某个字节上要求的固定值 0-255
  * 第2-5列表示源IP的第1-4个字节
  * 第6-9列表示目的IP的第1-4个字节
  * 值表示掩码对这个字节的8位都有要求且 值等于 第一列同一行的值 的规则的个数
  * 最后一行256表示掩码对这个字节上要求的位数不足8位的规则的个数