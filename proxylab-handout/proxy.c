#include <stdio.h>
#include "csapp.h"
#include <pthread.h>
#include<stdlib.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_CACHE_BLOCK 10

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";


void *thread(void *vargp);

void doit(int connfd);

void parse_uri(char *uri, char *hostname, char *path, int *port);

void build_http_header(char *http_header, char *hostname, char *path, int port, rio_t *client_rio);

int connect_server(char *hostname, int port, char *http_header);

typedef struct {
    char buf[MAX_OBJECT_SIZE];
    char url[MAXLINE];
    int LRU;

    int readCNT;
    sem_t wmutex;     // 保护buf的信号量 初始化为1
    sem_t rdcntMutex; // 保护readCNT的信号量 初始化为1

} cache_block;

typedef struct {
    cache_block cacheobjs[MAX_CACHE_BLOCK];
    int block_num;
} Cache;
Cache cache;

void cache_init() {
    int i = 0;
    cache.block_num = 0;
    for (i = 0; i < MAX_CACHE_BLOCK; i++) {
        Sem_init(&(cache.cacheobjs[i].wmutex), 0, 1);
        Sem_init(&(cache.cacheobjs[i].rdcntMutex), 0, 1);
        cache.cacheobjs[i].LRU = 0;
        cache.cacheobjs[i].readCNT = 0;
    }

}

void readerPrev(cache_block *bk) {
    P(&bk->rdcntMutex);
    bk->readCNT++;
    if (bk->readCNT == 1) /* First in*/
        P(&bk->wmutex);
    V(&bk->rdcntMutex);
}

void readerAfter(cache_block *bk) {
    P(&bk->rdcntMutex);
    bk->readCNT--;
    if (bk->readCNT == 0) /*Last out*/
        V(&bk->wmutex);
    V(&bk->rdcntMutex);
}

void writerPrev(cache_block *bk) {
    P(&bk->wmutex);
}

void writerAfter(cache_block *bk) {
    V(&bk->wmutex);
}

int cache_search(char *url) {
    int i = 0;
    for (i = 0; i < cache.block_num; i++) {
        readerPrev(&(cache.cacheobjs[i]));
        printf("\ncache[%d]: %s", i, cache.cacheobjs[i].url);
        if (strcasecmp(url, cache.cacheobjs[i].url) == 0) {
            cache.cacheobjs[i].LRU++;
            readerAfter(&(cache.cacheobjs[i])); //todo: 返回之前必须要先释放啊！
            return i;
        }
        readerAfter(&(cache.cacheobjs[i]));
    }
    return -1;
}


void cache_conviction(char *url, char *buf) {
    int i = 0;
    int index = -1;
    int min = 9999;
    for (i = 0; i < cache.block_num; i++) {
        readerPrev(&(cache.cacheobjs[i]));
        if (cache.cacheobjs[i].LRU < min) {
            min = cache.cacheobjs[i].LRU;
            index = i;
        }
        readerAfter(&(cache.cacheobjs[i]));
    }
    if (index < 0) index = 0;
    // 更新第i个的内容
    writerPrev(&(cache.cacheobjs[i]));
    strncpy(cache.cacheobjs[i].url, url, strlen(url));
    cache.cacheobjs[i].url[strlen(url)] = '\0';
    strncpy(cache.cacheobjs[i].buf, buf, strlen(buf));
    cache.cacheobjs[i].buf[strlen(buf)] = '\0';
    cache.cacheobjs[i].LRU = 0;
    cache.cacheobjs[i].readCNT = 0;
    writerAfter(&(cache.cacheobjs[i]));
}

void cache_insert(char *url, char *buf) {
    if (strlen(url) > MAXLINE || strlen(buf) > MAX_OBJECT_SIZE) {
        return;
    }
    if (cache.block_num == MAX_CACHE_BLOCK) {
        cache_conviction(url, buf);
    } else {
        int i = cache.block_num;
        cache.block_num++;
        writerPrev(&(cache.cacheobjs[i]));
        strncpy(cache.cacheobjs[i].url, url, strlen(url));
        cache.cacheobjs[i].url[strlen(url)] = '\0';
        strncpy(cache.cacheobjs[i].buf, buf, strlen(buf));
        cache.cacheobjs[i].buf[strlen(buf)] = '\0';
        cache.cacheobjs[i].LRU = 0;
        cache.cacheobjs[i].readCNT = 0;
        writerAfter(&(cache.cacheobjs[i]));
    }
}

int main(int argc, char **argv) {
    cache_init();
    int listenfd, *connfd;
    socklen_t clientlen;
    pthread_t tid;

    struct sockaddr_storage clientaddr;/*generic sockaddr struct which is 28 Bytes.The same use as sockaddr*/

    if (argc != 2) {
        fprintf(stderr, "usage :%s <port> \n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);

        connfd = Malloc(sizeof(int));
        *connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, connfd);
    }
}

