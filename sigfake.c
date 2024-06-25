#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

#include "bootimg.h"

static uint8_t fakesig[] = {
    0x30, 0x82, 0x04, 0xF8, 0x02, 0x01, 0x01, 0x30, 0x82, 0x03, 0xD1, 0x30, 0x82, 0x02, 0xB9, 0xA0, 
    0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00, 0xDB, 0x10, 0x2B, 0x70, 0x25, 0x56, 0xE2, 0xE4, 0x30, 
    0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x30, 0x7F, 
    0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x43, 0x4E, 0x31, 0x12, 0x30, 
    0x10, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C, 0x09, 0x47, 0x75, 0x61, 0x6E, 0x67, 0x64, 0x6F, 0x6E, 
    0x67, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x09, 0x53, 0x68, 0x65, 0x6E, 
    0x7A, 0x68, 0x65, 0x6E, 0x67, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x04, 
    0x4F, 0x50, 0x50, 0x4F, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x04, 0x4F, 
    0x50, 0x50, 0x4F, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x04, 0x4F, 0x50, 
    0x50, 0x4F, 0x31, 0x1B, 0x30, 0x19, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 
    0x01, 0x16, 0x0C, 0x77, 0x6C, 0x7A, 0x40, 0x6F, 0x70, 0x70, 0x6F, 0x2E, 0x63, 0x6F, 0x6D, 0x30, 
    0x1E, 0x17, 0x0D, 0x31, 0x36, 0x30, 0x38, 0x32, 0x32, 0x30, 0x37, 0x33, 0x34, 0x34, 0x39, 0x5A, 
    0x17, 0x0D, 0x31, 0x36, 0x30, 0x39, 0x32, 0x31, 0x30, 0x37, 0x33, 0x34, 0x34, 0x39, 0x5A, 0x30, 
    0x7F, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x43, 0x4E, 0x31, 0x12, 
    0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C, 0x09, 0x47, 0x75, 0x61, 0x6E, 0x67, 0x64, 0x6F, 
    0x6E, 0x67, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x09, 0x53, 0x68, 0x65, 
    0x6E, 0x7A, 0x68, 0x65, 0x6E, 0x67, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 
    0x04, 0x4F, 0x50, 0x50, 0x4F, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x04, 
    0x4F, 0x50, 0x50, 0x4F, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x04, 0x4F, 
    0x50, 0x50, 0x4F, 0x31, 0x1B, 0x30, 0x19, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 
    0x09, 0x01, 0x16, 0x0C, 0x77, 0x6C, 0x7A, 0x40, 0x6F, 0x70, 0x70, 0x6F, 0x2E, 0x63, 0x6F, 0x6D, 
    0x30, 0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 
    0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0F, 0x00, 0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 
    0x00, 0xB9, 0xB5, 0xEA, 0xCC, 0x4E, 0xC9, 0x2E, 0xA9, 0x69, 0xD7, 0xF9, 0xC9, 0x76, 0x3D, 0xE1, 
    0xFA, 0x7E, 0x34, 0x34, 0x3A, 0x7E, 0x1F, 0x0D, 0xC3, 0x12, 0xAE, 0x76, 0xF3, 0x1C, 0xFF, 0xDD, 
    0x0D, 0x3A, 0x4E, 0xC8, 0x8E, 0xFC, 0x54, 0xC8, 0xC6, 0x2A, 0xA7, 0x57, 0xA1, 0xDA, 0x02, 0xE0, 
    0x3C, 0x21, 0xAE, 0xCC, 0xA8, 0x78, 0xF2, 0xC7, 0x6C, 0xD0, 0xC0, 0x20, 0x3F, 0x27, 0x73, 0x7F, 
    0x53, 0x23, 0xEB, 0xFF, 0x35, 0x25, 0xA5, 0xAD, 0x05, 0x4F, 0x02, 0xF8, 0x11, 0xC8, 0xA8, 0x70, 
    0x88, 0x83, 0x9C, 0xFB, 0x47, 0xB5, 0xD2, 0xE5, 0xFF, 0xF0, 0x30, 0xF1, 0xB6, 0xB1, 0xBB, 0xDF, 
    0x0D, 0xE9, 0x52, 0xB8, 0x50, 0x0E, 0x8F, 0x96, 0x71, 0xB0, 0xB9, 0xF2, 0x4A, 0xFA, 0xE4, 0x26, 
    0x73, 0x56, 0xEB, 0xC3, 0x15, 0x02, 0xA9, 0x36, 0x41, 0x5A, 0xC9, 0xF2, 0x01, 0xFF, 0x9C, 0x3D, 
    0x8C, 0xE7, 0x9A, 0x2E, 0x3D, 0xEB, 0x55, 0xF7, 0xE8, 0xDD, 0x80, 0x54, 0x29, 0x32, 0xC5, 0xB3, 
    0x50, 0xDB, 0xDA, 0xFF, 0xCA, 0xBA, 0xF4, 0xB1, 0x7F, 0x1F, 0x7C, 0xE6, 0x38, 0x0B, 0x2F, 0xA6, 
    0x6C, 0xE3, 0xD8, 0xE7, 0x03, 0x2F, 0xDC, 0x99, 0x38, 0x46, 0xEA, 0xA4, 0x04, 0x39, 0xBC, 0xB8, 
    0x4D, 0xCD, 0xFC, 0x38, 0x42, 0xC0, 0x84, 0x33, 0x01, 0x1F, 0x12, 0x2E, 0x40, 0x2D, 0x1C, 0xB0, 
    0xBD, 0x30, 0xE0, 0x8F, 0x68, 0x62, 0xEC, 0x32, 0x9B, 0x15, 0x5E, 0xE2, 0x0A, 0xE0, 0xA7, 0x62, 
    0x00, 0xF8, 0xD3, 0x53, 0x31, 0x18, 0x60, 0x4E, 0xFE, 0xBD, 0xA9, 0x09, 0xF9, 0xF0, 0x7E, 0x1E, 
    0xA4, 0xDB, 0x1C, 0x13, 0x99, 0x76, 0xA0, 0xD6, 0x55, 0xFE, 0xE5, 0x7C, 0x65, 0xD2, 0xED, 0x25, 
    0x0E, 0x61, 0xA5, 0xB7, 0x16, 0x69, 0xED, 0x37, 0xFB, 0xD0, 0x7A, 0xD2, 0x2F, 0xEE, 0x59, 0x81, 
    0x27, 0x02, 0x03, 0x01, 0x00, 0x01, 0xA3, 0x50, 0x30, 0x4E, 0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 
    0x0E, 0x04, 0x16, 0x04, 0x14, 0x2E, 0xC0, 0x34, 0xF1, 0x97, 0x1F, 0xCE, 0x4B, 0x04, 0x82, 0x3B, 
    0x63, 0x83, 0xD2, 0xBE, 0x6D, 0x77, 0x2A, 0x3B, 0x27, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 
    0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x2E, 0xC0, 0x34, 0xF1, 0x97, 0x1F, 0xCE, 0x4B, 0x04, 0x82, 
    0x3B, 0x63, 0x83, 0xD2, 0xBE, 0x6D, 0x77, 0x2A, 0x3B, 0x27, 0x30, 0x0C, 0x06, 0x03, 0x55, 0x1D, 
    0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xFF, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 
    0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x3E, 0xB4, 0x2B, 0xE1, 
    0x7E, 0x8B, 0xFD, 0xE1, 0xA8, 0x67, 0xFB, 0x06, 0x56, 0xC7, 0x2A, 0xF9, 0x74, 0x3E, 0xC4, 0xFD, 
    0xF2, 0x05, 0xF8, 0x70, 0xB8, 0xEF, 0x76, 0x24, 0x4E, 0xAE, 0xA3, 0xA6, 0x24, 0xD3, 0xB4, 0xF6, 
    0x61, 0xAA, 0x41, 0xBE, 0xF5, 0x46, 0xAA, 0xB1, 0xB1, 0x12, 0x70, 0x68, 0x61, 0xF8, 0xAD, 0xA0, 
    0xE1, 0xAE, 0x29, 0x16, 0xF1, 0x6B, 0xD8, 0xCC, 0x6D, 0xD2, 0xFF, 0xCF, 0x15, 0x5A, 0xA9, 0x8E, 
    0xC7, 0xF9, 0xA0, 0x4A, 0xBB, 0x49, 0x07, 0x8F, 0x70, 0x35, 0x76, 0xB9, 0xB3, 0xFF, 0x61, 0xA5, 
    0x97, 0x34, 0xB4, 0x16, 0xFF, 0x90, 0x84, 0x91, 0x16, 0x46, 0x24, 0x78, 0xB8, 0x76, 0xA8, 0x84, 
    0x6B, 0xC7, 0x43, 0xC2, 0x7B, 0x7D, 0x8F, 0x0D, 0x3C, 0x0C, 0xBF, 0x29, 0x8D, 0x86, 0x13, 0x2D, 
    0xCF, 0x4B, 0xBF, 0xE9, 0xA1, 0xEB, 0x4E, 0x54, 0x88, 0xA4, 0x7E, 0x6E, 0x58, 0xC4, 0x38, 0x45, 
    0x8D, 0xB0, 0x07, 0xF6, 0x76, 0xA2, 0x78, 0xBA, 0x95, 0x8B, 0xAF, 0x5F, 0xE1, 0xF9, 0x1C, 0x06, 
    0x0F, 0x53, 0x3C, 0xC0, 0x6C, 0xDE, 0x87, 0x30, 0x30, 0x95, 0x7D, 0x3F, 0x1A, 0xB4, 0xE8, 0xEB, 
    0xED, 0x34, 0x66, 0xDB, 0xED, 0x87, 0x0B, 0xB4, 0x3E, 0xA7, 0xC8, 0x71, 0xF6, 0x55, 0xF1, 0x2D, 
    0x8F, 0xEB, 0xF2, 0xC1, 0xEB, 0x40, 0xB5, 0x25, 0xA7, 0xC9, 0xBE, 0x68, 0xAA, 0x0E, 0x1B, 0xDD, 
    0x91, 0xEF, 0xA1, 0x5D, 0x7F, 0xDC, 0x04, 0xCC, 0x1A, 0x94, 0xFF, 0x25, 0x22, 0x0A, 0x13, 0x45, 
    0x73, 0xB1, 0x1F, 0x10, 0xAA, 0x6C, 0x2A, 0x94, 0x40, 0x5E, 0xAC, 0x3B, 0xF0, 0xDA, 0x1B, 0x30, 
    0xB2, 0xD5, 0xA3, 0x2E, 0x12, 0x3B, 0x54, 0x30, 0x69, 0x72, 0xAE, 0xEE, 0xCF, 0xC5, 0xBD, 0xF0, 
    0x6D, 0x3A, 0xE3, 0x50, 0xE5, 0x09, 0x75, 0xF5, 0x86, 0xCF, 0x2C, 0x95, 0x30, 0x0B, 0x06, 0x09, 
    0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x30, 0x0D, 0x13, 0x05, 0x2F, 0x62, 0x6F, 
    0x6F, 0x74, 0x02, 0x04, 0x00, 0xA3, 0x10, 0x00, 0x04, 0x82, 0x01, 0x00, 0x70, 0xB2, 0xA7, 0x85, 
    0x12, 0x99, 0xB6, 0x02, 0x55, 0x2C, 0xC8, 0x94, 0xC5, 0xC3, 0xD5, 0x08, 0x88, 0x60, 0x98, 0xD0, 
    0x0D, 0xF9, 0xCF, 0x03, 0x3B, 0xC9, 0xA9, 0x69, 0x54, 0x75, 0x43, 0x06, 0xCE, 0xAD, 0x6A, 0x60, 
    0x4B, 0xD0, 0x63, 0x9A, 0x06, 0xF4, 0xD0, 0xD1, 0xD9, 0x3E, 0xA8, 0x4B, 0xA7, 0xA6, 0xA5, 0xD4, 
    0xEE, 0x19, 0x80, 0xBC, 0x1B, 0x5C, 0x28, 0x90, 0xD5, 0xA6, 0xBE, 0x70, 0x2F, 0xD8, 0x9D, 0x0E, 
    0x94, 0x09, 0x24, 0x8E, 0x21, 0x40, 0xA1, 0xFE, 0xA4, 0x76, 0xCD, 0x04, 0x7C, 0x3D, 0x61, 0xBC, 
    0x5E, 0xE0, 0xE1, 0x35, 0xAF, 0x6A, 0x53, 0xCC, 0x66, 0x3C, 0xF9, 0xDC, 0xAA, 0xCD, 0x70, 0x7F, 
    0x5E, 0x6B, 0xEE, 0xD9, 0x1E, 0xBB, 0x87, 0xEC, 0xD0, 0xA4, 0x02, 0x77, 0xF1, 0x7D, 0x73, 0x60, 
    0xB1, 0x9E, 0xC2, 0xF4, 0x12, 0x30, 0x2E, 0x11, 0xDA, 0xDF, 0xF6, 0xC2, 0x21, 0x81, 0x90, 0x25, 
    0x10, 0x5A, 0x38, 0x41, 0x5F, 0xD6, 0xE1, 0xA1, 0x40, 0x61, 0x8B, 0x50, 0x6D, 0x1B, 0x74, 0x80, 
    0xB1, 0xAF, 0x36, 0x45, 0x61, 0xCC, 0xDA, 0x98, 0x23, 0x45, 0x30, 0xD2, 0xDF, 0x61, 0xE7, 0x62, 
    0xEF, 0x87, 0x5C, 0x5A, 0xD8, 0x71, 0xB4, 0x1E, 0x25, 0x3C, 0x54, 0x98, 0x17, 0x38, 0x15, 0x6E, 
    0xD5, 0xBF, 0x38, 0xFA, 0x1E, 0x28, 0x4B, 0x3D, 0x93, 0x3A, 0xA6, 0xF2, 0x85, 0xD0, 0x6E, 0x8B, 
    0x8E, 0xB2, 0x39, 0x9D, 0xA4, 0x95, 0x1C, 0x1D, 0xE8, 0xBC, 0x98, 0xCA, 0xC1, 0xE1, 0x6E, 0x19, 
    0xA1, 0x14, 0x5F, 0xBA, 0x77, 0x59, 0x76, 0xF3, 0xAB, 0xF5, 0x0C, 0x6D, 0x06, 0x4B, 0x77, 0xBC, 
    0x5A, 0x29, 0x6D, 0x0B, 0x5F, 0x46, 0xE8, 0x83, 0x2A, 0x7B, 0x9C, 0x88, 0x02, 0xE6, 0x83, 0x81, 
    0x40, 0xF2, 0x03, 0x6A, 0x78, 0x32, 0x5B, 0x55, 0x92, 0xDC, 0x13, 0x38, 
};

