#include "FileSystem.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#define ZERO 0
#define ERROR_CODE -1



FileSystem::FileSystem() {
	usedBlocks = ZERO;
}


int FileSystem::WriteFile(int fileId, std::vector<char> data) {
	std::cout << "Current block usage: " << usedBlocks << "/" << TOTAL_BLOCKS << std::endl;
	for (int i=0; i<files.size(); ++i) {
		if (fileId == files[i].GetId()) {
			/* TODO: 
				Increase block size if new data exceeds block
				Return error if total block size exceeded
				Return size if not
			*/
			int offset = files[i].GetOffset();
			int emptyBlockSize = files[i].GetSize();
			int requiredExtraSize = data.size() - emptyBlockSize;
			
			if (requiredExtraSize > ZERO) {
				int requiredExtraBlock = requiredExtraSize / BLOCK_SIZE;
				if (requiredExtraSize%BLOCK_SIZE != ZERO) {
					++requiredExtraBlock;
				}
				
				if (usedBlocks + requiredExtraBlock >= TOTAL_BLOCKS) {
					return ERROR_CODE;
				}
				
				else {
					std::cout << "Empty: " << emptyBlockSize << std::endl;
					std::cout << "Size: " << requiredExtraSize << std::endl;
					std::cout << "Block: " << requiredExtraBlock << std::endl;
					
					usedBlocks += requiredExtraBlock;
					files[i].WriteData(data);
					return data.size();
				}
			}
			
			else {
				files[i].WriteData(data);
				return data.size();
			}
		}
	}
	
	
	return ERROR_CODE;
}



int FileSystem::OpenFile(std::vector<char> name) {
	char fileName[NAME_SIZE];
	char tempName[NAME_SIZE];
	
	
	memset(fileName, ZERO, NAME_SIZE);
	for (int i=0; i<name.size() && i<NAME_SIZE; ++i) {
		fileName[i] = name[i];
	}
	
	
	for (int i=0; i<files.size(); ++i) {
		files[i].GetName(tempName);
		if (!strcmp(tempName, fileName)) {
			return files[i].GetId();
		}
	}
	
	
	return ERROR_CODE;
}



int FileSystem::CreateFile(std::vector<char> name) {
	File file;
	struct FileSerializer serializer;
	
	
	if (usedBlocks != TOTAL_BLOCKS) {
		serializer.id = files.size()+1;
		serializer.accessDate = time(NULL);
		serializer.creationDate = time(NULL);
		serializer.modificationDate = time(NULL);
		serializer.size = ZERO;
		memset(serializer.name, ZERO, NAME_SIZE);
		for (int i=0; i<name.size() && i<NAME_SIZE; ++i) {
			serializer.name[i] = name[i];
		}

		usedBlocks++;
		file.Deserialize(serializer);
		files.push_back(file);
		return serializer.id;
	}
	
	
	return ERROR_CODE;
}



void FileSystem::ReadSystemFile(char *fileName) {
	char buffer;
	int descriptor;
	struct stat stats;
	struct FileSystemSerializer sysSerializer;
	struct FileSerializer fileSerializer;



	if ((descriptor = open(fileName, O_RDWR)) == ERROR_CODE) {
		std::cout << "Error! File system couldn't opened\n";
		exit(EXIT_FAILURE);
	}

	stat(fileName, &stats);
	if (stats.st_size == ZERO) {
		usedBlocks = ZERO;
	}

	else {
		read(descriptor, &sysSerializer, sizeof(sysSerializer));
		usedBlocks = sysSerializer.usedBlocks;
		
		for (int i=0; i<sysSerializer.fileCount; ++i) {
			File tempFile;
			std::vector<char> tempVector;
			
			read(descriptor, &fileSerializer, sizeof(fileSerializer));
			for (int j=0; j<fileSerializer.size; ++j) {
				read(descriptor, &buffer, sizeof(buffer));
				tempVector.push_back(buffer);
			}
			
			tempFile.SetData(tempVector);
			tempFile.Deserialize(fileSerializer);
			files.push_back(tempFile);
		}
	}
}