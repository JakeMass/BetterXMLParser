#include <iostream>
#include <fstream>
#include "FileTree.h"


std::string make_filestring(std::string filepath) {
	std::string filestring;
	std::ifstream filestream;
	filestream.open(filepath);

	while (filestream.good()) {
		filestring += filestream.get();
	}
	filestream.close();
	return filestring;
}

int main() {
	
	//std::string filestring = make_filestring("XMLFile.xml");

	FileTree filetree ("artikel_InStock.xml");
	filetree.parse();

	std::cin.get();
	return 0;
}