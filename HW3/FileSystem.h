#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <vector>
#include "File.h"


const int BLOCK_SIZE = 256;
const int DISK_SPACE = 1048576;
const int TOTAL_BLOCKS = 4096;



struct FileSystemSerializer {
	int fileCount;
	int usedBlocks;
};





class FileSystem {
	public:
		FileSystem();
		
		void ReadSystemFile(char *fileName);
		int CreateFile(std::vector<char> name);
		int OpenFile(std::vector<char> name);
		int WriteFile(int fileId, std::vector<char> data);
		
	private:
		std::vector<struct File> files;
		int usedBlocks;
};


#endif