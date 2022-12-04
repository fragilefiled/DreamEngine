#pragma once
#include<fstream>
#include<vector>
#include<string>

class FileOperation {
public:
	static std::vector<char> readfile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);//���ļ�ĩβ��ȡ �Զ�������ʽ��ȡ
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file");

		}
		size_t fileSize = (size_t)file.tellg(); //Get position in input sequence
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	};;

};