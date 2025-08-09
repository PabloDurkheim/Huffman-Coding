//código de compilação: g++ -fopenmp -o pipe pipeline.cpp Compressor.cpp Descompressor.cpp
// ./pipe arquivo.extensao

#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
//#include "progress_bar.hpp"
#include <time.h>
#include "compressor.h"
#include "descompressor.h"
#include <omp.h>
#include <vector>
#include <bitset>

using namespace std;

bool erselcompare0(ersel a,ersel b){
    return a.number<b.number;
}

const char *file_path;

int compressor(int argc,char *argv[]){
    long int number[256];
    long int total_bits=0;
    int letter_count=0;
    
    file_path = argv[1];
    
    if(argc==1){
        cout<<"Missing file name"<<endl<<"try './archive {{file_name}}'"<<endl;
        return 0;
    }
    
    #pragma omp parallel for
    for(long int *i=number;i<number+256;i++){                       
        *i=0;
    }
    
    string scompressed;
    FILE *original_fp,*compressed_fp;

    for(int i=1;i<argc;i++){                    //checking for wrong input
        if(this_is_not_a_folder(argv[i])){
            original_fp=fopen(argv[i],"rb");
            if(!original_fp){
                cout<<" file does not exist"<<endl<<"Process has been terminated"<<endl;
                return 0;
            }
            fclose(original_fp);
        }
    }

    scompressed=argv[1];
    
    scompressed+=".compressed";


    //------------------1 and 2--------------------
        // This code block counts number of times that all of the unique bytes is used on the files and file names and folder names
        // and stores that info in 'number' array
            // after this code block, program checks the 'number' array
            //and writes the number of unique bytes count to 'letter_count' variable

    unsigned char *x_p, x;  // Variáveis temporárias para leitura do arquivo
    x_p = &x;

    long int total_size = 0, size;
    total_bits += 16 + 9 * (argc - 1);

    // Contagem de frequências dos bytes
    #pragma omp parallel for reduction(+:number[:256],total_size, total_bits) //reduction(+:total_size, total_bits)
    for (int current_file = 1; current_file < argc; current_file++) {
        
        int thread_id = omp_get_thread_num();
        //cout << "Thread: "<< thread_id << endl;
        
        // Variáveis locais para cada thread
        unsigned char local_x;
        unsigned char *local_x_p = &local_x; // Ponteiro local
        long int local_size = 0;
        FILE *local_original_fp = nullptr;

        for (char *c = argv[current_file]; *c; c++) {  
            number[(unsigned char)(*c)]++;
            //cout << "Number:" << number[(unsigned char)(*c)] << endl;
        }

        if (this_is_not_a_folder(argv[current_file])) {
            // Calcular o tamanho do arquivo
            local_size = size_of_the_file(argv[current_file]);
            total_size += local_size;
            total_bits += 64;
            
            cout << "Total de bits: " << total_bits << endl;// Processar os bytes do arquivo
            local_original_fp = fopen(argv[current_file], "rb");
            if (local_original_fp) {
                fread(local_x_p, 1, 1, local_original_fp);  // Leitura inicial
                for (long int j = 0; j < local_size; j++) {  
                    number[*local_x_p]++;
                    fread(local_x_p, 1, 1, local_original_fp);
                    //cout << "Number: "<< number[*local_x_p] << endl;
                }
                fclose(local_original_fp);
            }
        } else {
          
            string temp = argv[current_file];  // Caso seja uma pasta, contar os bytes dentro delargv[current_file];
            count_in_folder(temp, number, total_size, total_bits);
        }
    }

        #pragma omp parallel for reduction(+: letter_count)
	for(long int *i=number;i<number+256;i++){   
	
        	if(*i){
		  letter_count++;
		}
        }
    //---------------------------------------------



    //--------------------3------------------------
        // creating the base of translation array(and then sorting them by ascending frequencies
        // this array of type 'ersel' will not be used after calculating transformed versions of every unique byte
        // instead its info will be written in a new string array called str_arr 
    ersel array[letter_count*2-1];
    ersel *e=array;
    for(long int *i=number;i<number+256;i++){                         
        	if(*i){
                e->right=NULL;
                e->left=NULL;
                e->number=*i;
                e->character=i-number;
                e++;
            }
    }
    sort(array,array+letter_count,erselcompare0);
    //---------------------------------------------
    
                   
    
    //-------------------4-------------------------
        // min1 and min2 represents nodes that has minimum weights
        // isleaf is the pointer that traverses through leafs and
        // notleaf is the pointer that traverses through nodes that are not leafs
    ersel *min1=array,*min2=array+1,*current=array+letter_count,*notleaf=array+letter_count,*isleaf=array+2;            
    for(int i=0;i<letter_count-1;i++){                           
        current->number=min1->number+min2->number;
        current->left=min1;
        current->right=min2;
        min1->bit="1";
        min2->bit="0";     
        current++;
        
        if(isleaf>=array+letter_count){
            min1=notleaf;
            notleaf++;
        }
        else{
            if(isleaf->number<notleaf->number){
                min1=isleaf;
                isleaf++;
            }
            else{
                min1=notleaf;
                notleaf++;
            }
        }
        
        if(isleaf>=array+letter_count){
            min2=notleaf;
            notleaf++;
        }
        else if(notleaf>=current){
            min2=isleaf;
            isleaf++;
        }
        else{
            if(isleaf->number<notleaf->number){
                min2=isleaf;
                isleaf++;
            }
            else{
                min2=notleaf;
                notleaf++;
            }
        }
        
    }
        // At every cycle, 2 of the least weighted nodes will be chosen to
        // create a new node that has weight equal to sum of their weights combined.
            // After we are done with these nodes they will become childrens of created nodes
            // and they will be passed so that they wont be used in this process again.
    //---------------------------------------------


    
    //-------------------5-------------------------
    for(e=array+letter_count*2-2;e>array-1;e--){
        if(e->left){
            e->left->bit=e->bit+e->left->bit;
        }
        if(e->right){
            e->right->bit=e->bit+e->right->bit;
        }
        
    }
        // In this block we are adding the bytes from root to leafs
        // and after this is done every leaf will have a transformation string that corresponds to it
            // Note: It is actually a very neat process. Using 4th and 5th code blocks, we are making sure that
            // the most used character is using least number of bits.
                // Specific number of bits we re going to use for that character is determined by weight distribution
    //---------------------------------------------












    compressed_fp=fopen(&scompressed[0],"wb");
    int current_bit_count=0;
    unsigned char current_byte;
    //--------------writes first--------------
    fwrite(&letter_count,1,1,compressed_fp);
    total_bits+=8;
    //----------------------------------------



    //--------------writes second-------------
    {
        cout<<"If you want a password write any number other then 0"<<endl
            <<"If you do not, write 0"<<endl;
        int check_password = 0;
        //cin>>check_password;
        if(check_password){
            string password;
            cout<<"Enter your password (Do not use whitespaces): ";
            cin>>password;
            int password_length=password.length();
            if(password_length==0){
                cout<<"You did not enter a password"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                remove(&scompressed[0]);
                return 0;
            }
            if(password_length>100){
                cout<<"Password cannot contain more then 100 characters"<<endl<<"Process has been terminated"<<endl;
                fclose(compressed_fp);
                remove(&scompressed[0]);
                return 0;
            }
            unsigned char password_length_unsigned=password_length;
            fwrite(&password_length_unsigned,1,1,compressed_fp);
            fwrite(&password[0],1,password_length,compressed_fp);
            total_bits+=8+8*password_length;
        }
        else{
            fwrite(&check_password,1,1,compressed_fp);
            total_bits+=8;
        }
    }
        //Above code block puts password to compressed file
    //----------------------------------------







    



    //------------writes third---------------
    char *str_pointer;
    unsigned char len,current_character;
    string str_arr[256];
    for(e=array;e<array+letter_count;e++){
        str_arr[(e->character)]=e->bit;     //we are putting the transformation string to str_arr array to make the compression process more time efficient
        len=e->bit.length();
        current_character=e->character;

        write_from_uChar(current_character,current_byte,current_bit_count,compressed_fp);
        write_from_uChar(len,current_byte,current_bit_count,compressed_fp);
        total_bits+=len+16;
        // above lines will write the byte and the number of bits
        // we re going to need to represent this specific byte's transformated version
        // after here we are going to write the transformed version of the number bit by bit.
        
        str_pointer=&e->bit[0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Compression process aborted"<<endl;
                fclose(compressed_fp);
                remove(&scompressed[0]);
                return 1;
            }
           str_pointer++;
        }
        
         total_bits+=len*(e->number);
    }
    if(total_bits%8){
        total_bits=(total_bits/8+1)*8;        
        // from this point on total bits doesnt represent total bits
        // instead it represents 8*number_of_bytes we are gonna use on our compressed file
    }
    // Above loop writes the translation script into compressed file and the str_arr array
    //----------------------------------------


    cout<<"The size of the sum of ORIGINAL files is: "<<total_size<<" bytes"<<endl;
    cout<<"The size of the COMPRESSED file will be: "<<total_bits/8<<" bytes"<<endl;
    cout<<"Compressed file's size will be [%"<<100*((float)total_bits/8/total_size)<<"] of the original file"<<endl;
    if(total_bits/8>total_size){
        cout<<endl<<"COMPRESSED FILE'S SIZE WILL BE HIGHER THAN THE SUM OF ORIGINALS"<<endl<<endl;
    }
    cout<<"If you wish to abort this process write 0 and press enter"<<endl
        <<"If you want to continue write any other number and press enter"<<endl;
    int check = 1;
    //cin>>check;
    if(!check){
        cout<<endl<<"Process has been aborted"<<endl;
        fclose(compressed_fp);
        remove(&scompressed[0]);
        return 0;
    }



    
    //-------------writes fourth---------------
    write_file_count(argc-1,current_byte,current_bit_count,compressed_fp);
    //---------------------------------------
    
    int escrita_final;
    
    //cada arquivo pode ser processado de forma independente.
    //#pragma omp parallel for
    for(int current_file=1;current_file<argc;current_file++){
        //int thread_id = omp_get_thread_num();
        //cout << "Thread: "<< thread_id << endl;
        if(this_is_not_a_folder(argv[current_file])){   //if current is a file and not a folder
            original_fp=fopen(argv[current_file],"rb");
            fseek(original_fp,0,SEEK_END);
            size=ftell(original_fp);
            rewind(original_fp);

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            
            current_byte<<=1;
            current_byte|=1;
            current_bit_count++;
            //---------------------------------------
            
            {     
            write_file_size(size,current_byte,current_bit_count,compressed_fp);             //writes sixth
            write_file_name(argv[current_file],str_arr,current_byte,current_bit_count,compressed_fp);   //writes seventh
            cout << "Ftell: " << ftell(compressed_fp) << endl;
            write_the_file_content(original_fp,size,str_arr,current_byte,current_bit_count,compressed_fp, ftell(compressed_fp), escrita_final);      //writes eighth
            }
            fclose(original_fp);
        }
        else{   //if current is a folder instead

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_bit_count++;
            //---------------------------------------
            
           //#pragma omp critical
            {
            write_file_name(argv[current_file],str_arr,current_byte,current_bit_count,compressed_fp);   //writes seventh

            string folder_name=argv[current_file];
            write_the_folder(folder_name,str_arr,current_byte,current_bit_count,compressed_fp);
            }
        }
    }



    //posiciona corretamente após a última escrita da última thread na função write_the_file_content
    fseek(compressed_fp, escrita_final+1, SEEK_SET);

    if(current_bit_count==8){      // here we are writing the last byte of the file
        fwrite(&current_byte,1,1,compressed_fp);
        cout << "passei por aqui";
    }
    else{
        current_byte<<=8-current_bit_count;
        fwrite(&current_byte,1,1,compressed_fp);
    }

    fclose(compressed_fp);
    //system("clear");
    cout<<endl<<"Created compressed file: "<<scompressed<<endl;
    cout<<"Compression is complete"<<endl;
    
    return 0;
}



