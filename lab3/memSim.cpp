#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>

using namespace std;

/**
 * @brief
 * page = frame = block = 256
*/
struct frameSize{
    char element[256];
};

struct pageStruct{
    uint32_t frameNumber;
    uint8_t present : 1;    //a present bit for keeping track of the validation
};

struct TLBStruct{
    uint32_t pageNumber;
    uint32_t frameNumber;
};

struct virtualMemInfo{
    uint numTransAddress;
    uint pageFault;
    float pageFaultRate;
    uint tlbHits;
    uint tlbMisses;
    float tlbHitRate;
};

//TLB of size 16 entries
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
int TLB_Check(uint32_t pageIndexNumber){
    int indexNum = -1;
    for(u_int i=0; i<TLB.size();i++){
        indexNum = (TLB[i].pageNumber == pageIndexNumber) ? i : -1;
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
    memInfo.pageFaultRate = (memInfo.pageFault/memInfo.numTransAddress);
    memInfo.tlbHitRate = (memInfo.tlbHits/memInfo.numTransAddress);
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
                if(TLB.size() == 16){
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
                if(TLB.size() == 16){
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
                printf("physical mem is full waiting for response\n");
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
    uint phyEntrySize = 10;

    //update entries based on the file input
    updateEntryInfo();
    memInfo.numTransAddress = reference_sequence.size();
    performFetching(phyEntrySize);

    return 0;
}