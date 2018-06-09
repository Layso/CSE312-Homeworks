#include "File.h"
#include <iostream>
#include <string.h>

#define ZERO 0




void File::WriteData(std::vector<char> newData) {
	if (offset == data.size()) {
		for (int i=0; i<newData.size(); ++i) {
			data.push_back(newData[i]);
		}
		
		offset = data.size();
	}
	
	else {
		for (int i=0; i<newData.size(); ++i) {
			if (offset<data.size()) {
				data[offset++] = newData[i];
			}
			else {
				data.push_back(newData[i]);
			}
		}
	}
}



void File::SetData(std::vector<char> newData) {
	data = newData;
}



void File::Deserialize(struct FileSerializer serializer) {
	id = serializer.id;
	accessDate = serializer.accessDate;
	creationDate = serializer.creationDate;
	modificationDate = serializer.modificationDate;
	strcpy(name, serializer.name);
}



struct FileSerializer File::Serialize() {
	
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