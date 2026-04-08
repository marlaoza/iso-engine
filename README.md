# isoEngine

<p align="center">
  <img src="https://github.com/user-attachments/assets/b662c0e6-94dc-4695-bddd-97b8cf6646cc" alt="GIF mostrando particulas em um projétil">
</p>

Engine isometrica feita em c++ com SDL3.
O motor conta com renderização 100% feita por GPU com shaders, sistema de habilidades escalavel, sistema de UI dinamico e 9-SLICE, animações em tempo de frame e mais.

# Instalação
## WINDOWS
### Requisitos
Toolchain GPP
- Instale MSYS2 pra windows (https://www.msys2.org/).
Dentro do terminal dele, rode
`pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain` 
Adicione a pasta do UCRT nas variaveis de ambiente.

Shaders
- Aproveita que você já instalou o MSYS2 e baixe o GLSLANG com `pacman -S mingw-w64-x86_64-glslang`

### Compilação
Na raiz do projeto, digite
`Make`
o Makefile vai compilar as shaders e depois o projeto em um executavel main.exe

# Features
### Renderização
A engine é renderizada em GPU com metologia de dirty rendering, ou seja, os buffers relacionados a cada leva de objetos só são redesenhados quando alguma coisa realmente precisa mudar, evitando processar frames repetidos toda iteração. As entidades, UIs, partes do mapa, particulas e projeteis, por ter ou geometria ou dados de shader ou pipelines diferentes, tem cada um um Buffer próprio que armazena todos os quads de todos os objetos do mesmo tipo e desenha eles com uma chamada só pra GPU. A ordenação dos objetos de mapa (unidades, projeteis e etc) é feita com um depth buffer, uma imagem que determina a prioridade dos pixels baseado na profundidade passada a ela pelo shader de vertices, que é calculada pela posição do objeto no mapa (X + Y).

### UI
Os objetos de interface do usuário podem ser de alto nivel, containers principais e telas, esses são registrados na lista, ou podem ser filhos, o que permite uma lógica mais limpa de interação com os objetos. Eles também recebem um Sprite, que pode ser normal ou 9-slice, o que permite a interface ter um tamanho qualquer sem distorcer a pixel art.

### Particulas
As particulas utilizam uma feature do SDL chamada fragmentos, que permite que eu descreva um unido Quad mas envie informações diferentes pra esse mesmo quad, permitindo a instancia de muitas particulas sem pesar na renderização. Elas são modulares, recebendo valores de inicio e fim (cor, tamanho, etc) que são interpolados diretamente no shader.

### Habilidades
As habilidades são definidas por parâmetros em uma estrutura, o que permite descrever diversos efeitos que são calculados na hora de serem ativadas, elas descrevem uma posição alvo, um tamanho e recebem uma lista de efeitos, que são aplicados as entidades que o efeito tem como alvo dentro da área da habilidade base. Efeitos que movem alvos demonstram isso com um feedback chamado Linhas de Ação, que pinta o caminho que a unidade alvo vai percorrer.

