#ifndef ETAT_H
#define ETAT_H

#include <string>
#include <vector>

#include "ArbreDico.hpp"
#include "LettreJoueur.hpp"

class Etat {

public:
	int numCaseSuivante;
	Noeud * noeud;
	LettreJoueur * lettresRestantesDuJoueur; //lettres pas encore parcourues
	std::string chaineDeLettresDejaParcouruPourArriveACetEtat;
	std::vector<unsigned int> numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
	bool sensDexploration; // 1 pour vertical 0 pour horizontal

	Etat();
	Etat(int num, Noeud *n, LettreJoueur *l, std::string str, std::vector<unsigned int> numeros, bool sens);
    ~Etat();

};

#endif

