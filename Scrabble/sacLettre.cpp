#include "sacLettre.hpp"

SacLettre::SacLettre(){
  mettreLigneDansTableau('A', 9, 1);
  mettreLigneDansTableau('B', 2, 3);
  mettreLigneDansTableau('C', 2, 3);
  mettreLigneDansTableau('D', 3, 2);
  mettreLigneDansTableau('E', 15, 1);
  mettreLigneDansTableau('F', 2, 4);
  mettreLigneDansTableau('G', 2, 2);
  mettreLigneDansTableau('H', 2, 4);
  mettreLigneDansTableau('I', 8, 1);
  mettreLigneDansTableau('J', 1, 8);
  mettreLigneDansTableau('K', 1, 10);
  mettreLigneDansTableau('L', 5, 1);
  mettreLigneDansTableau('M', 3, 2);
  mettreLigneDansTableau('N', 6, 1);
  mettreLigneDansTableau('O', 6, 1);
  mettreLigneDansTableau('P', 2, 3);
  mettreLigneDansTableau('Q', 3, 2);
  mettreLigneDansTableau('R', 6, 1);
  mettreLigneDansTableau('S', 6, 1);
  mettreLigneDansTableau('T', 6, 1);
  mettreLigneDansTableau('U', 6, 1);
  mettreLigneDansTableau('V', 2, 4);
  mettreLigneDansTableau('W', 1, 10);
  mettreLigneDansTableau('X', 1, 10);
  mettreLigneDansTableau('Y', 1, 10);
  mettreLigneDansTableau('Z', 1, 10);

  RemplirLeSacDeLettre();
}

void SacLettre::RemplirLeSacDeLettre(){
  for (unsigned int i = 0; i < lettre.size(); i++){
    for (int j = 0; j < nombre[i]; j++){
      sacDeLettrePourLesJoueurs.push_back(lettre[i]);
    }
  }
}

void SacLettre::mettreLigneDansTableau(char _lettre, int _nombre, int _points) {
  lettre.push_back(_lettre);
  nombre.push_back(_nombre);
  points.push_back(_points);
}

char SacLettre::choisirLettreAleatoire() {
   int nbAleatoire = rand() % sacDeLettrePourLesJoueurs.size();
   char lettreChoisieAleatoirement = sacDeLettrePourLesJoueurs[nbAleatoire];
   sacDeLettrePourLesJoueurs.erase(sacDeLettrePourLesJoueurs.begin() + nbAleatoire);
   return lettreChoisieAleatoirement;
}

bool SacLettre::estVide() {
  return (sacDeLettrePourLesJoueurs.size() == 0);
}

unsigned int SacLettre::donnerLeNombreDePoint(char c) {
  return points[c - 'A'];
}

void SacLettre::reremplirLeSac(std::vector<char> lettreDuJoueur){
    for (int i = 0; i < lettreDuJoueur.size(); i++) {
        sacDeLettrePourLesJoueurs.push_back(lettreDuJoueur[i]);
    }
}





