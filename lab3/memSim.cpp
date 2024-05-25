#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string.h>

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

struct OPT_struct{
    int pageNumber;
    int frameNumber;
    int indexNumber;
    int prevIndexNumber;
    OPT_struct(){
        this->frameNumber = -1;
        this->pageNumber = -1;
        this->indexNumber = -1;
        this->prevIndexNumber = -1;
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

//LRU data-structure
vector<TLBStruct> LRU_Record;

//OPT data-structure
vector<OPT_struct> OPT_Record;

//FIFO data-struct
vector<TLBStruct> FIFO_record;

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
 * read the ref instruction from the file
 * read the binary backing store information in the 
*/
void updateEntryInfo(const char * reference_sequence_file){
    FILE * ref_seq_file =  fopen(reference_sequence_file, "r");
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
        if((pageTable[i].frameNumber == FIFO_record[0].frameNumber) && (FIFO_record[0].pageNumber == (int)i)){  //0 frame b/c of FIFO
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
    physMem[FIFO_record[0].frameNumber] = frame;
    pageTable[pageIndexNumber].frameNumber = (FIFO_record[0].frameNumber);
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

    //update the FIFO_record
    FIFO_record.erase(FIFO_record.begin());
    FIFO_record.push_back(mapPage);
}

/**
 * @brief
 * data struct that keeps track of addresses and updates it based on the frequency
*/
void LRU_rec(int frameNumber, int pageNumber){
    //check if the pageNumber exist on the list - remove it from that position
    for(uint i=0; i < LRU_Record.size(); i++){
        if(LRU_Record[i].pageNumber == pageNumber){
            LRU_Record.erase(LRU_Record.begin() + i);
        }
    }

    //pushing it on top of the list
    TLBStruct tempTLB;
    tempTLB.frameNumber = frameNumber;
    tempTLB.pageNumber = pageNumber;
    LRU_Record.push_back(tempTLB);
}

/**
 * @brief
 * data struct that keeps track of addresses and updates it based on FIFO
*/
void FIFO_rec(int frameNumber, int pageNumber){
    //check if the pageNumber exist on the list - if it does don't add it
    for(uint i=0; i < FIFO_record.size(); i++){
        if(FIFO_record[i].pageNumber == pageNumber){
            return;
        }
    }

    //otherwise add it to the list
    TLBStruct tempTLB;
    tempTLB.frameNumber = frameNumber;
    tempTLB.pageNumber = pageNumber;
    FIFO_record.push_back(tempTLB);
}

/**
 * @brief
 * LRU eviction algorithm
*/
void LRU_eviction(frameSize frame, int pageIndexNumber){
    //remove the element based on the LRU__rec
    pageTable[LRU_Record[0].pageNumber].present = 0;    //invalidate the page table based on LRU
    //update the TLB
    for(uint i =0; i < TLB.size(); i++){
        if(LRU_Record[0].pageNumber == TLB[i].pageNumber){
            TLB.erase(TLB.begin() + i);
            break;
        }
    }

    //add the frame to the physical memory
    physMem[LRU_Record[0].frameNumber] = frame;
    //update the pageTable based on the pageIndexNumber
    pageTable[pageIndexNumber].present = 1;
    pageTable[pageIndexNumber].frameNumber = LRU_Record[0].frameNumber;
    //add the element to the TLB
    TLBStruct tempTLB;
    tempTLB.frameNumber = LRU_Record[0].frameNumber;
    tempTLB.pageNumber = pageIndexNumber;
    TLB.push_back(tempTLB);
    
    //update the LRU_record based on this request
    LRU_Record.erase(LRU_Record.begin());
    LRU_Record.push_back(tempTLB);
    
}

/**
 * @brief
 * add element to the OPT struct until the frame array is fill
*/
void OPT_update(int frameNumber, int pageIndexNumber, int addressIndex){
    OPT_struct tempStruct;
    tempStruct.frameNumber = frameNumber;
    tempStruct.indexNumber = addressIndex;
    tempStruct.prevIndexNumber = addressIndex;
    tempStruct.pageNumber = pageIndexNumber;

    OPT_Record.push_back(tempStruct);
}

/**
 * @brief
 * OPT replacement based on the 
*/
void OPT_eviction(frameSize frame, int pageIndexNumber, int currentAddressIndex){
    //for each instruction currently in frame check their next indexes
    for(uint i = 0; i < OPT_Record.size(); i++){
        for(uint j = currentAddressIndex; j < reference_sequence.size(); j++){
            int tempPageNumber = (reference_sequence[j] >> 8) & 0xFF;
            if(OPT_Record[i].pageNumber == tempPageNumber){
                OPT_Record[i].indexNumber = j;
                break;
            }
        }
    }

    //performing the eviction
    for(uint i=0; i< OPT_Record.size(); i++){
        if(OPT_Record[i].indexNumber < currentAddressIndex){    //no future address evict this instantly
            pageTable[OPT_Record[i].pageNumber].present = 0;
            physMem[OPT_Record[i].frameNumber] = frame;
            pageTable[pageIndexNumber].present = 1;
            pageTable[pageIndexNumber].frameNumber = OPT_Record[i].frameNumber;

            //update the tlb
            for(uint t = 0; t < TLB.size(); t++){
                if(TLB[t].pageNumber == OPT_Record[i].pageNumber){
                    TLB.erase(TLB.begin() + t);
                    break;
                }
            }
            TLBStruct mapPage;
            mapPage.frameNumber = pageTable[pageIndexNumber].frameNumber;
            mapPage.pageNumber = pageIndexNumber;
            TLB.push_back(mapPage);

            OPT_struct tempOPT;
            tempOPT.pageNumber = pageIndexNumber;
            tempOPT.indexNumber = currentAddressIndex;
            tempOPT.prevIndexNumber = currentAddressIndex;
            tempOPT.frameNumber = OPT_Record[i].frameNumber;

            //update all other OPT index
            for(uint a=0; a<OPT_Record.size(); a++){
                OPT_Record[a].indexNumber = OPT_Record[a].prevIndexNumber;
            }

            //update the current OPT
            OPT_Record[i] = tempOPT;
            return;
        }
    }

    //if there exist the future addresses - then evict the one with the greatest index value
    OPT_struct maxIndexOPT;
    int maxOPTindexVal = 0;
    for(uint i=0; i<OPT_Record.size(); i++){
        if(OPT_Record[i].indexNumber > maxIndexOPT.indexNumber){
            maxIndexOPT = OPT_Record[i];
            maxOPTindexVal = i;
        }
    }

    //evict the address with the greatest index
    pageTable[maxIndexOPT.pageNumber].present = 0;
    physMem[maxIndexOPT.frameNumber] = frame;
    pageTable[pageIndexNumber].present = 1;
    pageTable[pageIndexNumber].frameNumber = maxIndexOPT.frameNumber;

    //update the tlb
    for(uint t = 0; t < TLB.size(); t++){
        if(TLB[t].pageNumber == maxIndexOPT.pageNumber){
            TLB.erase(TLB.begin() + t);
            break;
        }
    }
    TLBStruct mapPage;
    mapPage.frameNumber = pageTable[pageIndexNumber].frameNumber;
    mapPage.pageNumber = pageIndexNumber;
    TLB.push_back(mapPage);

    //update all other OPT index
    for(uint a=0; a<OPT_Record.size(); a++){
        OPT_Record[a].indexNumber = OPT_Record[a].prevIndexNumber;
    }

    //update the OPT_rec - (frame number stays same cuz we are evicting the frame value)
    maxIndexOPT.pageNumber = pageIndexNumber;
    maxIndexOPT.indexNumber = currentAddressIndex;
    maxIndexOPT.prevIndexNumber = currentAddressIndex;
    maxIndexOPT.frameNumber = OPT_Record[maxOPTindexVal].frameNumber;

    OPT_Record[maxOPTindexVal] = maxIndexOPT;
    return;
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

            //update the LRU_rec
            LRU_rec(TLB[tlbIndex].frameNumber, TLB[tlbIndex].pageNumber);

            //update the FIFO_rec
            FIFO_rec(TLB[tlbIndex].frameNumber, TLB[tlbIndex].pageNumber);

            //add the OPT struct until frame size is full
            OPT_update(TLB[tlbIndex].frameNumber, TLB[tlbIndex].pageNumber, i);

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

                //update the LRU_rec
                LRU_rec(pageTable[pageIndexNumber].frameNumber, pageIndexNumber);

                //update the FIFO_rec
                FIFO_rec(pageTable[pageIndexNumber].frameNumber, pageIndexNumber);

                //add the OPT struct until frame size is full
                OPT_update(pageTable[pageIndexNumber].frameNumber, pageIndexNumber, i);
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

                //update the LRU_rec
                LRU_rec(pageTable[pageIndexNumber].frameNumber, pageIndexNumber);

                //update the FIFO_rec
                FIFO_rec(pageTable[pageIndexNumber].frameNumber, pageIndexNumber);

                //add the OPT struct until frame size is full
                OPT_update(pageTable[pageIndexNumber].frameNumber, pageIndexNumber, i);
            }
            //physical memory is full - perform eviction based on the algorithm
            else{
                if(algorithm == FIFO){
                    FIFO_eviction(backingStore[pageIndexNumber], pageIndexNumber);
                    printf("%d, %d, %d, ",reference_sequence[i], physMem[pageTable[pageIndexNumber].frameNumber].element[offsetPosition], pageTable[pageIndexNumber].frameNumber);
                    printFrameContent(physMem[pageTable[pageIndexNumber].frameNumber]);
                }
                else if(algorithm == LRU){
                    LRU_eviction(backingStore[pageIndexNumber], pageIndexNumber);
                    printf("%d, %d, %d, ",reference_sequence[i], physMem[pageTable[pageIndexNumber].frameNumber].element[offsetPosition], pageTable[pageIndexNumber].frameNumber);
                    printFrameContent(physMem[pageTable[pageIndexNumber].frameNumber]);
                }
                else if(algorithm == OPT){
                    OPT_eviction(backingStore[pageIndexNumber], pageIndexNumber, i);
                    printf("%d, %d, %d, ",reference_sequence[i], physMem[pageTable[pageIndexNumber].frameNumber].element[offsetPosition], pageTable[pageIndexNumber].frameNumber);
                    printFrameContent(physMem[pageTable[pageIndexNumber].frameNumber]);
                }
            }
        }
    }

