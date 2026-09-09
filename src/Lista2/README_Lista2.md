# Lista de Exercícios 2 — Processamento Gráfico: Fundamentos

Matriz de Projeção Ortográfica e mapeamento com a Viewport

## Como compilar e executar

Pré-requisitos: CMake, MinGW (MSYS2 UCRT64) e Git instalados e no PATH.

```bash
cd build
cmake --build .
```

Os executáveis são gerados dentro da pasta `build/`. Para rodar cada um:

```bash
./Lista2_Ex1.exe
./Lista2_Ex2.exe
./Lista2_Ex3.exe
./Lista2_Ex4.exe
./Lista2_Ex5.exe
./Lista2_Ex6.exe
```

> Em alguns ambientes Windows é necessário copiar as DLLs de runtime do MinGW
> (`libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`, encontradas em
> `C:\msys64\ucrt64\bin`) para dentro da pasta `build`, caso o `.exe` feche
> sozinho sem abrir janela.

## Estrutura

```
src/Lista2/Ex1/Ex1.cpp
src/Lista2/Ex2/Ex2.cpp
src/Lista2/Ex3/Ex3.cpp
src/Lista2/Ex4/Ex4.cpp
src/Lista2/Ex5/Ex5.cpp
src/Lista2/Ex6/Ex6.cpp
```

## Exercício 1 — Janela ortográfica (-10 a 10)

Configura a matriz de projeção com `glm::ortho(-10, 10, -10, 10, -1, 1)`,
tornando o mundo simétrico em torno da origem. Desenha um triângulo com
coordenadas dentro desse intervalo.

## Exercício 2 — Janela ortográfica em coordenadas de tela

Configura a janela do mundo com `xmin=0, xmax=800, ymin=600, ymax=0`
(`glm::ortho(0, 800, 600, 0, -1, 1)`), fazendo o eixo Y ficar invertido —
origem no canto superior esquerdo, igual ao sistema de coordenadas de tela
(pixels). O triângulo é posicionado diretamente em coordenadas de pixel.

## Exercício 3 — Desenho usando a câmera do Exercício 2

Usa a mesma configuração do Exercício 2 para desenhar um triângulo.

**Pergunta: o que acontece quando posicionamos os objetos? Por que é útil essa
configuração?**

Com essa janela, os objetos passam a ser posicionados diretamente em
coordenadas de tela (pixels), com origem no canto superior esquerdo e eixo Y
crescendo para baixo — igual ao sistema usado por janelas, imagens e
frameworks de UI. Isso é útil porque elimina a conversão manual entre "pixel
da tela" e "coordenada normalizada" (NDC): qualquer posição vinda da
interface (cliques do mouse, layout de HUD, sprites 2D) pode ser usada
diretamente como coordenada do mundo. É por isso que jogos 2D e sistemas de
UI costumam usar esse tipo de projeção ortográfica "screen-space".


## Exercício 4 — Viewport restrito a 1 quadrante

Mantém a cena original, mas restringe o `glViewport` para desenhar apenas no
quadrante superior direito da janela da aplicação:
```cpp
glViewport(width / 2, height / 2, width / 2, height / 2);
```

## Exercício 5 — Mesma cena nos 4 quadrantes

Desenha o mesmo triângulo 4 vezes, uma por quadrante, trocando o `glViewport`
antes de cada chamada de desenho dentro do loop principal.

## Exercício 6 — Triângulos criados por clique do mouse

**Controles:**
- Clique com o **botão esquerdo** do mouse: cria 1 vértice na posição do
  cursor.
- A cada 3 cliques, um triângulo é formado com uma **cor aleatória** nova.
- `ESC` — sair

A janela do mundo é configurada com o mesmo tamanho da tela (800x600), então
a posição do clique (em pixels) é usada diretamente como coordenada do mundo,
sem necessidade de conversão. A cor de cada triângulo é armazenada por
vértice (VBO intercalado posição + cor) e enviada dinamicamente para a GPU a
cada novo triângulo completado.
