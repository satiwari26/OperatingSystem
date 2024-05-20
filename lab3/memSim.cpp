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
    uint8_t element[256];
};

struct pageStruct{
    frameSize page;
    uint8_t present : 1;    //a present bit for keeping track of the validation
};

struct TLBStruct{
    uint32_t pageNumber;
    uint32_t frameNumber;
};

//TLB of size 16 entries
vector<TLBStruct> TLB(16);

//Page table 2^8(256) entries
vector<pageStruct> pageTable(256);

//physical memory table - entries dynamically allocated
vector<frameSize>physMem; 

// backing-store contains stores bin data for memory - (addressable by 256 blocks)
vector<frameSize> backingStore;

//reference-sequence: logical address requests
vector<uint32_t> reference_sequence;

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

    // Output the number of frames read
    std::cout << "Number of frames read in the reference sequence: " << backingStore.size() << std::endl;

    return;
}

/**
 * @brief
 * performs the check in the TBL quickly and returns valid index
 * returns -1 if valid entry not found
*/
int TLB_Check(uint32_t pageIndexNumber){
    int indexNum;
    for(int i=0; i<TLB.size();i++){
        indexNum = (TLB[i].pageNumber == pageIndexNumber) ? i : -1;
    }
    return indexNum;
}

void printFrameContent(const frameSize& frame){
    for (int i = 0; i < sizeof(frame.element); ++i) {
        printf("%c", frame.element[i]);
    }
    printf("\n");
}


/**
 * @brief
 * performs the fetching of the instruction for reference_sequence
*/
void performFetching(){

    for(int i=0; i<reference_sequence.size();i++){
        uint32_t pageIndexNumber = (reference_sequence[i] >> 8) & 0xFF;
        uint32_t offsetPosition = (reference_sequence[i]) & 0xFF;
        int tlbIndex = TLB_Check(pageIndexNumber);
        //if tlb hit
        if(tlbIndex > 0){
            printf("%d, %d, %d,",pageIndexNumber,  pageTable[TLB[tlbIndex].pageNumber].page.element[offsetPosition], TLB[tlbIndex].frameNumber);
            printFrameContent(pageTable[TLB[tlbIndex].pageNumber].page);
        }
        //if tlb miss && table miss
        else{
            for(int i = 0; i < backingStore.size(); i++){

            }
        }
    }

}

int main(){

    //invalidate the page table for the first time
    for(int i=0; i<pageTable.size();i++){
        pageTable[i].present = 0;
    }

    //temp size for the physical memory
    int phyEntrySize = 10;
    physMem.resize(phyEntrySize);

    //update entries based on the file input
    updateEntryInfo();
    performFetching();

    return 0;
}