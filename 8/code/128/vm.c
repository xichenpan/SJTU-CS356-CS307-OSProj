#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#define PAGE_ENTRIES 256
#define PAGE_SIZE 256 // bytes
#define TLB_ENTRIES 16
#define FRAME_SIZE 256 // bytes
#define FRAME_NUM 128

typedef struct {
    int latest_use;
    int page_number;
    int frame_number;
} tlb;

typedef struct {
    bool valid;
    int frame_number;
} page;

typedef struct {
    int latest_use;
    char data[PAGE_SIZE];
} frame;

void replace_tlb(int page_number, int frame_number);

void replace_page(int page_number, int frame_number);

frame physical_mem[FRAME_NUM];
page page_table[PAGE_ENTRIES];
tlb TLB[TLB_ENTRIES];
float page_fault_rate = 0;
float tlb_hit_rate = 0;
int cu_time = 0;
int cnt = 0;
FILE *addresses;
FILE *backing_store;
FILE *out;

int main(int argc, char *argv[]) {
    assert (argc == 2);

    for (int i = 0; i < TLB_ENTRIES; i++) {
        TLB[i].latest_use = -1;
        TLB[i].page_number = -1;
        TLB[i].frame_number = -1;
    }
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        page_table[i].valid = false;
        page_table[i].frame_number = -1;
    }

    for (int i = 0; i < FRAME_NUM; i++)
        physical_mem[i].latest_use = -1;

    addresses = fopen(argv[1], "r");
    backing_store = fopen("BACKING_STORE.bin", "rb");
    out = fopen("output.txt","w");

    int address;
    int offset;
    int page_number;
    int frame_number;
    int res;

    bool tlb_hit;
    bool page_fault;

    fscanf(addresses, "%d", &address);
    while (!feof(addresses)) {
        cnt++;
        cu_time++;
        tlb_hit = false;
        page_fault = true;

        offset = address & 0x000000ff;
        page_number = (address >> 8) & 0x000000ff;
        //tlb hit
        for (int i = 0; i < TLB_ENTRIES; i++) {
            if (TLB[i].page_number == page_number) {
                tlb_hit = true;
                tlb_hit_rate++;
                page_fault = false;
                frame_number = TLB[i].frame_number;

                physical_mem[frame_number].latest_use = cu_time;
                TLB[i].latest_use = cu_time;
                break;
            }
        }
        //tlb miss, page table hit
        if (!tlb_hit && page_table[page_number].valid) {
            page_fault = false;
            frame_number = page_table[page_number].frame_number;
            replace_tlb(page_number, frame_number);
            physical_mem[frame_number].latest_use = cu_time;
        }
        //page table miss
        if (page_fault) {
            page_fault_rate++;
            page_table[page_number].valid = true;
            int min = INT_MAX;
            for (int i = 0; i < FRAME_NUM; i++)
                if (physical_mem[i].latest_use < min) {
                    min = physical_mem[i].latest_use;
                    frame_number = i;
                }
            for (int i = 0; i < PAGE_ENTRIES; i++)
                if (page_table[i].valid && page_table[i].frame_number == frame_number)
                {
                    page_table[i].valid = false;
                    break;
                }
            replace_page(page_number, frame_number);
            replace_tlb(page_number, frame_number);
            page_table[page_number].frame_number = frame_number;
        }
        res = physical_mem[frame_number].data[offset];
        fprintf(out, "Virtual address: %d Physical address: %d Value: %d\n", address, frame_number * FRAME_SIZE + offset, res);
        fscanf(addresses, "%d", &address);
    }
    fclose(addresses);
    fclose(backing_store);
    fclose(out);
    printf("Page-fault rate: %.5f, TLB hit rate: %.5f\n", page_fault_rate / cnt, tlb_hit_rate / cnt);
    return 0;
}

void replace_tlb(int page_number, int frame_number) {
    int min = INT_MAX;
    int index = 0;
    for (int i = 0; i < TLB_ENTRIES; i++)
        if (TLB[i].latest_use < min) {
            min = TLB[i].latest_use;
            index = i;
        }

    TLB[index].latest_use = cu_time;
    TLB[index].page_number = page_number;
    TLB[index].frame_number = frame_number;
}

void replace_page(int page_number, int frame_number) {
    physical_mem[frame_number].latest_use = cu_time;
    fseek(backing_store, page_number * PAGE_SIZE, SEEK_SET);
    fread(physical_mem[frame_number].data, sizeof(char), PAGE_SIZE, backing_store);
}
