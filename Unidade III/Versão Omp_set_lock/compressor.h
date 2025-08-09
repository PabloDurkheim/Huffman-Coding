#ifndef COMPRESSOR_H
#define COMPRESSOR_H
#include <string>

struct ersel{   //this structure will be used to create the translation tree
    ersel *left,*right;
    long int number;
    unsigned char character;
    std::string bit;
};

bool erselcompare0(ersel a,ersel b);

int compressor(int argc,char *argv[]);

void write_from_uChar(unsigned char,unsigned char&,int,FILE*);

int this_is_not_a_folder(char*);
long int size_of_the_file(char*);
void count_in_folder(std::string,long int*,long int&,long int&);

void write_file_count(int,unsigned char&,int,FILE*);
void write_file_size(long int,unsigned char&,int,FILE*);
void write_file_name(char*,std::string*,unsigned char&,int&,FILE*);
void write_the_file_content(FILE*,long int,std::string*,unsigned char&,int&,FILE*, int, int&);
void write_the_folder(std::string,std::string*,unsigned char&,int&,FILE*);


#endif