//below function is used for writing the uChar to compressed file
    //It does not write it directly as one byte instead it mixes uChar and current byte, writes 8 bits of it 
    //and puts the rest to curent byte for later use
void write_from_uChar(unsigned char uChar,unsigned char &current_byte,int current_bit_count,FILE *fp_write){
    current_byte<<=8-current_bit_count;
    current_byte|=(uChar>>current_bit_count);
    fwrite(&current_byte,1,1,fp_write);
    current_byte=uChar;   
}



//below function is writing number of files we re going to translate inside current folder to compressed file's 2 bytes
    //It is done like this to make sure that it can work on little, big or middle-endian systems
void write_file_count(int file_count,unsigned char &current_byte,int current_bit_count,FILE *compressed_fp){
    unsigned char temp=file_count%256;
    write_from_uChar(temp,current_byte,current_bit_count,compressed_fp);
    temp=file_count/256;
    write_from_uChar(temp,current_byte,current_bit_count,compressed_fp);
}



//This function is writing byte count of current input file to compressed file using 8 bytes
    //It is done like this to make sure that it can work on little, big or middle-endian systems
void write_file_size(long int size,unsigned char &current_byte,int current_bit_count,FILE *compressed_fp){
   
    for(int i=0;i<8;i++){
        write_from_uChar(size%256,current_byte,current_bit_count,compressed_fp);
        size/=256;
    }
}



