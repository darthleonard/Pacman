#include <allegro.h>
#include <cstdlib> // permite usar la funcion rand()

#define MAXFILAS 20
#define MAXCOLS 31

BITMAP *buffer;
BITMAP *roca;
BITMAP *pacbmp;
BITMAP *pacman;
BITMAP *comida;
BITMAP *muertebmp;

MIDI *musica1;
SAMPLE *bolita;
SAMPLE *caminando;
SAMPLE *muerte;

int dir = 4;
int px = 14*30;
int py = 17*30;
int apx;
int apy;

char mapa[MAXFILAS][MAXCOLS] = {
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "X  o |o o o XXXXX o o o| o  X",
    "X XXX XXXXX XXXXX XXXXX XXX X",
    "XoXXX XXXXX XXXXX XXXXX XXXoX",
    "X      o|o   o o   o|o      X",
    "XoXXXoXX XXXXXXXXXXX XXoXXXoX",
    "X    |XX    |XXX|    XX     X",
    "XoXXXoXXXXXX XXX XXXXXXoXXXoX",
    "X XXXoXX ooo|ooo|ooo XXoXXX X",
    " o   |XX XXXXXXXXXXX XX|   o ",
    "X XXXoXX XXXXXXXXXXX XXoXXX X",
    "XoXXXoXX oo |ooo|ooo XXoXXXoX",
    "X XXXoXXXXXX XXX XXXXXXoXXX X",
    "X     XX     XXX     XX     X",
    "X XXXoXX XXXXXXXXXXX XXoXXX X",
    "XoXXX| o| o o o o o |o |XXXoX",
    "X XXXoXXXX XXXXXXXX XXX XXX X",
    "XoXXXoXXXX          XXX XXXoX",
    "X  o |o o  XXXXXXXX o o| o  X",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
};

void dibujar_mapa() {
    int row, col;
    for(row = 0; row < MAXFILAS; row++) {
        for(col = 0; col < MAXCOLS; col++) {
            if(mapa[row][col] == 'X') {
                draw_sprite(buffer, roca, col*30, row*30);
            }else if(mapa[row][col] == 'o') {
                draw_sprite(buffer, comida, col*30, row*30);
                if(py/30 == row && px/30 == col) {
                    play_sample(bolita,200,150,1000,0);
                    mapa[row][col] = ' ';
                }
            }

        }
    }
}

void pantalla() {
    blit(buffer, screen, 0,0,0,0,880, 600);
}

void dibujar_personaje() {
    blit(pacbmp, pacman, dir*33, 0, 0, 0, 33, 33);
    draw_sprite(buffer, pacman, px, py);
}

bool game_over() {
    int row, col;
    for(row = 0; row < MAXFILAS; row++) {
        for(col = 0; col < MAXCOLS; col++) {
            if(mapa[row][col] == '0') {
                return false;
            }
        }
    }
    return true;
}

class Fantasma {
    BITMAP *enemigobmp;
    BITMAP *enemigo;
    int fdir;
    int _x;
    int _y;

public:
    Fantasma(int x, int y);
    void dibujar_fantasma() const;
    void mover_fantasma();
    void choque_pacman();
};

Fantasma::Fantasma(int x, int y) {
    _x = x;
    _y = y;
    fdir = rand() % 4;
    enemigo = create_bitmap(30, 30);
    enemigobmp = load_bitmap("enemigo.bmp", NULL);
}

void Fantasma::dibujar_fantasma() const {
    blit(enemigobmp, enemigo, 0, 0, 0, 0, 30, 30);
    draw_sprite(buffer, enemigo, _x, _y);
}

void Fantasma::choque_pacman() {
    if((py == _y && px == _x) || (apy == _y && apx == _x)) {
        play_sample(muerte,100,150,1000,0);
        for(int j = 0; j <= 5; j++) {
            clear(pacman);
            clear(buffer);
            dibujar_mapa();

            blit(muertebmp,pacman,j*33,0,0,0,33,33);
            draw_sprite(buffer, pacman,px, py);

            pantalla();
            rest(80);
        }
        px = 14 * 30;
        py = 17 * 30;
        dir = 4;
    }
}

