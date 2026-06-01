# Enduro_PIF

Recriação do clássico **Enduro** (Atari 2600) em C usando a biblioteca [raylib](https://www.raylib.com/).
O objetivo é ultrapassar a quantidade de carros exigida a cada dia, gerenciando a velocidade
e desviando dos adversários numa pista com perspectiva pseudo-3D.

Projeto desenvolvido para a disciplina de **Programação Imperativa e Funcional (PIF)**.

[Showcase no Youtube](https://youtu.be/KMFmuuaKFFM)

## Funcionalidades

- Pista com perspectiva pseudo-3D e curvas.
- Carros inimigos com colisão e hitboxes (modo debug disponível).
- Sistema de dias: a cada dia é preciso ultrapassar um número de carros para se classificar.
- Pontuação e ranking dos 5 melhores scores (persistido em disco).
- Efeitos sonoros: motor, vento e batida.
- Menu com controle de volume.

## Requisitos

- **GCC** (ou outro compilador C compatível)
- **make**
- **raylib** instalada no sistema
- Bibliotecas do sistema: `m`, `pthread`, `dl`, `rt` (geralmente já presentes em Linux)

### Instalando a raylib (Linux)

Arch / CachyOS:

```bash
sudo pacman -S raylib
```

Debian / Ubuntu:

```bash
sudo apt install libraylib-dev
```

Caso o pacote não exista na sua distro, compile a partir do
[repositório oficial da raylib](https://github.com/raysan5/raylib).

### Instalando no Windows (MSYS2 / MinGW)

1. Instale o [MSYS2](https://www.msys2.org/).
2. Abra o terminal **"MSYS2 MINGW64"** (não o "MSYS2 MSYS").
3. Instale o compilador, o make e a raylib:

   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib make
   ```

A partir daí, `make` e `make run` funcionam igual ao Linux — o `Makefile`
detecta o Windows automaticamente e linka com as bibliotecas corretas
(`opengl32`, `gdi32`, `winmm`), gerando `enduro.exe`.

> Sempre compile e execute pelo terminal **MINGW64**.

## Como compilar e executar

Clone o repositório e, na raiz do projeto:

```bash
# compila o jogo (gera o executável ./enduro)
make

# compila (se necessário) e executa
make run
```

Os assets (`assets/`) são carregados com caminhos relativos, então execute o jogo
a partir da raiz do projeto.

## Controles

| Tecla | Ação |
|-------|------|
| `↑` / `↓` (menu) | Navegar opções |
| `←` / `→` (menu, opção Volume) | Ajustar volume |
| `Enter` | Confirmar / iniciar corrida |
| `Espaço` | Acelerar |
| `↓` / `S` | Frear |
| `←` / `A` | Mover para a esquerda |
| `→` / `D` | Mover para a direita |
| `Backspace` | Apagar letra (tela de game over) |

## Estrutura do projeto

```
.
├── assets/        # imagens e efeitos sonoros (sfx/)
├── data/          # arquivos de dados gerados (ex.: ranking de scores)
├── include/       # headers (.h)
├── src/           # código-fonte (.c)
├── tests/         # testes
└── Makefile
```

| Módulo | Responsabilidade |
|--------|------------------|
| `game` | Máquina de estados (logo, título, menu, jogo, game over, ranking) |
| `player` | Movimento, velocidade e renderização do jogador |
| `track` | Geração e desenho da pista pseudo-3D |
| `enemy` | Carros inimigos |
| `collision` | Detecção de colisões e hitboxes |
| `score` | Pontuação e ranking persistido |
| `audio` | Efeitos sonoros (motor, vento, batida) e volume |
| `config` | Constantes de configuração (resolução, dimensões, etc.) |

## Limpeza

```bash
make clean
```
