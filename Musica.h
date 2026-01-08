#ifndef MUSICA_H
#define MUSICA_H

#include "Config.h"

extern NotaGravada musicaDefault[];
extern int totalDefault;

extern NotaGravada musicaFacil[];
extern int totalFacil;

extern NotaGravada musicaDificil[];
extern int totalDificil;

void iniciarReproducao(bool comSom);
void processarMusica();
void finalizarReproducao();

#endif
