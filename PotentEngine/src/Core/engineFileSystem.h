#pragma once
#ifndef _POTENT_ENGINE_FILE_SYSTEM_
#define _POTENT_ENGINE_FILE_SYSTEM_

#include <string>
#include <vector>
#include <fstream>
#include "engineCore.h"

namespace potent {
	struct FileData {
		bool isString = false;
		std::string name;
		std::string valString;
		std::uint64_t valNumber;
	};

	class FileFormat {
	private:
		std::vector<FileData> mFileData;

		std::string mFileContents;

	public:
		std::vector<FileData>* operator()() { return &mFileData; }

		void addNumber(std::string name, std::uint64_t value) {
			mFileData.emplace_back(FileData());
			mFileData[mFileData.size() - 1].name = name;
			mFileData[mFileData.size() - 1].valNumber = value;
			mFileData[mFileData.size() - 1].isString = false;
		}

		void addString(std::string name, std::string value) {
			mFileData.emplace_back(FileData());
			mFileData[mFileData.size() - 1].name = name;
			mFileData[mFileData.size() - 1].valString = value;
			mFileData[mFileData.size() - 1].isString = true;
		}

		void saveData(std::string path) {
			mFileContents.clear();

			for (auto& data : mFileData) {
				if (!data.isString) {
					mFileContents += '\1' + data.name + '\0';

					for (int i = 0; i < 8; i++) {
						mFileContents += (std::uint8_t)((data.valNumber >> (i * 8)) & 0xff);
					}
				}
				else {
					mFileContents += '\2' + data.name + '\0' + data.valString + '\0';
				}
			}

			std::ofstream file(path, std::ios::binary);

			file.write(mFileContents.c_str(), mFileContents.size());

			file.close();
		}

		void loadData(std::string path) {
			mFileData.clear();
			mFileContents.clear();

			std::ifstream file(path, std::ios::binary | std::ios::ate);

			if (!file.is_open() || file.bad()) {
				std::cout << "Cannot open " << path << std::endl;

				return;
			}

			std::size_t length = file.tellg();
			file.seekg(0, std::ios::beg);

			mFileContents.resize(length);

			file.read((char*)mFileContents.data(), mFileContents.size());

			file.close();

			for (std::size_t i = 0; i < mFileContents.size(); i++) {
				FileData data;

				if (mFileContents[i] == 1) {
					// Value 64 bit
					data.name = std::string();
					data.valString = std::string();
					data.valNumber = 0;
					data.isString = false;

					i++;
					while (mFileContents[i] != 0) data.name += mFileContents[i++];

					for (int j = 0; j < 8; j++) {
						data.valNumber |= (((std::uint64_t)((std::uint8_t)mFileContents[++i]) << j * 8) & (0xffULL << j * 8));
					}
				}
				else if (mFileContents[i] == 2) {
					// String
					data.name = std::string();
					data.valString = std::string();
					data.valNumber = 0;
					data.isString = true;

					i++;
					while (mFileContents[i] != 0) data.name += mFileContents[i++];
					i++;
					while (mFileContents[i] != 0) data.valString += mFileContents[i++];
				}

				mFileData.emplace_back(data);
			}
		}
	};

