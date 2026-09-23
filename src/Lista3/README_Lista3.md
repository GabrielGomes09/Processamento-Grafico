## Como compilar e executar

Pré-requisitos: CMake, MinGW (MSYS2 UCRT64) e Git instalados e no PATH.

```bash
cd build
cmake --build .
```

Os executavel é gerado dentro da pasta `build/`. Para rodar:

```bash
./Lista3_Ex.exe
```

## Exercício — Triângulos criados por clique do mouse

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
