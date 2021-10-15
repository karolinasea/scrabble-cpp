#include <string>
#include <vector>

#include "Etat.hpp"
#include "ArbreDico.hpp"
#include "LettreJoueur.hpp"

Etat::Etat() {}

Etat::Etat(int num, Noeud *n, LettreJoueur *l, std::string str, std::vector<unsigned int> numeros, bool sens) {
	numCaseSuivante = num;
	noeud = n;
	lettresRestantesDuJoueur = l;
	chaineDeLettresDejaParcouruPourArriveACetEtat = str;
	numerosDesCasesSurLesquelsSontLesLettresDeLaChaine = numeros;
	sensDexploration = sens;
}

Etat::~Etat() {
	delete noeud;
	delete lettresRestantesDuJoueur;
}