	// Great concept down there, maybe I implement if better overtime
	// File data type
	/*enum FileDataType {
		// Nothing
		FileDataType_Null = 0,
		// std::string
		FileDataType_string = 3,

		// std::uint8_t
		FileDataType_u8 = 1,
		// std::uint16_t
		FileDataType_u16 = 2,
		// std::uint32_t
		FileDataType_u32 = 4,
		// std::uint64_t
		FileDataType_u64 = 8,
		
		// std::int8_t
		FileDataType_i8 = 0x10,
		// std::int16_t
		FileDataType_i16 = 0x20,
		// std::int32_t
		FileDataType_i32 = 0x40,
		// std::int64_t
		FileDataType_i64 = 0x80,
		
		// float
		FileDataType_f32 = 0x3f,
		// double 
		FileDataType_f64 = 0x7f,
	};

	struct FileData {
		// Type flag
		uint8_t dataType;
		// Variable name
		std::string dataName;

		// Data types (uints can be used also as ints and floats)
		std::string str;
		std::uint8_t u8;
		std::uint16_t u16;
		std::uint32_t u32;
		std::uint64_t u64;

		// Sets data for ease of use
		template<typename T>
		void SetData(const T& data, uint8_t desiredDataType) {
			// Set data based on desired data type
			switch (desiredDataType) {
			case FileDataType_string:
				str = data;
				// Zeros aren`t allowed in strings, other characters are fine
				while (str.find_first_of('/0') != std::string::npos)
					str.erase(str.begin() + str.find_first_of('/0'));
				dataType = desiredDataType;
				break;

			case FileDataType_u8:
				u8 = (std::uint8_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_u16:
				u16 = (std::uint16_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_u32:
				u32 = (std::uint32_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_u64:
				u64 = (std::uint64_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_i8:
				u8 = (std::uint8_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_i16:
				u16 = (std::uint16_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_i32:
				u32 = (std::uint32_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_i64:
				u64 = (std::uint64_t)data;
				dataType = desiredDataType;
				break;

			case FileDataType_f32:
				u32 = *reinterpret_cast<std::uint32_t*>((float*)&data);
				dataType = desiredDataType;
				break;

			case FileDataType_f64:
				u64 = *reinterpret_cast<std::uint64_t*>((double*)&data);
				dataType = desiredDataType;
				break;

			default:
				u8 = (std::uint8_t)data;
				dataType = FileDataType_u8;
				break;
			}
		}

		// Gets data for ease of use
		template<typename T>
		T GetData() {
			// Return data based on data type flag
			switch (dataType) {
			case FileDataType_string:
				return str;
				break;

			case FileDataType_u8:
				return u8;
				break;

			case FileDataType_u16:
				return u16;
				break;

			case FileDataType_u32:
				return u32;
				break;

			case FileDataType_u64:
				return u64;
				break;

			case FileDataType_i8:
				return (std::int8_t)u8;
				break;

			case FileDataType_i16:
				return (std::int16_t)u16;
				break;

			case FileDataType_i32:
				return (std::int32_t)u32;
				break;

			case FileDataType_i64:
				return (std::int64_t)u64;
				break;

			case FileDataType_f32:
				return *reinterpret_cast<float*>((std::uint32_t*)&u32);
				break;

			case FileDataType_f64:
				return *reinterpret_cast<double*>((std::uint64_t*)&u64);
				break;

			default:
				return u8;
				break;
			}
		}
	};
	
	class File {
	private:
		// entire file data converted into vector
		std::vector<FileData> mFileData;
		// file data converted/loaded to string
		std::string mFileDataString;

	public:
		// Adds new data with name <name> of value <data> of type <dataType> 
		// to data vector (make sure that name is unique)
		template<typename T>
		void operator()(std::uint8_t dataType, std::string name, T data) {
			FileData result;
			// Set data values
			result.dataName = name;
			result.SetData(data, dataType);

			// And add to vector
			mFileData.emplace_back(result);
		}

		// Delete data by name;
		void operator()(std::string varName) {
			// Search for data name (could be more efficient)
			for (std::size_t i = 0; i < mFileData.size(); i++) {
				// If data with name was found, return reference to it
				if (mFileData[i].dataName == varName) {
					mFileData.erase(mFileData.begin() + i);

					break;
				}
			}
		}

		// Get pointer to file data
		std::vector<FileData>* operator()() {
			return &mFileData;
		}

		// Returns value <varName> loaded from file
		FileData& operator[](std::string varName) {
			// Search for data name (could be more efficient)
			for (FileData& rData : mFileData) {
				// If data with name was found, return reference to it
				if (rData.dataName == varName) {
					return rData;
				}
			}

			// Error case
			FileData nonExistant;

			return nonExistant;
		}

		// Loads file from path <filename>
		bool loadFile(std::string filename) {
			// Clear garbage data as there could be some
			mFileDataString.clear();

			std::ifstream fileHandle;

			// Open file handle as input in binary and seek to end of file
			fileHandle.open(filename, std::ios::binary | std::ios::ate);

			if (!fileHandle.is_open() || fileHandle.bad()) return false;

			// Get file length
			std::size_t fileLength = fileHandle.tellg();
			// And seek to begining of file;
			fileHandle.seekg(0, std::ios::beg);

			// Resize file data string to fit entire file
			mFileDataString.resize(fileLength);

			// Read data to file data string
			fileHandle.read(mFileDataString.data(), mFileDataString.size());

			// File handle is unused by this point
			fileHandle.close();

			// Vector also must cleared from garbage data
			mFileData.clear();

			// Loop through every signle character to find data types
			// data format follow template: 
			// 1Bytes -> data type, NBytes -> data name, 0, NBytes -> value, (if data type == string) 0
			for (std::size_t i = 0; i < mFileDataString.size(); i++) {
				FileData currentlyReadData;

				switch (mFileDataString[i]) {
				case FileDataType_string:
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_string;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];


					// Read data value
					while (mFileDataString[i] != 0)
						currentlyReadData.str += mFileDataString[i++];

					break;

				case FileDataType_u8:
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_u8;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file
					currentlyReadData.u8 = (std::uint8_t)(mFileDataString[++i]);

					break;

				case FileDataType_u16:
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_u16;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u16 = ((std::uint16_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u16 |= ((std::uint16_t)((std::uint8_t)mFileDataString[++i])) << 8;

					break;

				case FileDataType_u32:
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_u32;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u32 = ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 8;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 16;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 24;

					break;

				case FileDataType_u64:
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_u64;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u64 = ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 8;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 16;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 24;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 32;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 40;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 48;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 56;

					break;

				case FileDataType_i8:
					// Calm down, I know we just change type here and reading from file
					// would save some code, but reason for this decision is below \/
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_i8;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file
					currentlyReadData.u8 = (std::uint8_t)(mFileDataString[++i]);

					break;

				case FileDataType_i16:
					// Calm down, I know we just change type here and reading from file
					// would save some code, but reason for this decision is below \/
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_i16;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u16 = ((std::uint16_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u16 |= ((std::uint16_t)((std::uint8_t)mFileDataString[++i])) << 8;

					break;

				case FileDataType_i32:
					// Calm down, I know we just change type here and reading from file
					// would save some code, but reason for this decision is below \/
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_i32;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u32 = ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 8;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 16;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 24;

					break;

				case FileDataType_i64:
					// Calm down, I know we just change type here and reading from file
					// would save some code, but reason for this decision is below \/
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_i64;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u64 = ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 8;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 16;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 24;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 32;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 40;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 48;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 56;

					break;

				case FileDataType_f32:
					// Calm down, I know we just change type here and reading from file
					// would save some code, but reason for this decision is below \/
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_f32;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u32 = ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 8;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 16;
					currentlyReadData.u32 |= ((std::uint32_t)((std::uint8_t)mFileDataString[++i])) << 24;

					break;

				case FileDataType_f64:
					// Calm down, I know we just change type here and reading from file
					// would save some code, but reason for this decision is below \/
					// Set data type to right type, for error correction use type from enum rather 
					// than file, tho we could teoreticly read it from file
					currentlyReadData.dataType = FileDataType_f64;
					// Advance index to skip data type
					i++;

					// Read data name
					while (mFileDataString[i] != 0)
						currentlyReadData.dataName += mFileDataString[i++];

					// We don`t need to advance '/0' as code below does this for us
					// and read byte from file, here we also must do this crazy conversion as
					// normal string have std::int8_t
					currentlyReadData.u64 = ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 0;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 8;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 16;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 24;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 32;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 40;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 48;
					currentlyReadData.u64 |= ((std::uint64_t)((std::uint8_t)mFileDataString[++i])) << 56;

					break;

				default:
					// Continue loop
					continue;
					break;
				}

				mFileData.emplace_back(currentlyReadData);
			}

			return true;
		}

		// Saves file to path <filename>
		void saveFile(std::string filename) {
			// Clear garbage data in save data string to write only desired 
			// data specified in vector
			mFileDataString.clear();

			// Run through all data and assign them correctly in save data string 
			// Values are little endian, string are big endian, quite understandable
			for (FileData& rData : mFileData) {
				mFileDataString += rData.dataType;
				mFileDataString += rData.dataName;
				// Strings are null terminated
				mFileDataString += '\0';

				// Check out data type
				switch (rData.dataType)
				{
				case FileDataType_u8:
				case FileDataType_i8:
					// Zero right shift just for ease of read
					mFileDataString += (std::uint8_t)((rData.u8 & 0xff) >> 0);
					break;

				case FileDataType_u16:
				case FileDataType_i16:
					// Zero right shift just for ease of read
					mFileDataString += (std::uint8_t)((rData.u16 & 0xff) >> 0);
					mFileDataString += (std::uint8_t)((rData.u16 & 0xff00) >> 8);
					break;

				case FileDataType_u32:
				case FileDataType_i32:
				case FileDataType_f32:
					// Zero right shift just for ease of read
					mFileDataString += (std::uint8_t)((rData.u32 & 0xff) >> 0);
					mFileDataString += (std::uint8_t)((rData.u32 & 0xff00) >> 8);
					mFileDataString += (std::uint8_t)((rData.u32 & 0xff0000) >> 16);
					mFileDataString += (std::uint8_t)((rData.u32 & 0xff000000) >> 24);
					break;

				case FileDataType_u64:
				case FileDataType_i64:
				case FileDataType_f64:
					// Zero right shift just for ease of read
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff) >> 0);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff00) >> 8);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff0000) >> 16);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff000000) >> 24);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff00000000) >> 32);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff0000000000) >> 40);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff000000000000) >> 48);
					mFileDataString += (std::uint8_t)((rData.u64 & 0xff00000000000000) >> 56);
					break;

				case FileDataType_string:
					mFileDataString += rData.str;
					// Strings are null terminated
					mFileDataString += '\0';

					//default:
						// There is no default statement but may be in future
					//	break;
				}
			}

			std::ofstream fileHandle;

			// Open (if deesn`t exist create) file handle as output in binary
			fileHandle.open(filename, std::ios::binary);

			// After adding all 1's and 0's write data to file
			fileHandle.write(mFileDataString.c_str(), mFileDataString.size());

			// File handle is useless after saving and needs to be closed
			fileHandle.close();
		}
	};*/
}

#endif