void Fantasma::mover_fantasma() {
    dibujar_fantasma();
    choque_pacman();

    if(mapa[_y/30][_x/30] == '|') {
        fdir = rand() % 4;
    }

    if(fdir == 0) {
        if(mapa[_y/30][(_x-30)/30] != 'X')
            _x -= 30;
        else
            fdir = rand() % 4;
    } else if(fdir == 1) {
        if(mapa[_y/30][(_x+30)/30] != 'X')
            _x += 30;
        else
            fdir = rand() % 4;
    } else if(fdir == 2) {
        if(mapa[(_y-30)/30][_x/30] != 'X')
            _y -= 30;
        else
            fdir = rand() % 4;
    } else if(fdir == 3) {
        if(mapa[(_y+30)/30][_x/30] != 'X')
            _y += 30;
        else
            fdir = rand() % 4;
    }

    // validar atajo
    if(_x <= -30)
            _x = 870;
        else if(_x >= 870)
            _x = -30;
}

int main() {
    allegro_init();
    install_keyboard();

    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 880, 600, 0,0);

    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
            allegro_message("Error: inicializando sistema de sonido\n%s\n", allegro_error);
            return 1;
    }
    set_volume(70, 70);

    musica1 = load_midi("mario.mid");
    bolita = load_wav("coin.wav");
    caminando = load_wav("jump.wav");
    muerte = load_wav("muerte.wav");

    buffer = create_bitmap(880, 600);
    roca = load_bitmap("roca.bmp", NULL);
    pacbmp = load_bitmap("pacman.bmp", NULL);
    pacman = create_bitmap(33,33);
    comida = load_bitmap("comida.bmp", NULL);
    muertebmp = load_bitmap("muerte.bmp", NULL);

    Fantasma A(2*30, 3*30);
    Fantasma B(15*30, 15*30);
    Fantasma C(15*30, 15*30);
    Fantasma D(15*30, 15*30);
    Fantasma E(15*30, 15*30);
    Fantasma F(15*30, 15*30);

    bool pause = false;

    play_midi(musica1,1);
    while(!key[KEY_ESC] && game_over()) {
        if(pause == true)
            continue;


        apx = px;
        apy = py;

        if(dir != 4)
            play_sample(caminando,100,150,1000,0);

        if(key[KEY_RIGHT]) dir = 1;
        else if(key[KEY_LEFT]) dir = 0;
        else if(key[KEY_UP]) dir = 2;
        else if(key[KEY_DOWN]) dir = 3;

        if(dir == 0) {
            if(mapa[py/30][(px-30)/30] != 'X')
                px -= 30;
            else
                dir = 4;
        }else if(dir == 1) {
            if(mapa[py/30][(px+30)/30] != 'X')
                px += 30;
            else
                dir = 4;
        }
        else if(dir == 2) {
            if(mapa[(py-30)/30][(px)/30] != 'X')
                py -= 30;
            else
                dir = 4;
        }
        else if(dir == 3) {
            if(mapa[(py+30)/30][(px)/30] != 'X')
                py += 30;
            else
                dir = 4;
        }

        // validar atajos
        if(px <= -30)
            px = 870;
        else if(px >= 870)
            px = -30;

        clear(buffer);
        dibujar_mapa();
        dibujar_personaje();
        A.mover_fantasma();
        B.mover_fantasma();
        C.mover_fantasma();
        D.mover_fantasma();
        E.mover_fantasma();
        F.mover_fantasma();
        pantalla();
        rest(70);

        clear(pacman);
        blit(pacbmp, pacman, 4*33, 0, 0, 0, 33, 33);
        draw_sprite(buffer, pacman, px, py);
        pantalla();
        rest(90);
    }

    return 0;
}
END_OF_MAIN();
