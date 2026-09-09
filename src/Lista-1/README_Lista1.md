# Lista de Exercícios 1 — Processamento Gráfico: Fundamentos

Introdução à OpenGL Moderna — Primitivas Gráficas, Shaders & Buffers

## Como compilar e executar

Pré-requisitos: CMake, MinGW (MSYS2 UCRT64) e Git instalados e no PATH.

```bash
cd build
cmake --build .
```

Os executáveis são gerados dentro da pasta `build/`. Para rodar cada um:

```bash
./Ex1.exe
./Ex2.exe
./Ex3.exe
./Ex4.exe
```

> Em alguns ambientes Windows é necessário copiar as DLLs de runtime do MinGW
> (`libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`, encontradas em
> `C:\msys64\ucrt64\bin`) para dentro da pasta `build`, caso o `.exe` feche
> sozinho sem abrir janela.

## Estrutura

```
src/Lista-1/Ex1/Ex1.cpp
src/Lista-1/Ex2/Ex2.cpp
src/Lista-1/Ex3/Ex3.cpp
src/Lista-1/Ex4/Ex4.cpp
```

## Exercício 1 — Dois triângulos (preenchido, contorno, pontos)

Desenha os dois triângulos do enunciado, trocando o modo de desenho em tempo real.

**Controles:**
- `1` — apenas preenchido
- `2` — apenas contorno
- `3` — apenas como pontos
- `4` — as 3 formas juntas
- `ESC` — sair

## Exercício 2 — Círculo e polígonos via equação paramétrica

Gera as formas usando a equação paramétrica do círculo (`x = cx + r·cos(θ)`,
`y = cy + r·sin(θ)`), variando o número de segmentos e o intervalo de ângulo.

**Controles:**
- `1` — círculo
- `2` — octógono
- `3` — pentágono
- `4` — pac-man
- `5` — fatia de pizza
- `ESC` — sair

## Exercício 3 — Triângulo com cor por vértice

Triângulo com P1 (topo) vermelho, P2 (esquerda) verde e P3 (direita) azul,
interpolados pela GPU entre os vértices.

**Respostas (itens 3a e 3b):**
- **3a)** Um único VBO com dados intercalados por vértice `[x, y, z, r, g, b]`
  para P1, P2 e P3. Um único VAO com 2 ponteiros de atributo apontando para
  esse mesmo VBO: um para posição (offset 0) e outro para cor (offset de 3
  floats), ambos com stride de 6 floats.
- **3b)** No vertex shader, os atributos são identificados via
  `layout (location = X) in ...`, onde `location = 0` é a posição e
  `location = 1` é a cor — os mesmos índices usados em
  `glVertexAttribPointer`/`glEnableVertexAttribArray` no código C++.

(Respostas completas comentadas no topo do arquivo `Ex3.cpp`.)

## Exercício 4 — Desenho no papel quadriculado (casa)

Exemplo de uma casa simples (corpo + telhado + porta), usando 3 VAOs
diferentes e 3 chamadas de desenho — como o enunciado permite. As coordenadas
usadas são apenas um exemplo; adapte-as ao seu próprio desenho.
