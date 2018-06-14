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



FileSystem::~FileSystem() {
	WriteSystemFile();
}



void FileSystem::ReadDir(int *fileCount, int *allocatedBytes, int *usedBytes, int *blockCount) {
	*fileCount = files.size();
	*allocatedBytes = BLOCK_SIZE * usedBlocks;
	*blockCount = usedBlocks;
	*usedBytes = ZERO;
	for (int i=0; i<files.size(); ++i) {
		*usedBytes += files[i].GetSize();
	}
}



bool FileSystem::SeekFile(int fileId, int position) {
	for (int i=0; i<files.size(); ++i) {
		if (files[i].GetId() == fileId) {
			return files[i].SetOffset(position);
		}
	}
}



std::vector<char> FileSystem::ReadFile(int fileId, int size, bool *status) {
	std::vector<char> result;
	int offset;
	int fileSize;
	
	
	
	for (int i=0; i<files.size(); ++i) {
		if (files[i].GetId() == fileId) {
			offset = files[i].GetOffset();
			fileSize = files[i].GetSize();
			
			
			if (offset + size > fileSize) {
				*status = false;
				return result;
			}
			
			else {
				result = files[i].ReadData(size);
				*status = true;
				return result;
			}
		}
	}
}



int FileSystem::WriteFile(int fileId, std::vector<char> data) {
	for (int i=0; i<files.size(); ++i) {
		if (fileId == files[i].GetId()) {
			int fileSize = files[i].GetSize();
			int fileOffset = files[i].GetOffset();
			int fileBlocks = files[i].GetBlocks();
			int emptyFileSize = (fileBlocks*BLOCK_SIZE) - fileOffset;
			int requiredExtraSize = data.size() - emptyFileSize;
			
			
			if (requiredExtraSize >= ZERO) {
				int requiredExtraBlock = requiredExtraSize / BLOCK_SIZE;
				if (requiredExtraSize%BLOCK_SIZE != ZERO) {
					++requiredExtraBlock;
				}
				
				if (usedBlocks + requiredExtraBlock >= TOTAL_BLOCKS) {
					return ERROR_CODE;
				}
				
				else {
					usedBlocks += requiredExtraBlock;
					files[i].GiveBlock(requiredExtraBlock);
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
			files[i].SetOffset(ZERO);
			return files[i].GetId();
		}
	}
	
	
	return ERROR_CODE;
}



int FileSystem::CreateFile(std::vector<char> name) {
	File file;
	char tempName[NAME_SIZE];
	bool status = true;
	struct FileSerializer serializer;
	
	
	if (usedBlocks != TOTAL_BLOCKS) {
		serializer.id = files.size()+1;
		serializer.accessDate = time(NULL);
		serializer.creationDate = time(NULL);
		serializer.modificationDate = time(NULL);
		serializer.size = ZERO;
		serializer.blocks = 1;
		memset(serializer.name, ZERO, NAME_SIZE);
		for (int i=0; i<name.size() && i<NAME_SIZE; ++i) {
			serializer.name[i] = name[i];
		}
		
		
		for (int i=0; i<files.size(); ++i) {
			files[i].GetName(tempName);
			if (!strcmp(serializer.name, tempName)) {
				status = false;
			}
		}
		
		if (status) {
			usedBlocks++;
			file.Deserialize(serializer);
			files.push_back(file);
			return serializer.id;
		}
	}
	
	
	return ERROR_CODE;
}



void FileSystem::ReadSystemFile(char *newFileName) {
	char buffer;
	int descriptor;
	struct stat stats;
	struct FileSystemSerializer sysSerializer;
	struct FileSerializer fileSerializer;



	if ((descriptor = open(newFileName, O_RDONLY)) == ERROR_CODE) {
		std::cout << "Error! File system couldn't opened to read\n";
		exit(EXIT_FAILURE);
	}

	strcpy(fileName, newFileName);
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
	
	
	close(descriptor);
}


void FileSystem::WriteSystemFile() {
	int descriptor;
	struct stat stats;
	struct FileSystemSerializer sysSerializer;
	
	
	if (files.size() > ZERO) {
		if ((descriptor = open(fileName, O_WRONLY | O_TRUNC)) == ERROR_CODE) {
			std::cout << "Error! File system couldn't opened to write\n";
			exit(EXIT_FAILURE);
		}
		
		sysSerializer.usedBlocks = usedBlocks;
		sysSerializer.fileCount = files.size();
		write(descriptor, &sysSerializer, sizeof(sysSerializer));
		
		for (int i=0; i<files.size(); ++i) {
			struct FileSerializer fileSerializer = files[i].Serialize();
			std::vector<char> fileData = files[i].GetData();
			write(descriptor, &fileSerializer, sizeof(fileSerializer));
			
			for (int j=0; j<fileData.size(); ++j) {
				write(descriptor, &fileData[j], sizeof(char));
			}
		}
		
		close(descriptor);
	}
}


void FileSystem::PrintFiles(std::vector<int> fids) {
	char name[NAME_SIZE];
	char create[20];
	char access[20];
	char modify[20];
	time_t tm;
	
	
	if (fids.size() > ZERO)
		std::cout << std::endl << "Name - Size - Create Date - Modification Date - Access Date - Block Size\n";
	
	for (int i=0; i<fids.size(); ++i) {
		for (int j=0; j<files.size(); ++j) {
			if (fids[i] == files[j].GetId()) {
				files[j].GetName(name);
				
				tm = files[j].GetCreation();
				strftime(create, 20, "%Y-%m-%d %H:%M:%S", localtime(&tm));
				
				tm = files[j].GetModification();
				strftime(modify, 20, "%Y-%m-%d %H:%M:%S", localtime(&tm));
				
				tm = files[j].GetAccess();
				strftime(access, 20, "%Y-%m-%d %H:%M:%S", localtime(&tm));
				
				std::cout << name << " - " << files[j].GetSize() << " - " << create << " - " << modify << " - " << access << " - " << files[j].GetBlocks() << std::endl;
			}
		}
	}
	
	if (fids.size() > ZERO)
		std::cout << std::endl;
}



void FileSystem::PrintDirectory() {
	int fileCount, allocCount, usageCount, blockCount;
	std::vector<int> allFiles;
	
	
	for (int i=0; i<files.size(); ++i) {
		allFiles.push_back(files[i].GetId());
	}
	
	ReadDir(&fileCount, &allocCount, &usageCount, &blockCount);
	std::cout << std::endl;
	std::cout << "Directory name:   /" << std::endl;
	std::cout << "File number:      " << fileCount << std::endl;
	std::cout << "Size used:        " << usageCount << "/" << DISK_SPACE << std::endl;
	std::cout << "Size allocated:   " << allocCount << "/" << DISK_SPACE << std::endl;
	std::cout << "Blocks allocated: " << blockCount << "/" << TOTAL_BLOCKS << std::endl;
	PrintFiles(allFiles);
}