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
		~FileSystem();
		
		
		void PrintFiles(std::vector<int> fids);
		void PrintDirectory();
		void ReadDir(int *fileCount, int *allocatedBytes, int *usedBytes, int *blockCount);
		void ReadSystemFile(char *fileName);
		int CreateFile(std::vector<char> name);
		int OpenFile(std::vector<char> name);
		int WriteFile(int fileId, std::vector<char> data);
		std::vector<char> ReadFile(int fileId, int size, bool *status);
		bool SeekFile(int fileId, int position);
		void WriteSystemFile();
	private:
		std::vector<struct File> files;
		int usedBlocks;
		char fileName[250];
};


#endif