uint32_t getBlockSize(uint32_t page_size, uint32_t n) {
    return ((n + page_size - 1) / page_size) * page_size;
}

// Get boot image size
uint32_t retSigOffset(int fd) {
    // uint32_t ret = 0;
    lseek(fd, 0, SEEK_SET);
    struct boot_img_hdr bhead;
    read(fd, &bhead, sizeof(bhead));
    uint32_t kernelsize = getBlockSize(bhead.page_size, bhead.kernel_size);
    uint32_t ramdisksize = getBlockSize(bhead.page_size, bhead.ramdisk_size);
    uint32_t secondsize = getBlockSize(bhead.page_size, bhead.second_size);

    return bhead.page_size + kernelsize + ramdisksize + secondsize;
}

uint32_t signFakeSignature(int fd) {
    // Get signature offset
    uint32_t sigoff = retSigOffset(fd);
    fprintf(stdout, "Get signature offset : %d\n", sigoff);
    lseek(fd, sigoff, SEEK_SET);
    fprintf(stdout, "Writing fake signatures ... ");
    write(fd, fakesig, sizeof(fakesig));
    fprintf(stdout, "Done !\n");
    return 0;
}

void Usage(void) {
    fprintf(stdout, 
    "Usage:\n"
    "\t./sigfake [boot.img]\n"
    "Use this tool by your own risk !\n"
    "Author : affggh\nUnder GPLv3 License\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        Usage();
        return 0;
    }

    if (access(argv[1], F_OK) != 0) {
        fprintf(stdout, "File does not exist !\n");
        return 1;
    }

    int fd = open("boot.img", O_BINARY | O_RDWR);
    if (!fd) {
        fprintf(stdout, "File open failed !\n");
        return 1;
    }
    // int sigoff = retSigOffset(fd);
    // printf("signature offset : %d\n", sigoff);
    signFakeSignature(fd);
    return 0;
}
