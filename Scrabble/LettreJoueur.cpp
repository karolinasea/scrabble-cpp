#include"LettreJoueur.hpp"

LettreJoueur::LettreJoueur(SacLettre *sac){
  for (int i = 0; i < 7; i++){
    lettreDuJoueur.push_back(sac->choisirLettreAleatoire());
  }
}

LettreJoueur::LettreJoueur(std::vector<char> lettres) {
	lettreDuJoueur = lettres;
}

void LettreJoueur::rechargerLesLettresDuJoueur(SacLettre *sac){
  while(!sac->estVide() && lettreDuJoueur.size() < 7){
    lettreDuJoueur.push_back(sac->choisirLettreAleatoire());
  }
}

unsigned int LettreJoueur::nombreDeLettre() {
	return lettreDuJoueur.size();
}
