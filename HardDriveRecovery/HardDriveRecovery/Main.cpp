#include <windows.h>
#include <cstdlib>
#include <iostream>

#define CHUNK_SIZE 65536

void SetCorruptFilePointer(HANDLE hdisk, std::size_t chunkPosition);
void RecoverMp3(BYTE *data, std::size_t length);

int main() {
	std::size_t chunkPosition = 0;
	BYTE buf[65536];
	DWORD read;
	for (;;) {
		HANDLE hdisk = CreateFile("\\\\.\\C:",
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			0, NULL);
		SetCorruptFilePointer(hdisk, chunkPosition);
		if (!ReadFile(hdisk, buf, CHUNK_SIZE, &read, nullptr)) {
			break;
		}
		for (std::size_t index = 0; index < CHUNK_SIZE; ++index) {
			if (buf[index] == 0x49 && buf[index + 1] == 0x44 && buf[index + 2] == 0x33 && buf[index + 3] == 0x03) {
				RecoverMp3(&buf[index], CHUNK_SIZE - index);
			}
		}
		++chunkPosition;
		CloseHandle(hdisk);
	}
}

void SetCorruptFilePointer(HANDLE hdisk, std::size_t chunkPosition) {
	BYTE buf[CHUNK_SIZE];
	DWORD read;
	for (std::size_t index = 0; index < chunkPosition; ++index) {
		ReadFile(hdisk, buf, CHUNK_SIZE, &read, nullptr);
	}
}

void RecoverMp3(BYTE* data, std::size_t length) {
	for (std::size_t index = 0; index < 4096; ++index) {
		std::cout << std::hex << (unsigned int)data[index];
	}
}