void *thread(void *vargp) {
    int connfd = *((int *) vargp);
    // 确保在执行完毕后会自动回收
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}

/*handle the client HTTP transaction*/
void doit(int connfd) {
    int serverfd;/*the end server file descriptor*/

    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char server_http_header[MAXLINE];
    /*store the request line arguments*/
    char hostname[MAXLINE], path[MAXLINE];
    int port;

    rio_t rio, server_rio;/*rio is client's rio,server_rio is endserver's rio*/

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version); /*read the client request line*/

    if (strcasecmp(method, "GET")) {
        printf("Proxy does not implement the method");
        return;
    }

    int ind; // TODO 如果cache中缓存过这个uri，则直接返回结果
    if ((ind = cache_search(uri)) != -1) {
        readerPrev(&(cache.cacheobjs[ind]));
        Rio_writen(connfd, cache.cacheobjs[ind].buf, strlen(cache.cacheobjs[ind].buf));
        readerAfter(&(cache.cacheobjs[ind]));
        writerPrev(&(cache.cacheobjs[ind]));
        cache.cacheobjs[ind].LRU++;
        writerAfter(&(cache.cacheobjs[ind]));
        return;
    }

    char uri_store[100];
    strcpy(uri_store, uri);
    parse_uri(uri, hostname, path, &port);

    build_http_header(server_http_header, hostname, path, port, &rio);

    char portStr[100];
    sprintf(portStr, "%d", port);
    serverfd = Open_clientfd(hostname, portStr);
    if (serverfd < 0) {
        printf("connection failed\n");
        return;
    }

    Rio_readinitb(&server_rio, serverfd);
    Rio_writen(serverfd, server_http_header, strlen(server_http_header));

    size_t n;
    char whole[MAX_OBJECT_SIZE];
    int size_whole = 0;
    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
        printf("proxy received %d bytes,then send\n", (int) n);
        Rio_writen(connfd, buf, n);
        size_whole += n;
        if (size_whole < MAX_OBJECT_SIZE)
            strcat(whole, buf);
    }
    cache_insert(uri_store, whole); // 将结果插入到cache中
    Close(serverfd);
}

void build_http_header(char *http_header, char *hostname, char *path, int port, rio_t *client_rio) {
    static const char *conn_hdr = "Connection: close\r\n";
    static const char *prox_hdr = "Proxy-Connection: close\r\n";
    static const char *host_hdr_format = "Host: %s\r\n";
    static const char *request_hdr_format = "GET %s HTTP/1.0\r\n";
    static const char *endof_hdr = "\r\n";

    static const char *connection_key = "Connection";
    static const char *user_agent_key = "User-Agent";
    static const char *proxy_connection_key = "Proxy-Connection";
    static const char *host_key = "Host";

    char buf[MAXLINE], request_hdr[MAXLINE], other_hdr[MAXLINE], host_hdr[MAXLINE];
    /*request line*/
    sprintf(request_hdr, request_hdr_format, path);
    /*get other request header for client rio and change it */
    while (Rio_readlineb(client_rio, buf, MAXLINE) > 0) {
        if (strcmp(buf, endof_hdr) == 0) break;/*EOF*/

        if (!strncasecmp(buf, host_key, strlen(host_key)))/*Host:*/
        {
            strcpy(host_hdr, buf);
            continue;
        }

        if (!strncasecmp(buf, connection_key, strlen(connection_key))
            && !strncasecmp(buf, proxy_connection_key, strlen(proxy_connection_key))
            && !strncasecmp(buf, user_agent_key, strlen(user_agent_key))) {
            strcat(other_hdr, buf);
        }
    }
    if (strlen(host_hdr) == 0) {
        sprintf(host_hdr, host_hdr_format, hostname);
    }
    sprintf(http_header, "%s%s%s%s%s%s%s",
            request_hdr,
            host_hdr,
            conn_hdr,
            prox_hdr,
            user_agent_hdr,
            other_hdr,
            endof_hdr);

    return;
}

/*parse the uri to get hostname,file path ,port*/
void parse_uri(char *uri, char *hostname, char *path, int *port) {
    *port = 80;
    char *pos = strstr(uri, "//");

    pos = pos != NULL ? pos + 2 : uri;

    char *pos2 = strstr(pos, ":");
    if (pos2 != NULL) {
        *pos2 = '\0';
        sscanf(pos, "%s", hostname);
        sscanf(pos2 + 1, "%d%s", port, path);
    } else {
        pos2 = strstr(pos, "/");
        if (pos2 != NULL) {
            *pos2 = '\0';
            sscanf(pos, "%s", hostname);
            *pos2 = '/';
            sscanf(pos2, "%s", path);
        } else {
            sscanf(pos, "%s", hostname);
        }
    }
    return;
}