// This function writes bytes that are translated from current input file's name to the compressed file.
void write_file_name(char *file_name,string *str_arr,unsigned char &current_byte,int &current_bit_count,FILE *compressed_fp){
    write_from_uChar(strlen(file_name),current_byte,current_bit_count,compressed_fp);
    char *str_pointer;
    for(char *c=file_name;*c;c++){
        str_pointer=&str_arr[(unsigned char)(*c)][0];
        while(*str_pointer){
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            switch(*str_pointer){
                case '1':current_byte<<=1;current_byte|=1;current_bit_count++;break;
                case '0':current_byte<<=1;current_bit_count++;break;
                default:cout<<"An error has occurred"<<endl<<"Process has been aborted";
                exit(2);
            }
            str_pointer++;
        }
    }
}

//adicionei mais um parametro que é "atualizar_escrita", tem o valor da última escrita antes de chamar essa função
void write_the_file_content(FILE *original_fp, long int size, string *str_arr, unsigned char &current_byte, int &current_bit_count, FILE *compressed_fp, int atualizar_escrita, int &escrita_final) {
    unsigned char *x_p, x;
    x_p = &x;
    char *str_pointer;
    //fread(x_p, 1, 1, original_fp); 
    
    //------------------LEITURA-----------------
    
    int num_de_thread; //numero de threads
    
    long int fim_de_cada_thread[omp_get_max_threads()];
    
    long int bits_restantes[omp_get_max_threads()];
    long int bit_atual[omp_get_max_threads()];
    int bit_aux = 0;
    
    int soma_aux = current_bit_count;
    int contador = 1;
    int indice_da_thread = 0;
    int espacamento;
    int ultima_escrita;
    
    //estabecelendo o número de threads necessárias e o espaçamento entre elas
    if( size > omp_get_max_threads() && (size % omp_get_max_threads() == 0) ){
        //caso em que o espaçamento fica igual em todas as threads
        espacamento = size / omp_get_max_threads();
        num_de_thread = omp_get_max_threads();
    }else if( size > omp_get_max_threads() && (size % omp_get_max_threads() != 0) ){
        //caso em que o espaçamento fica diferente: precisa somar 1 porque iria faltar iterações finais no que deveria ser a última thread
        //as iterações seriam distribuídas em outras threads e bagunçaria tudo, exemplo: thread 0: i = 0, 1, 2, 31, 32. Não queremos isso
        //somando 1, todas as thread ficam com o espaçamento igual com exceção da última que ficaria com o restante das iterações mas TUDO em ORDEM
        espacamento = size / omp_get_max_threads() + 1;
        num_de_thread = ( size / espacamento ) + 1;
    }else{
        //estabeleci um padrão caso o arquivo seja muito pequeno
        espacamento = 3; 
        num_de_thread = 6;
    }
    
    //--------------------ESCRITA-----------------
    
    int qtd_de_escrita_por_thread[omp_get_max_threads()];
    long int posicao_final_de_escrita_por_thread[omp_get_max_threads()];
    int qtd_de_escrita = 0;
    int indice_aux = 0;
    
    cout << "current_byte: " << bitset<8>(current_byte) << " | current_bit_count: " << current_bit_count << endl;
    cout << endl << "Tenho: " << num_de_thread << "threads e o espaço é: " << espacamento << endl;
    
    for(long int i = 0; i < size; i++){
      fread(x_p, 1, 1, original_fp);
      str_pointer = &str_arr[x][0];
      
      //soma os bits caracter por caracter
      soma_aux += strlen(str_pointer);
      
      //se passou de 8 bits é porque escreveu
      if(soma_aux > 8){
        //precisa desse while por um caracter pode ter mais de 8 bits, tipo os de imagens
        while(soma_aux >= 8){
          soma_aux -= 8;
          ultima_escrita = i; //atualiza a última escrita
          qtd_de_escrita++;
        }
        bit_aux = soma_aux; //pego o bit que restou
        
        //cout << "próxima escrita: " << ultima_escrita << " | soma_aux: " << soma_aux << endl;
      }else if(soma_aux % 8 == 0){
        ultima_escrita = i+1;
        
        soma_aux -= 8;
        qtd_de_escrita++;
        bit_aux = 0;
        
      }
      
      //salva os bits restantes e o valor simbólico do caracter que parou a escrita
      //esse IF representa o fim de cada thread, ele atualiza tudo que precisa
      if(  (i+1) % espacamento == 0 || (i+1) == size ){
        //parte de leitura
        bits_restantes[indice_da_thread] = soma_aux;
        fim_de_cada_thread[indice_da_thread] = ultima_escrita; //salva a representação do byte onde a próxima thread teoricamente deveria começar
        bit_atual[indice_da_thread] = bit_aux; //salva o bit atual restante
        indice_da_thread++;
        
        //parte de escrita
        qtd_de_escrita_por_thread[indice_aux] = qtd_de_escrita;
        
        indice_aux++;
        
        //atualiza a primeira posição, usa o parâmetro novo adiciona na função
        if(qtd_de_escrita_por_thread[0] != 1){
          posicao_final_de_escrita_por_thread[0] = atualizar_escrita + qtd_de_escrita_por_thread[0]; //caso em que a 1ª thread escreve mais de uma vez
        }else{
          posicao_final_de_escrita_por_thread[0] = atualizar_escrita; //caso em que a 1ª thread escreva apenas uma vez;
        }
        
        //evita que compute o primeiro valor que corresponde a primeira thread, só interessa os outros
        if( (i+1) > espacamento ){
          posicao_final_de_escrita_por_thread[indice_aux-1] = posicao_final_de_escrita_por_thread[indice_aux-2] + qtd_de_escrita;
        }
        
        cout << "Posição final de escrita: " << posicao_final_de_escrita_por_thread[indice_aux-1] << " qtd: " << qtd_de_escrita << endl;
        qtd_de_escrita = 0;
        
      }
      
    }
    
    for(long int i = 0; i < num_de_thread; i++){
      cout << "Sou a thread " << i << " | próxima escrita: " << fim_de_cada_thread[i] << " | sobrou: " << bits_restantes[i] << " bits | bit restante atual: " << bit_atual[i] << endl;
      cout << "Eu escrevo: " << qtd_de_escrita_por_thread[i] << " vezes e termino em: " << posicao_final_de_escrita_por_thread[i] << endl; 
    }
    
    cout << endl;
    
    //-----volta para origem-----------
    rewind(original_fp);
    
    bool buscar_bits = true;
    int contagem_dos_bits = current_bit_count;
    unsigned char current_byte_teste = current_byte;
    
    int posicao_escrita;
    
    bool primeira_thread = false;
    int contagem_das_threads = 0;
    
    #pragma omp parallel for private(posicao_escrita) firstprivate(buscar_bits, contagem_dos_bits, current_byte_teste, str_arr, original_fp, compressed_fp) \
    schedule(static, espacamento) shared(primeira_thread, contagem_das_threads)
    for(long int i = 0; i < size; i++){
      unsigned char *local_x_p, local_x;
      local_x_p = &local_x;
      char *local_str_pointer;
      
      //faz a verificaçao se a thread anterior deixou bits restantes
      if(buscar_bits){
        
        //-----------------PARTE DE ESCRITA---------------------
        
        if(i != 0){
          posicao_escrita = posicao_final_de_escrita_por_thread[omp_get_thread_num()-1]++;
          contagem_dos_bits = 0;
        }else{
          //---------IMPORTANTE!!!-----
          posicao_escrita = atualizar_escrita; //atualiza a posição de escrita para a última escrita antes de chamar essa função
          
        }
        
        //-----------------PARTE DE LEITURA---------------------
        
        //só pego bits anteriores se a thread anterior deixou bits, caso contrário não faço
        if( ( omp_get_thread_num() != 0) && bits_restantes[omp_get_thread_num()-1] != 0){ 
        
          //posição do caracter que deverá começar
          int id_posicional = fim_de_cada_thread[omp_get_thread_num()-1];
          bool deslocar = false;
          
          //ando caracter por caracter até chegar no atual
          while(id_posicional < i){
          
            #pragma omp critical
            {
            fseek(original_fp, id_posicional , SEEK_SET);
            fread(local_x_p, 1, 1, original_fp);
            local_str_pointer = &str_arr[local_x][0];
            
            //deslocar bits do caracter caso ele não inicio do começo
            if(!deslocar && bit_atual[omp_get_thread_num()-1] != 0){
                local_str_pointer += strlen(local_str_pointer) - bit_atual[omp_get_thread_num()-1];
                deslocar = true;
              }
            }
            
            while (*local_str_pointer){
              
              switch (*local_str_pointer) {
                case '1':
                    current_byte_teste <<= 1; // Desloca para a esquerda
                    current_byte_teste |= 1;  // Adiciona 1 no bit menos significativo
                    contagem_dos_bits++;
                    break;
                case '0':
                    current_byte_teste <<= 1; // Desloca para a esquerda, adicionando 0
                    contagem_dos_bits++;
                    break;
                default:
                    cout << "---------- Ocorreu um erro, leu o bit errado ao deslocar --------" << endl;
                    cout <<"Thread: " << omp_get_thread_num() << " | i: " << i << endl;
                    cout << x << endl;
                    exit(2); // Sai com erro
              }
              local_str_pointer++;
            }
            id_posicional++;
          }
        
        }
        
        buscar_bits = false;
        //#pragma omp critical
        {
        //fseek(original_fp, i, SEEK_SET);
        //cout << "Sou a thread: " << omp_get_thread_num() << " | escrevi os bits restantes: " << bitset<8>(current_byte_teste) << " | i: " << i << endl;
        }
      }
      
      //depois de buscar bits restantes, precisa apontar para a posição atual novamente
      #pragma omp critical
      {
        fseek(original_fp, i, SEEK_SET);
        fread(local_x_p, 1, 1, original_fp);
        local_str_pointer = &str_arr[local_x][0];
        //str_pointer = &str_arr[x][0];
        //cout << "Sou a thread: " << omp_get_thread_num() << " | estou em: " << x << " | e meu i: "<< i << endl << endl;
      }
      
      while (*local_str_pointer){
            
              switch (*local_str_pointer) {
                case '1':
                    current_byte_teste <<= 1; // Desloca para a esquerda
                    current_byte_teste |= 1;  // Adiciona 1 no bit menos significativo
                    contagem_dos_bits++;
                    break;
                case '0':
                    current_byte_teste <<= 1; // Desloca para a esquerda, adicionando 0
                    contagem_dos_bits++;
                    break;
                default:
                    cout << "--------- Ocorreu um erro o processo foi abortado ----------" << endl;
                    exit(2); // Sai com erro
              }
                
                if (contagem_dos_bits == 8) {
                         
                      #pragma omp critical
                      { 
                       fseek(compressed_fp, posicao_escrita, SEEK_SET);
                       fwrite(&current_byte_teste, 1, 1, compressed_fp);
                       contagem_dos_bits = 0;
                       //cout << "Sou a thread: " << omp_get_thread_num() << " | escrevi: " << bitset<8>(current_byte_teste) << " | posicao: " << posicao_escrita << " Opa:"<< todas_thread << endl;
                       posicao_escrita++;
                             
                      }     
                         
                  }
                  local_str_pointer++;
                
                //#pragma omp critical
              {
                //cout << "Sou a thread: " << omp_get_thread_num() << " | iteracao: " << i << " | escrevi: " << bitset<8>(current_byte_teste) << " | estou em: " << x << " | bits: " << contagem_dos_bits << endl;
              }
                
        }
      /*
      if(i > 474100)        //cout << "I: " << i << " | current_byte: " << bitset<8>(current_byte_teste) << " | current_bit_count: " << contagem_dos_bits << endl;
      }
      */
      
      //verifica se as thread acabaram, menos a última
      if( i>0 && (i+1 == (omp_get_thread_num()+1)*espacamento ) && (omp_get_thread_num() != (num_de_thread-1) )){
        contagem_das_threads++;
        cout << "----- " << contagem_das_threads << " --- Thread: " << omp_get_thread_num() << " -----" << endl; 
      }
      
      //atualizando as variaveis que são passada por referência na função
      if( (i+1) == size ){
        current_byte = current_byte_teste;
        current_bit_count = contagem_dos_bits;
        
        //existe uma escrita após a chamada dessa função, tem que salvar a posição da última escrita
        escrita_final = posicao_escrita;
        cout << "current_byte: " << bitset<8>(current_byte) << " | current_bit_count: " << current_bit_count << endl;
        cout << "-------------- ESTOU NA THREAD: " << omp_get_thread_num() << endl;
  
      }
      
    }
    
    //rewind(original_fp);
    
}


