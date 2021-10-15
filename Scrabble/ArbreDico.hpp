#ifndef ARBRE_DICO_H
#define ARBRE_DICO_H

#include <vector>
#include <string>

struct Noeud {
    char lettre;
    bool finale;
    std::vector<Noeud *> tabNoeud;

	//constructeur
	Noeud(char caractere='\0', bool fin=false) {
		lettre = caractere;
		finale = fin;
		for(unsigned int i=0; i<26; i++) {
			tabNoeud.push_back(nullptr);
		}
	}
};

class ArbreDico {

public:

    /****** Donnée membre: ******/
    Noeud * racine;

    /****** Fonctions membres: ******/

    //Constructeur créer un arbre vide
    ArbreDico();
    
    //Destructeur supprime tous les noeuds de l'arbre en utilisant detruireDepuisNoeud
    ~ArbreDico();

    //méthode recursive pour detruire chaque noeud de l'arbre
    void detruireDepuisNoeud(Noeud * n);

    //remplit l'arbre avec les mots du dictionnaire du fichier data/dico.txt
    void remplirArbre();

    //insère le mot "mot" dans l'arbre
    void insererMotIt(std::string str);

    void insererMotRec(std::string mot, Noeud *n);

    //prend un mot et genere toutes les combinaisons avec le signe + 
    //et insere ces mots dans l'arbre
    void genereEtInsereMotsAvecLeSignePlus(std::string mot);

    // affiche tous les noeuds de l'arbre 
    void afficherArbre(Noeud *n, std::string s);

    //parcours l'arbre en profondeur pour trouver un mot a jouer
    bool verifieSiMotExiste(std::string &str);
};

#endif

