#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <iomanip>

#define PAGETABLESIZE 256
#define NPROCESSES 256

int frameTableSize = 2048; //default = 256
int pageMisses = 0;
int pageHits = 0;

typedef struct {
    //limited memory
    int frame{-1};
    int pageHits{0};
    int pageMisses{0};

    //unlimited memory
    int infFrame{-1};
    int infPageHits{0};
    int infPageMisses{0};
} PageTableEntry;

typedef struct {
    uint8_t pid{0};
    uint8_t page{0};
    bool vacant{true};
} FrameTableEntry;

typedef struct {
    uint8_t pid;
    uint8_t page;
} MemoryAccess;

int main(int argc, char *args[]) {

    std::cout<<"Jake TerHark"<<std::endl
            <<"jterha2"<<std::endl
            << "Memory Paging Simulation";

    char *filename;
    int fd,
            limit = 0,
            currentFrame = 0;
    size_t fileSize;
    struct stat stats{};
    MemoryAccess *accesses;
    PageTableEntry tables[PAGETABLESIZE][NPROCESSES];

    if (argc < 2) {
        std::cerr << "Incorrect number of command line arguments" << std::endl;
        exit(-1);
    }

    //parse command line arguments
    filename = args[1];

    for (int i = 2; i < argc; i += 2) {
        if (strcmp(args[i], "--limit") == 0) {
            if ((i + 1) >= argc) {
                std::cerr << "Incorrect usage of '--limit'!" << std::endl;
                exit(-2);
            } else {
                limit = atoi(args[i + 1]);
            }

        } else if (strcmp(args[i], "--table") == 0) {
            if ((i + 1) >= argc) {
                std::cerr << "Incorrect usage of '--table'!" << std::endl;
                exit(-3);
            } else {
                frameTableSize = atoi(args[i + 1]);
            }
        }
    }

    FrameTableEntry frameTable[frameTableSize];

    //open file
    if ((fd=open(filename, O_RDONLY)) < 0) {
        perror("Cannot open file");
        exit(-1);
    }

    //get file stats
    if (fstat(fd, &stats) < 0) {
        perror("Cannot read file stats");
        exit(-2);
    }

    fileSize = (size_t) stats.st_size;

    int max = static_cast<int>(fileSize) / sizeof(MemoryAccess);

    //if user specified 0 for limit or if it wasn't provided
    if (limit == 0) {
        limit = max;
    }

    accesses = (MemoryAccess *) mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);

    if (accesses == MAP_FAILED) {
        perror("Map failed");
        exit(-3);
    }

    for (int i = 0; i < limit; ++i) {
        //get pid and page
        auto pid = accesses[i].pid;
        auto page = accesses[i].page;

        //limited memory
        if (tables[pid][page].frame != -1) {
            tables[pid][page].pageHits += 1;
            ++pageHits;
        } else {
            int index = currentFrame % frameTableSize; //index of the frame table that will hold the page

            auto temp = frameTable[index];
            auto boolean = temp.vacant;

            //select victim frame if not vacant
            if (!(boolean)) {
                tables[frameTable[index].pid][frameTable[index].page].frame = -1;
                frameTable[index].vacant = true;
            }

            //fill page table entry
            tables[pid][page].pageMisses += 1;
            tables[pid][page].frame = index;

            //fill frame table entry
            frameTable[index].vacant = false;
            frameTable[index].pid = pid;
            frameTable[index].page = page;

            pageMisses += 1;

            //update the frame index for next miss
            currentFrame += 1;
        }


        //unlimited simulation
        if (tables[pid][page].infFrame == -1) {
            tables[pid][page].infPageMisses += 1;
            tables[pid][page].infFrame = 1;
        } else {
            tables[pid][page].infPageHits += 1;
        }

    }

    //get statistics
    int infPageHits = 0;
    int infPageMisses = 0;

    for (auto &table : tables) {
        for (auto entry : table) {
            infPageHits += entry.infPageHits;
            infPageMisses += entry.infPageMisses;
        }
    }

    //free memory
    munmap(accesses, fileSize);
    close(fd);

    //output results
    std::cout << "***" << filename << "***" << std::endl
              << "          Filesize: " << fileSize <<std::endl
              << "    Total Accesses: " << max << std::endl
              << "Accesses Processed: " << limit << std::endl << std::endl;

    std::cout << "***Infinite Memory***" << std::endl
              << "  Page Hits: " << infPageHits << std::endl
              << "Page Misses: " << infPageMisses << std::endl << std::endl;

    std::cout << "***Frame Table***" << std::endl
              << "Frame Table: " << frameTableSize << std::endl
              << "  Page Hits: " << pageHits << std::endl
              << "Page Misses: " << pageMisses << std::endl;

    return 0;
}