int this_is_not_a_folder(char *path){
    DIR *temp=opendir(path);
    if(temp){
        closedir(temp);
        return 0;
    }
    return 1;
}

long int size_of_the_file(char *path){
    long int size;
    FILE *fp=fopen(path,"rb");
    fseek(fp,0,SEEK_END);
    size=ftell(fp);
    fclose(fp);
    return size;
}



// This function counts usage frequency of bytes inside a folder
    // only give folder path as input
void count_in_folder(string path,long int *number,long int &total_size,long int &total_bits){
    FILE *original_fp;
    path+='/';
    DIR *dir=opendir(&path[0]),*next_dir;
    string next_path;
    total_size+=4096;
    total_bits+=16; //for file_count
    struct dirent *current;
    while((current=readdir(dir))){
        if(current->d_name[0]=='.'){
            if(current->d_name[1]==0)continue;
            if(current->d_name[1]=='.'&&current->d_name[2]==0)continue;
        }
        total_bits+=9;

        for(char *c=current->d_name;*c;c++){        //counting usage frequency of bytes on the file name (or folder name)
            number[(unsigned char)(*c)]++;
        }

        next_path=path+current->d_name;

        if((next_dir=opendir(&next_path[0]))){
            closedir(next_dir);
            count_in_folder(next_path,number,total_size,total_bits);
        }
        else{
            long int size;
            unsigned char *x_p,x;
            x_p=&x;
            total_size+=size=size_of_the_file(&next_path[0]);
            total_bits+=64;

            //--------------------2------------------------
            original_fp=fopen(&next_path[0],"rb");

            fread(x_p,1,1,original_fp);
            for(long int j=0;j<size;j++){    //counting usage frequency of bytes inside the file
                number[x]++;
                fread(x_p,1,1,original_fp);
            }
            fclose(original_fp);
        }
    }
    closedir(dir);
}







