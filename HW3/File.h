#ifndef FILE_H
#define FILE_H

#include <time.h>
#include <vector>


const int NAME_SIZE = 50;


struct FileSerializer {
	int id;
	char name[NAME_SIZE];
	time_t creationDate;
	time_t modificationDate;
	time_t accessDate;
	int size;
	int blocks;
};


class File {
	public:
	
		int GetId();
		void GetName(char *outName);
		int GetSize();
		int GetOffset();
		int GetBlocks();
		void GiveBlock(int blockCount);
		time_t GetCreation();
		time_t GetModification();
		time_t GetAccess();
		
		
		bool SetOffset(int value);
		std::vector<char> ReadData(int size);
		void WriteData(std::vector<char> newData);
		void SetData(std::vector<char> newData);
		std::vector<char> GetData();
		void Deserialize(struct FileSerializer serializer);
		struct FileSerializer Serialize();

	private:
		int id;
		char name[NAME_SIZE];
		std::vector<char> data;
		time_t creationDate;
		time_t modificationDate;
		time_t accessDate;
		int offset;
		int blocks;
};



#endif