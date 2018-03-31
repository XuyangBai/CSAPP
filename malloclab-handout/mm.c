/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
        /* Team name */
        "ateam",
        /* First member's full name */
        "Harry Bovik",
        /* First member's email address */
        "bovik@cs.cmu.edu",
        /* Second member's full name (leave blank if none) */
        "",
        /* Second member's email address (leave blank if none) */
        ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE     4
#define DSIZE     8
#define CHUNKSIZE (1 << 12)


#define MAX(x, y) ((x) > (y) ? (x) : (y))
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 用size和alloc标记 构造块的头部 */
#define PACK(size, alloc) ((size) | (alloc))

/* read and write a word at address p */
#define GET(p)      (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p*/
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer*/
#define HDRP(bp)    ((char *)(bp) - WSIZE)
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks*/
#define NEXT_BLKP(bp) ( (char *)(bp) + GET_SIZE((char *)(bp) - WSIZE) )
#define PREV_BLKP(bp) ( (char *)(bp) - GET_SIZE((char *)(bp) - DSIZE) )

static void *extend_heap(size_t words);

static void *coalesce(void *bp);

static void *find_fit(size_t size);

static void place(void *bp, size_t asize);

static char *heap_head; // 指向序言块

static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); // 前一个块是否分配
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); // 后一个块是否分配
    if (prev_alloc == 0 && next_alloc == 0) {
        void *prev_bp = PREV_BLKP(bp);
        void *next_bp = NEXT_BLKP(bp);
        size_t size = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(prev_bp), PACK(size, 0));
        PUT(FTRP(next_bp), PACK(size, 0));
        return prev_bp;
    } else if (prev_alloc == 0 && next_alloc != 0) {
        void *prev_bp = PREV_BLKP(bp);
        size_t size = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(prev_bp), PACK(size, 0)); // 前一个块的head
        PUT(FTRP(bp), PACK(size, 0));      // 当前块的footer
        return prev_bp;
    } else if (prev_alloc != 0 && next_alloc == 0) {
        void *next_bp = NEXT_BLKP(bp);
        size_t size = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));       // 当前块的head
        PUT(FTRP(next_bp), PACK(size, 0));  // 下一个块footer
        return bp;
    } else {
        return bp;
    }
}

static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long) (bp = mem_sbrk(size)) == (void *) -1) { // TODO: 为什么要转化为long呢？？？
        return NULL;
    }

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // coalesce if the previous block was free
    return coalesce(bp);
}

// 首次适配搜索
static void *find_fit(size_t size) {
    void *bp;
    for (bp = heap_head; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (size <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL;
}

// 将请求块放在空闲块的起始位置
static void place(void *bp, size_t asize) {
    size_t origin_size = GET_SIZE(HDRP(bp));
    if (origin_size - asize < 2 * DSIZE) { // 剩余部分大小 小于最小块的大小了
        PUT(HDRP(bp), PACK(origin_size, 1));
        PUT(FTRP(bp), PACK(origin_size, 1));
    } else {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(origin_size - asize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(origin_size - asize, 0));
    }
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    if ((heap_head = mem_sbrk(4 * WSIZE)) == (void *) -1) { // 创建序言块、结尾块
        return -1;
    }
    PUT(heap_head, 0);
    PUT(heap_head + (1 * WSIZE), PACK(DSIZE, 1)); // prologue header
    PUT(heap_head + (2 * WSIZE), PACK(DSIZE, 1)); // prologue footer
    PUT(heap_head + (3 * WSIZE), PACK(0, 1));     // Epilogue header
    heap_head += (2 * WSIZE);

    /* Extend the empty heap with a free block of CHUNKSIZE bytes*/
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
        return -1;
    }
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
//    size_t newsize = ALIGN(size + SIZE_T_SIZE);
    size_t newsize;
    size_t extendsize;
    char *bp;

    if (size == 0) return NULL;

    if (size <= DSIZE) {
        newsize = 2 * DSIZE;
    } else {
        newsize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    }
    // search the free list for a fit 寻找一个合适的空闲快
    if ((bp = find_fit(newsize)) != NULL) {
        place(bp, newsize);
        return bp;
    }

    // no fit found. Get more memory and place the block
    extendsize = MAX(newsize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL) {
        return NULL;
    }
    place(bp, newsize);
    return bp;

}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
    if (ptr == 0) {
        return;
    }
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    // TODO: 在free里合并就是立即合并， 也可以放到malloc中，当找不到合适大小的空闲块的时候执行一次合并
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    void *newptr;
    size_t oldsize;

    if (ptr == NULL) {
        return mm_malloc(size);
    }
    if (size == 0) {
        mm_free(ptr);
        return 0;
    }

    newptr = mm_malloc(size);
    if (!newptr) {
        return 0;
    }
    // copy the old data
    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    //free old block
    mm_free(ptr);

    return newptr;
}
