#include "File.h"
#include <iostream>
#include <string.h>

#define ZERO 0




bool File::SetOffset(int value) {
	if (value < ZERO || value > data.size()) {
		return false;
	}
	
	else {
		offset = value;
		return true;
	}
}



std::vector<char> File::ReadData(int size) {
	std::vector<char> result;
	
				
	for (int i=0; i<size; ++i) {
		result.push_back(data[offset++]);
	}
	
	accessDate = time(NULL);
	
	return result;
}



void File::WriteData(std::vector<char> newData) {
	for (int i=0; i<newData.size(); ++i) {
		if (offset<data.size()) {
			data[offset++] = newData[i];
		}
		
		else {
			data.push_back(newData[i]);
			++offset;
		}
	}
	
	accessDate = time(NULL);
	modificationDate = time(NULL);
}



void File::SetData(std::vector<char> newData) {
	data = newData;
}



std::vector<char> File::GetData() {
	return data;
}



void File::Deserialize(struct FileSerializer serializer) {
	id = serializer.id;
	accessDate = serializer.accessDate;
	creationDate = serializer.creationDate;
	modificationDate = serializer.modificationDate;
	blocks = serializer.blocks;
	strcpy(name, serializer.name);
}



struct FileSerializer File::Serialize() {
	struct FileSerializer result;
	
	result.id = id;
	result.creationDate = creationDate;
	result.modificationDate = modificationDate;
	result.accessDate = accessDate;
	result.size = data.size();
	result.blocks = blocks;
	strcpy(result.name, name);
	
	return result;
}


int File::GetId() {
	return id;
}



void File::GetName(char *outName) {
	strcpy(outName, name); 
}



int File::GetSize() {
	return data.size();
}



int File::GetOffset() {
	return offset;
}



int File::GetBlocks() {
	return blocks;
}



void File::GiveBlock(int blockCount) {
	blocks += blockCount;
}



time_t File::GetCreation() {
	return creationDate;
}



time_t File::GetModification() {
	return modificationDate;
}



time_t File::GetAccess() {
	return accessDate;
}
		