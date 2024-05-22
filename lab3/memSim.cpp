#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string>

using namespace std;

#define TLB_SIZE 16

/**
 * @brief
 * page = frame = block = 256
*/
struct frameSize{
    char element[256];
};

struct pageStruct{
    int frameNumber;
    uint8_t present : 1;    //a present bit for keeping track of the validation
    pageStruct(){
        this->frameNumber = -1;
        this->present = 0;
    }
};

/**
 * @brief
 * algorithm to be used
*/
enum evictAlgorithm{FIFO, LRU, OPT};

//default frame evict algorithm
evictAlgorithm algorithm = FIFO;

struct TLBStruct{
    int pageNumber;
    int frameNumber;
    TLBStruct(){
        this->frameNumber = -1;
        this->pageNumber = -1;
    }
};

struct virtualMemInfo{
    uint numTransAddress;
    uint pageFault;
    float pageFaultRate;
    uint tlbHits;
    uint tlbMisses;
    float tlbHitRate;
};

//TLB of size TLB_SIZE entries
vector<TLBStruct> TLB;

//Page table 2^8(256) entries
vector<pageStruct> pageTable(256);

//physical memory table - entries dynamically allocated
vector<frameSize>physMem; 

// backing-store contains stores bin data for memory - (addressable by 256 blocks)
vector<frameSize> backingStore;

//reference-sequence: logical address requests
vector<uint32_t> reference_sequence;

//virtual Mem info
virtualMemInfo memInfo;

/**
 * @brief
 * parses the users command based on their input
*/
void parseCommands(){
    return;
}

/**
 * @brief
 * read the ref instruction from the file
 * read the binary backing store information in the 
*/
void updateEntryInfo(){
    FILE * ref_seq_file =  fopen("./fifo1.txt", "r");
    if (ref_seq_file == NULL) {
        printf("Failed to open the file.\n");
        exit(0);
    }

    // Read each number from the file
    uint32_t entry;
    while (fscanf(ref_seq_file, "%d", &entry) == 1) {
        reference_sequence.push_back(entry);
    }
    // Close the file
    fclose(ref_seq_file);

    // Open the binary file for reading
    FILE *file = fopen("./BACKING_STORE.bin", "rb");
    if (!file) {
        std::cerr << "Failed to open the file." << std::endl;
        exit(0);
    }

    // Read the binary data from the file and store it in the vector
    frameSize frame;
    size_t bytesRead;
    while ((bytesRead = fread(&frame, sizeof(frame), 1, file)) > 0) {
        backingStore.push_back(frame);
    }

    // Close the file
    fclose(file);

    return;
}

/**
 * @brief
 * performs the check in the TBL quickly and returns valid index
 * returns -1 if valid entry not found
*/
int TLB_Check(int pageIndexNumber){
    int indexNum = -1;
    for(u_int i=0; i<TLB.size();i++){
        indexNum = (TLB[i].pageNumber == pageIndexNumber) ? i : -1;
        if(indexNum > -1){
            return indexNum;
        }
    }
    return indexNum;
}

/**
 * @brief
 * prints the frame content information
*/
void printFrameContent(const frameSize& frame){
    for (u_int i = 0; i < sizeof(frame); ++i) {
        printf("%02X", (unsigned char)frame.element[i]);
    }
    printf("\n");
}

/**
 * @brief
 * calculates the virtual mem info
*/
void calculateVirtualMemInfo(){
    memInfo.pageFaultRate = ((float)memInfo.pageFault/memInfo.numTransAddress);
    memInfo.tlbHitRate = ((float)memInfo.tlbHits/memInfo.numTransAddress);
}

/**
 * @brief
 * prints virtual mem info
*/
void printVirtualMemInfo(){
    printf("Number of Translated Addresses = %d\n", memInfo.numTransAddress);
    printf("Page Faults = %d\n", memInfo.pageFault);
    printf("Page Fault Rate = %.3f\n", memInfo.pageFaultRate);
    printf("TLB Hits = %d\n", memInfo.tlbHits);
    printf("TLB Misses = %d\n", memInfo.tlbMisses);
    printf("TLB Hit Rate = %.3f\n", memInfo.tlbHitRate);
}

/**
 * @brief
 * performs eviction based on the fifo algorithm
*/
void FIFO_eviction(frameSize frame, int pageIndexNumber){
    //invalidate the entry in the page table
    for(uint i=0; i < pageTable.size(); i++){
        if(pageTable[i].frameNumber == 0){  //0 frame b/c of FIFO
            pageTable[i].present = 0;
            //also remove that element from the tlb if it exists
            for(uint j=0; j < TLB.size(); j++){
                if(TLB[j].pageNumber == (int)i){
                    TLB.erase(TLB.begin() + j);
                    break;
                }
            }
            break;
        }
    }

    //add the element to the physical memory and reupdate the page table and tlb
    physMem.push_back(frame);
    pageTable[pageIndexNumber].frameNumber = (physMem.size() - 1);
    pageTable[pageIndexNumber].present = 1;

    //add the entry in the tlb
    TLBStruct mapPage;
    mapPage.frameNumber = pageTable[pageIndexNumber].frameNumber;
    mapPage.pageNumber = pageIndexNumber;
    if(TLB.size() == TLB_SIZE){
        TLB.erase(TLB.begin()); //remove the front most element
        //add the mapPage to the tlb
        TLB.push_back(mapPage);
    }
    //tbl has space add the element at the end of it
    else{
        TLB.push_back(mapPage);
    }
}

