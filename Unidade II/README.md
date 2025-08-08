# ⚡ Paralelização e Análise de Escalabilidade

---

## 🧵 Paralelização

No perfilamento realizado na etapa anterior, identificamos dois gargalos principais: as funções `write_the_file_content()` e `translate_file()`. Esses pontos impactavam diretamente o tempo total de execução, motivando a investigação de paralelização para otimizar o programa.

Durante a implementação, encontramos uma limitação importante no compressor:  
o tamanho dos blocos comprimidos é imprevisível, pois depende da repetição de padrões e da eficiência da compressão. Isso impede determinar antecipadamente os deslocamentos no arquivo comprimido para que múltiplas threads escrevam em paralelo suas partes, inviabilizando a divisão direta do problema.

Para contornar isso, optamos por paralelizar funções com menor impacto, porém de menor complexidade, que não exigissem grandes mudanças estruturais.  
Além disso, uma melhoria significativa foi implementada paralelizando a contagem de caracteres em múltiplos arquivos.  
Embora essa paralelização não acelere a contagem em um único arquivo grande, ela traz ganhos em cenários com vários arquivos.

Para implementar a paralelização, algumas adaptações foram necessárias:  
- Variáveis privadas para cada thread, evitando conflitos.  
- Uso de redução (`reduction`) para combinar resultados compartilhados de forma segura.  

Essas mudanças permitiram distribuir o processamento entre threads e melhorar a performance na compressão de múltiplos arquivos.

---

## 📈 Escalabilidade

A escalabilidade do código foi avaliada com a ferramenta **PasCal Analyzer** utilizando conjuntos de dados com diferentes tamanhos.  

Foram usados vídeos com resoluções relativas aos tamanhos:  
`1/8`, `1/4`, `1/2`, `1` e `2` (base `n`).

Testes foram realizados com configurações de número de threads:  
`1, 2, 4, 8, 16, 32, 64, 128`  

Cada configuração teve **3 repetições** e foi executada em um nó da partição **amd-512** do Núcleo de Processamento de Alto Desempenho (NPAD).

---

### 📋 Comando utilizado para análise

```bash
pascalanalyzer -c 1,2,4,8,16,32,64,128 -i 1_8.mp4,1_4.mp4,1_2.mp4,1.mp4,2.mp4 ./pipe -o pcd_huffman1283of.json -r 3
