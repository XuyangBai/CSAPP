#include"cachelab.h"
#include"stdlib.h"
#include"stdio.h"
#include"unistd.h"
#include<math.h>
#include"string.h"
#include"getopt.h"
int hit_count, miss_count, eviction_count;
int s = 0;
int E = 0;
int b = 0;
typedef struct{
    int valid;
    int tag;
    int time_stamp;
}cache_line;
cache_line *cache = NULL;

void initialCache(){
    // 第set_index 组第i个cache_line即 cache[(set_index-1) * E + i - 1]
    cache = (cache_line*)calloc((size_t)pow(2, s) * E, sizeof(cache_line));
}

void doEviction(int tag, int set_index, int lineNum){
    int minInd = -1;
    int min = 2147483647;
    int i = 0;
    for(i = 0; i < E; i++){
        int index = (set_index) * E + i;
        if(min > cache[index].time_stamp){
            minInd = index;
            min = cache[index].time_stamp;
        }
    }
    if(minInd > -1){
        cache[minInd].tag = tag;
        cache[minInd].time_stamp = lineNum;
    }
}

void loadData(char * buf, int tag, int set_index, int lineNum, int verbose, int modify){
    int i = 0;
    for(i = 0; i < E; i++){
        int index = (set_index) * E + i;
        if(cache[index].valid == 0){
            break;
        }
        if(cache[index].tag == tag){
            if(modify == 0){
                if(verbose == 1)
                    printf("%s hit\n", buf);
            }else{
                if(verbose == 1)
                    printf("%s hit hit\n", buf);
                hit_count++;
            }
            cache[index].time_stamp = lineNum;
            hit_count++;
            return;
        }
    }
    // 执行到这里说明没有hit 且i<E说明该组还未存满
    miss_count++;
    if(i < E){
        int index = (set_index) * E + i;
        cache[index].valid = 1;
        cache[index].time_stamp = lineNum;
        cache[index].tag = tag;
        if(modify == 0){
            if(verbose == 1)
                printf("%s miss\n", buf);
        }else {
            if(verbose == 1)
                printf("%s miss hit\n", buf);
            hit_count++;
        }

    }else{
        eviction_count++;
        doEviction(tag, set_index, lineNum);
        if(modify == 0) {
            if(verbose == 1)
                printf("%s miss eviction\n", buf);
        }else{
            if(verbose == 1)
                printf("%s miss eviction hit\n", buf);
            hit_count++;
        }

    }
}


void parseData(char *fileName, int verbose){
    printf("%s\n", fileName);
    FILE *fileStream = fopen(fileName, "r");
    char buf[30];
    int lineNum = 0; //lineNum来衡量time Stamp
    while(fgets(buf, 30, fileStream) != NULL){
        lineNum++;
        if(buf[0] == 'I'){
            continue;
        }
        // 从地址中提取tag set_index block_offset
        //int address = atol(buf + 3);
        int address = (int)strtol(buf+3, NULL, 16);
        int tag = address >> (s + b);
        int set_index = (address >> b) & ( (1 << s) - 1);
//        printf("address: %x\n", address);
//        printf("set_index: %x\n", set_index);
//        printf("tag: %x\n", tag);
//      int block_offset = address & ((1 << b) - 1);
        buf[strlen(buf) - 1] = '\0';
        switch(buf[1]){
            case 'L':
                loadData(buf, tag, set_index, lineNum, verbose, 0);
                break;
            case 'S':
                loadData(buf, tag, set_index, lineNum, verbose, 0);
                break;
            case 'M':
                loadData(buf, tag, set_index, lineNum, verbose, 1);
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    extern char *optarg;
    extern int optind, opterr, optopt;

    int verbose = 0;
    char * t = NULL;
    char opt;
    while( (opt = getopt(argc, argv, "hvs:E:b:t:")) != -1 ){
        switch(opt){
            case 'h':
                break;
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = optarg;
                break;
            default:
                break;
        }
    }
    initialCache();
    parseData(t, verbose);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

