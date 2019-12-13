// char*optstring = “ab:c::”;
// 单个字符a         表示选项a没有参数            格式：-a即可，不加参数
// 单字符加冒号b:     表示选项b有且必须加参数      格式：-b 100或-b100,但-b=100错
// 单字符加2冒号c::   表示选项c可以有，也可以无     格式：-c200，其它格式错误

#include <unistd.h>
#include <stdio.h>
int main(int argc, char * argv[]){
    int ch;
    while ((ch = getopt(argc, argv, "a::b::c::")) != -1){
        switch (ch){
            case 'a':
                printf("%s\n", optarg);
                break;
            case 'b':
                printf("%s\n", optarg);
                break;
            case 'c':
                printf("%s\n", optarg);
                break;
            case '?':
                printf("Unknown option: %c\n",(char)optopt);
                break;
            }
       }


}
