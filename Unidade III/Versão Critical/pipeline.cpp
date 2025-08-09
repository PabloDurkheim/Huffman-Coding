//código de compilação: g++ -fopenmp -o pipe pipeline.cpp Compressor.cpp Descompressor.cpp
// ./pipe arquivo.extensao

#include "compressor.h"
#include "descompressor.h"
#include <string>
#include <iostream>

int main(int argc,char *argv[]){
  
  /*
  for (int i = 0; i < argc; ++i) {
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
    }
  */
  std::cout << argc;
  compressor(argc, argv);
  
  descompressor(argc, argv);

  return 0;
}

