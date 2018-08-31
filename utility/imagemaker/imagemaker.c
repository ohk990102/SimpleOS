#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

int adjustInSectorSize(int fd, int sourceSize);
void writeKernelInformation(int targetFd, int kernelSectorCount);
int copyFile(int sourceFd, int targetFd);

int main(int argc, char * argv[]) {
    int sourceFd;
    int targetFd;
    int bootLoaderSize;
    int kernel32SectorCount;
    int sourceSize;

    if(argc < 3) {
        fprintf(stderr, "[!] %s bootloader.bin kernel32.bin\n", argv[0]);
        exit(-1);
    }
    if((targetFd = open("disk.img", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE)) == -1) {
        fprintf(stderr, "[!] disk.img open failed\n");
        exit(-1);
    }

    printf("[*] Copying bootloader to image file\n");
    if((sourceFd = open(argv[1], O_RDONLY | O_BINARY)) == -1) {
        fprintf(stderr, "[!] %s open fail\n", argv[1]);
        exit(-1);
    }
    sourceSize = copyFile(sourceFd, targetFd);
    close(sourceFd);

    bootLoaderSize = adjustInSectorSize(targetFd, sourceSize);
    printf("[+] %s size = [%d] and sector count = [%d]\n", argv[1], sourceSize, bootLoaderSize);
    printf("[*] Copying protected mode kernel to image file\n");
    if((sourceFd = open(argv[2], O_RDONLY | O_BINARY)) == -1) {
        fprintf(stderr, "[!] %s open fail\n", argv[2]);
        exit(-1);
    }
    sourceSize = copyFile(sourceFd, targetFd);
    close(sourceFd);

    kernel32SectorCount = adjustInSectorSize(targetFd, sourceSize);
    printf("[+] %s size = [%d] and sector count = [%d]\n", argv[2], sourceSize, kernel32SectorCount);

    printf("[*] Started to write kernel information\n");

    writeKernelInformation(targetFd, kernel32SectorCount);
    printf("[+] Image file create complete\n");

    close(targetFd);
    return 0;
}

int adjustInSectorSize(int fd, int sourceSize) {
    int i;
    int adjustSizeToSector;
    char ch;
    int sectorCount;

    adjustSizeToSector = sourceSize % BYTESOFSECTOR;
    ch = 0x00;

    if(adjustSizeToSector != 0) {
        adjustSizeToSector = 512 - adjustSizeToSector;
        printf("[+] File size [%lu] and fill [%u] byte\n", sourceSize, adjustSizeToSector);
        for(i = 0; i < adjustSizeToSector; i++) {
            write(fd, &ch, 1);
        }
    }
    else {
        printf("[+] File size is aligned to 512 byte\n");
    }
    sectorCount = (sourceSize + adjustSizeToSector) / BYTESOFSECTOR;
    return sectorCount;
}

void writeKernelInformation(int targetFd, int kernelSectorCount) {
    unsigned short data;
    long position;
    
    position = lseek(targetFd, 5, SEEK_SET);
    if(position == -1) {
        fprintf(stderr, "[!] lseek fail. Return value = %d, errno = %d, %d\n", position, errno, SEEK_SET);
        exit(-1);
    }
    data = (unsigned short) kernelSectorCount;
    write(targetFd, &data, 2);
    printf("[+] Total sector count except boot loader [%d]\n", kernelSectorCount);
}

int copyFile(int sourceFd, int targetFd) {
    int sourceFileSize;
    int readSize;
    int writeSize;
    char buffer[BYTESOFSECTOR];

    sourceFileSize = 0;
    while(1) {
        readSize = read(sourceFd, buffer, sizeof(buffer));
        writeSize = write(targetFd, buffer, readSize);

        if(readSize != writeSize) {
            fprintf(stderr, "[!] readSize != writeSize\n");
            exit(-1);
        }
        sourceFileSize += readSize;
        if(readSize != sizeof(buffer)) {
            break;
        }
    }
    return sourceFileSize;
}