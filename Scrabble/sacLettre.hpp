#ifndef SAC_LETTRE_HPP
#define SAC_LETTRE_HPP

#include <vector>

class SacLettre {

  public :
    SacLettre();
    void RemplirLeSacDeLettre();
    void mettreLigneDansTableau(char _lettre, int _nombre, int _points);
    char choisirLettreAleatoire();
    bool estVide();
    unsigned int donnerLeNombreDePoint(char c);
    void reremplirLeSac(std::vector<char> lettreDuJoueur);
    
  public :
    std::vector<char> lettre;
    std::vector<int> nombre;
    std::vector<int> points;
    std::vector<char> sacDeLettrePourLesJoueurs;
} ;

#endif