    // calculate the virtual Mem Info
    calculateVirtualMemInfo();
    printVirtualMemInfo();
}

int main(int argc, char * argv[]){

    if(argc != 4){
        printf("Provide the correct number of arguments\n");
        printf("memSim <reference-sequence-file.txt> <FRAMES> <PRA>\n");
        exit(EXIT_FAILURE);
    }

    char * reference_sequence_file = argv[1];
    //physical memory size
    uint phyEntrySize = atoi(argv[2]);

    char * textAlgorithm = argv[3];

    if(strcmp(textAlgorithm,"FIFO") == 0){
        algorithm = FIFO;
    }
    else if(strcmp(textAlgorithm,"OPT") == 0){
        algorithm = OPT;
    }
    else if(strcmp(textAlgorithm,"LRU") == 0){
        algorithm = LRU;
    }
    else{
        printf("Eviction Algorithm provided is unknown.\n");
    }

    //initialize virtualMemInfo
    memInfo.numTransAddress = 0;
    memInfo.pageFault = 0;
    memInfo.pageFaultRate = 0;
    memInfo.tlbHitRate = 0;
    memInfo.tlbHits = 0;
    memInfo.tlbMisses = 0;

    //update entries based on the file input
    updateEntryInfo(reference_sequence_file);
    memInfo.numTransAddress = reference_sequence.size();
    performFetching(phyEntrySize);

    return 0;
}