/**
 * @brief
 * performs the fetching of the instruction for reference_sequence
*/
void performFetching(uint phyEntrySize){

    for(u_int i=0; i<reference_sequence.size();i++){
        uint32_t pageIndexNumber = (reference_sequence[i] >> 8) & 0xFF;
        uint32_t offsetPosition = (reference_sequence[i]) & 0xFF;
        int tlbIndex = TLB_Check(pageIndexNumber);
        int pageTableMiss = 0;
        //if tlb hit
        if(tlbIndex >= 0){
            memInfo.tlbHits += 1;
            printf("%d, %d, %d, ",reference_sequence[i], physMem[TLB[tlbIndex].frameNumber].element[offsetPosition], TLB[tlbIndex].frameNumber);
            printFrameContent(physMem[TLB[tlbIndex].frameNumber]);
        }
        //if tlb miss, check page table for valid entry
        else{
            memInfo.tlbMisses += 1;
            pageTableMiss = pageTable[pageIndexNumber].present ? 1 : -1;
            if(pageTable[pageIndexNumber].present){ //if hit on the page table
                //create a temp tbl instance for storage
                TLBStruct mapPage;
                mapPage.frameNumber = pageTable[pageIndexNumber].frameNumber;
                mapPage.pageNumber = pageIndexNumber;
                //tlb size is full - update (FIFO)
                if(TLB.size() == TLB_SIZE){
                    TLB.erase(TLB.begin()); //remove the front most element
                    //add the mapPage to the tlb
                    TLB.push_back(mapPage);
                }
                //tbl has space add the element at the end of it
                else{
                    TLB.push_back(mapPage);
                }

                printf("%d, %d, %d, ",reference_sequence[i], physMem[pageTable[pageIndexNumber].frameNumber].element[offsetPosition], pageTable[pageIndexNumber].frameNumber);
                printFrameContent(physMem[pageTable[pageIndexNumber].frameNumber]);
            }
        }

        //if hard Miss(page table miss)
        if(pageTableMiss == -1){
            memInfo.pageFault += 1;
            //if there is a space in the phys mem add the element from the backing store
            if(physMem.size() < phyEntrySize){
                physMem.push_back(backingStore[pageIndexNumber]);
                //update the page table, based on the entry just added to the physical mem
                pageTable[pageIndexNumber].frameNumber = (physMem.size() - 1);
                pageTable[pageIndexNumber].present = 1;

                //create a temp tbl instance for storage
                TLBStruct mapPage;
                mapPage.frameNumber = pageTable[pageIndexNumber].frameNumber;
                mapPage.pageNumber = pageIndexNumber;
                //tlb size is full - update (FIFO)
                if(TLB.size() == TLB_SIZE){
                    TLB.erase(TLB.begin()); //remove the front most element
                    //add the mapPage to the tlb
                    TLB.push_back(mapPage);
                }
                //tbl has space add the element at the end of it
                else{
                    TLB.push_back(mapPage);
                }

                printf("%d, %d, %d, ",reference_sequence[i], physMem[pageTable[pageIndexNumber].frameNumber].element[offsetPosition], pageTable[pageIndexNumber].frameNumber);
                printFrameContent(physMem[pageTable[pageIndexNumber].frameNumber]);
            }
            //physical memory is full - perform eviction based on the algorithm
            else{
                if(algorithm == FIFO){
                    FIFO_eviction(backingStore[pageIndexNumber], pageIndexNumber);
                }
            }
        }
    }

    // calculate the virtual Mem Info
    calculateVirtualMemInfo();
    printVirtualMemInfo();
}

int main(){

    //initialize virtualMemInfo
    memInfo.numTransAddress = 0;
    memInfo.pageFault = 0;
    memInfo.pageFaultRate = 0;
    memInfo.tlbHitRate = 0;
    memInfo.tlbHits = 0;
    memInfo.tlbMisses = 0;

    //invalidate the page table for the first time
    for(u_int i=0; i<pageTable.size();i++){
        pageTable[i].present = 0;
    }

    //temp size for the physical memory
    uint phyEntrySize = 5;

    //update entries based on the file input
    updateEntryInfo();
    memInfo.numTransAddress = reference_sequence.size();
    performFetching(phyEntrySize);

    return 0;
}