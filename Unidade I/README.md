### ⚙️ Estrutura e Funcionamento do Código

O código completo consiste em **dois programas independentes**: um para **compressão** e outro para **descompressão** de arquivos.  
Para este trabalho, foi criado um **pipeline** que executa ambos em sequência, de forma que o arquivo é comprimido e, em seguida, descomprimido.  
O objetivo dessa junção foi facilitar a análise do código de forma completa.

### 📏 Especificações do Problema

O arquivo utilizado na aplicação foi um vídeo que, inicialmente, possuía resolução 1280 x 720 e foi, posteriormente, ajustado para outra resolução, com o objetivo de facilitar a mensuração do tamanho do problema, o qual foi definido como sendo a quantidade de pixels presentes em cada frame. Além disso, foi adotado o mesmo tempo de duração para todos os vídeos, variando apenas a quantidade de pixels em cada frame. Para facilitar a definição das resoluções correspondentes a cada tamanho de problema, o tamanho do vídeo foi redefinido de modo que a nova resolução resultasse em um vídeo quadrado. A nova resolução do vídeo base foi definida como 1000 x 1000 pixels. Esse vídeo inicial possui n pixels por frame, sendo n = 1000 x 1000 = 1000000 pixels.

Para os outros tamanhos de problema, a resolução foi determinada de forma simples: calculamos o número total de pixels desejado e extraímos a raiz quadrada desse valor. Dessa forma, obtivemos a altura e a largura da nova resolução, mantendo a proporção quadrada do vídeo. Por exemplo, para um problema de tamanho n/4, onde o número total de pixels é de 250000, a resolução do vídeo seria de 500 x 500. Vale ressaltar que, para alguns tamanhos de problema a altura e a largura resultam em quantidades de pixels aproximadas do valor real.


### 🔍 Perfilamento

O perfilamento foi realizado com o **Gprof** para identificar o tempo gasto em cada função.  
Foram testados diferentes tamanhos de problema:

`n/8, n/4, n/2, n, 2n, 4n, 8n`, sendo `n` o tamanho base.

**Comando de compilação para perfilamento:**
```bash
g++ -pg -fno-builtin -o pipe pipeline.cpp Compressor.cpp Descompressor.cpp
```
**Execução:**
```bash
./pipe um_meio.mp4
```
**Visualizar o relatório de perfilamento:**
```bash
gprof ./pipe | less
```
### 🏎️ Vetorização

A vetorização é uma técnica de otimização em que várias operações são executadas simultaneamente em um único comando, aproveitando as instruções de processamento vetorial do hardware.
O código foi compilado com autovetorização ativada, permitindo que o compilador identifique automaticamente os laços que podem ser vetorizados.
Os arquivos Compressor.cpp e Descompressor.cpp foram compilados separadamente para facilitar a visualização dos laços que não puderam ser vetorizados.

**Comandos de compilação com vetorização::**
```bash
g++ -O3 -ftree-vectorize -fopt-info-vec-missed -c Compressor.cpp -o Compressor.o
g++ -O3 -ftree-vectorize -fopt-info-vec-missed -c Descompressor.cpp -o Descompressor.o
