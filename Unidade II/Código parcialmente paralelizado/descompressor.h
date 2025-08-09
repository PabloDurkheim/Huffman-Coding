#ifndef DESCOMPRESSOR_H
#define DESCOMPRESSOR_H
#include <string>

struct translation{
    translation *zero,*one;
    unsigned char character;
};

int descompressor(int argc,char *argv[]);

bool this_is_a_file(unsigned char&,int&,FILE*);
long int read_file_size(unsigned char&,int,FILE*);
void write_file_name(char*,int,unsigned char&,int&,translation*,FILE*);
void translate_file(char*,long int,unsigned char&,int&,translation*,FILE*);
void translate_folder(std::string,unsigned char&,int&,FILE*,translation*);


unsigned char process_8_bits_NUMBER(unsigned char&,int,FILE*);
void process_n_bits_TO_STRING(unsigned char&,int,int&,FILE*,translation*,unsigned char);

bool file_exists(char*);
void change_name_if_exists(char*);

void burn_tree(translation*);

#endif