void write_the_folder(string path,string *str_arr,unsigned char &current_byte,int &current_bit_count,FILE *compressed_fp){
    
    FILE *original_fp;
    path+='/';
    DIR *dir=opendir(&path[0]),*next_dir;
    string next_path;
    struct dirent *current;
    int file_count=0;
    long int size;
    while((current=readdir(dir))){
        if(current->d_name[0]=='.'){
            if(current->d_name[1]==0)continue;
            if(current->d_name[1]=='.'&&current->d_name[2]==0)continue;
        }
        file_count++;
    }
    rewinddir(dir);
    write_file_count(file_count,current_byte,current_bit_count,compressed_fp);  //writes fourth

    while((current=readdir(dir))){  //if current is a file
        if(current->d_name[0]=='.'){
            if(current->d_name[1]==0)continue;
            if(current->d_name[1]=='.'&&current->d_name[2]==0)continue;
        }

        next_path=path+current->d_name;
        if(this_is_not_a_folder(&next_path[0])){

            original_fp=fopen(&next_path[0],"rb");
            fseek(original_fp,0,SEEK_END);
            size=ftell(original_fp);
            rewind(original_fp);

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_byte|=1;
            current_bit_count++;
            //---------------------------------------
            
            int escrita_final;
           
            write_file_size(size,current_byte,current_bit_count,compressed_fp);                     //writes sixth
           
            write_file_name(current->d_name,str_arr,current_byte,current_bit_count,compressed_fp);                //writes seventh
            cout << "write_the_folder - Posição de escrita: " << ftell(compressed_fp);
            write_the_file_content(original_fp,size,str_arr,current_byte,current_bit_count,compressed_fp, ftell(compressed_fp), escrita_final);      //writes eighth
            fclose(original_fp);
        }
        else{   // if current is a folder

            //-------------writes fifth--------------
            if(current_bit_count==8){
                fwrite(&current_byte,1,1,compressed_fp);
                current_bit_count=0;
            }
            current_byte<<=1;
            current_bit_count++;
            //---------------------------------------

            write_file_name(current->d_name,str_arr,current_byte,current_bit_count,compressed_fp);   //writes seventh

            write_the_folder(next_path,str_arr,current_byte,current_bit_count,compressed_fp);
        }
    }
    closedir(dir);


}
