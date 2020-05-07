#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGES 256
#define FRAMES 128
#define FRAMESIZE 256
#define PAGESIZE 256
#define MAX_TLB 16

#define PHYSICAL_MEMSIZE FRAMES * FRAMESIZE
#define BACKING_MEMSIZE PAGES * PAGESIZE

int pagetable[PAGES];
char memory[PHYSICAL_MEMSIZE];
char tempMemory[FRAMESIZE];
int memCounter[FRAMES];
int frame_index = 0;
int pagefaultNum;
int tlbhitNum;

//structure for tlb entry
struct tlb {
  int logical;
  int physical;
};

//tlb table creation
struct tlb tlbtable[MAX_TLB];
int tlb_index = 0;

/* search tlb table to find an entry with logical page that equals to passed
 variable logical and returns the physical page of the same entry */
int findTlb(int logical){
  int i;
  for (i = 0; i < MAX_TLB; i++) {
    if (tlbtable[i].logical == logical) {
      return tlbtable[i].physical;
    }
  }
  return -1;
}

//insert an logical page and physical page as an tlb entry into the tlb table
void insertTlb(int logical, int physical){
  struct tlb entry = {logical, physical};
  tlbtable[tlb_index % MAX_TLB] = entry;
  tlb_index++;
}

//increments clock counter of each memory frames each time a page is referenced
void increCounter(int ref){
  for (int i = 0; i < FRAMES; i++) {
    if (memCounter[i] != -1) {
      memCounter[i]++;
    }
  }
  memCounter[ref] = 1;
}

//finds and returns index of the least recently used frame
int findLeast(){
  int index = 0;
  for (int i = 0; i < FRAMES - 1; i++) {
    if (memCounter[i] < memCounter[i + 1]) {
      index = i + 1;
    }
  }
  return index;
}

int main(int argc, char const *argv[]) {
  for (int i = 0; i < PAGES; i++) { pagetable[i] = -1; }
  for (int i = 0; i < FRAMES; i++) { memCounter[i] = -1; }

  int page_num, offset, frame_num;
  const char * backing_store = argv[1];
  const char * input = argv[2];
  FILE * input_file;
  FILE * output_file;
  input_file = fopen(input, "r");
  output_file = fopen("output.csv", "w");
  char buf[10];
  int fd = open(backing_store, O_RDWR);
  char * back = mmap(0, BACKING_MEMSIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  while (fgets(buf, 10, input_file) != NULL) {
    int entry = atoi(buf);
    page_num = entry >> 8;
    offset = entry & 255;
    frame_num = findTlb(page_num);
    if (frame_num != -1) {
      //tlb hit
      tlbhitNum++;
    } else {
      //tlb miss
      frame_num = pagetable[page_num];
      if (frame_num == -1) {
        //page fault
        if (frame_index < FRAMES) {
          //free frame exist
          memcpy(memory + frame_index * FRAMESIZE, back + page_num * PAGESIZE, PAGESIZE);
          frame_num = frame_index;
          pagetable[page_num] = frame_num;
          increCounter(frame_num);
          pagefaultNum++;
          frame_index++;
          insertTlb(page_num, frame_num);
        } else {
          //no free memory
          //LRU swap
          int least = findLeast();
          memcpy(tempMemory, memory + least * FRAMESIZE, FRAMESIZE);
          memcpy(memory + least * FRAMESIZE, back + page_num * PAGESIZE, PAGESIZE);
          memcpy(back + page_num * PAGESIZE, tempMemory, FRAMESIZE);
          frame_num = least;
          pagetable[page_num] = frame_num;
          pagefaultNum++;
          increCounter(least);
          insertTlb(page_num, frame_num);
        }
      }
      //insertTlb(page_num, frame_num);
    }
    int physical = (frame_num << 8) | offset;
    signed char value = memory[frame_num * FRAMESIZE + offset];
    //writes the each line of output into the output.csv file
    fprintf(output_file, "%d,%d,%d\n", entry, physical, value);
  }
  //statistics
  printf("Page Faults Rate = %0.1f%%\n", pagefaultNum / 10.0);
  printf("TLB Hits Rate = %0.1f%%\n", tlbhitNum / 10.0);
  return 0;
}
