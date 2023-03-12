#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <bitset>
using std::cout;
using std::string;
using std::bitset;

int main()
{
    FILE* fileptr;
    unsigned char* buffer;
    long filelen;

    fileptr = fopen("listing41", "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (unsigned char*)malloc(filelen * sizeof(unsigned char)); // Enough memory for the file
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file


    FILE* fileptr2;
    unsigned char* buffer2;
    long filelen2;

    fileptr2 = fopen("out", "rb");  // Open the file in binary mode
    fseek(fileptr2, 0, SEEK_END);          // Jump to the end of the file
    filelen2 = ftell(fileptr2);             // Get the current byte offset in the file
    rewind(fileptr2);                      // Jump back to the beginning of the file

    buffer2 = (unsigned char*)malloc(filelen2 * sizeof(unsigned char)); // Enough memory for the file
    fread(buffer2, filelen2, 1, fileptr2); // Read in the entire file
    fclose(fileptr2); // Close the file

    bool error = false;
    if (filelen != filelen2) {
        cout << "file lengths were different" << filelen << " vs " << filelen2 << std::endl;
        error = true;
    }
    //else {
        for (int i = 0; i < filelen; ++i) {
            if (buffer[i] != buffer2[i]) {
                cout << "files different at byte " << i << ": " << bitset<8>(buffer[i]).to_string() << " <> " << bitset<8>(buffer2[i]).to_string() << std::endl;
                error = true;
                break;
            }
        }
    //}
    if (!error) {
        cout << "files are equal";
    }
}
