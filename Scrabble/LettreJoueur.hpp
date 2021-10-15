#ifndef LETTRE_JOUEUR_HPP
#define LETTRE_JOUEUR_HPP

#include <vector>
#include <string>
#include "sacLettre.hpp"

class LettreJoueur {

  public :
    LettreJoueur(SacLettre *sac);
    LettreJoueur(std::vector<char> lettres);
    void rechargerLesLettresDuJoueur(SacLettre *sac);
    unsigned int nombreDeLettre();

  public :
    std::vector<char> lettreDuJoueur;

} ;

#endif
