#include<stdio.h>
#include<unistd.h>
#include<getopt.h>


// char*optstring = “ab:c::”;
// 单个字符a         表示选项a没有参数            格式：-a即可，不加参数
// 单字符加冒号b:     表示选项b有且必须加参数      格式：-b 100或-b100,但-b=100错
// 单字符加2冒号c::   表示选项c可以有，也可以无     格式：-c200，其它格式错误


int main(int argc, char *argv[])
{
    int opt;
    char *string = "a::b:c:d";
    while ((opt = getopt(argc, argv, string))!= -1)
    {  
        printf("opt = %c\t\t", opt);
        printf("optarg = %s\t\t",optarg);
        printf("optind = %d\t\t",optind);
        printf("argv[optind] = %s\n",argv[optind]);
    }  
}
