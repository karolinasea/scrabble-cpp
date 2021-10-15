#include <fstream>
#include <iostream>

#include "ArbreDico.hpp"

ArbreDico::ArbreDico() {
    racine = new Noeud();
	Noeud * plus = new Noeud('+');
	racine->tabNoeud.push_back(plus);
}

ArbreDico::~ArbreDico() {
	detruireDepuisNoeud(racine);
}

void ArbreDico::detruireDepuisNoeud(Noeud * n) {
	if(n != nullptr) {
		for(int i=0; i<n->tabNoeud.size(); i++) {
			detruireDepuisNoeud(n->tabNoeud[i]);
		}
	delete n;
	}
}

void ArbreDico::insererMotRec(std::string mot, Noeud *n) {
	int asciiLettreIndex;
	if(mot[0] == '+') asciiLettreIndex = 26;
	else asciiLettreIndex = mot[0]-65;
	if(n->tabNoeud[asciiLettreIndex] != nullptr) {  
		if(mot.size() == 1) {        
			n->finale = true;
		} else {
			insererMotRec(mot.substr(1, mot.size()-1), n->tabNoeud[asciiLettreIndex]);
		}
	}
	if(n->tabNoeud[asciiLettreIndex] == nullptr) {
		Noeud * nouveauNoeud = new Noeud(mot[0]);
		if(mot[0] != '+') {
			Noeud * plus = new Noeud('+');
			nouveauNoeud->tabNoeud.push_back(plus);
		}
		n->tabNoeud[asciiLettreIndex] = nouveauNoeud;
		if(mot.size() == 1) {   
			nouveauNoeud->finale = true;
		 } else { 
			insererMotRec(mot.substr(1, mot.size()-1), nouveauNoeud);
		}
	}
}

void ArbreDico::insererMotIt(std::string str) {
	Noeud * n = racine;
	for(unsigned int i=0; i<str.size(); i++) {
		int ind; 
		if(str[i] == '+') {
			ind = 26;
		} else {
			ind = str[i]-65;
		}
		if(n->tabNoeud[ind] != nullptr) {
			if(i == str.size()-1) {
				n->tabNoeud[ind]->finale = true;
			} else {
				n = n->tabNoeud[ind];
			}
		} else {
			Noeud *nouveau = new Noeud(str[i]);
			n->tabNoeud[ind] = nouveau;
			if(str[0] != '+') {
				Noeud * plus = new Noeud('+');
				nouveau->tabNoeud.push_back(plus);
			}
			if(i == str.size()-1) {
				n->tabNoeud[ind]->finale = true;
			} else {
				n = nouveau;
			}
		}
	}
}

void ArbreDico::genereEtInsereMotsAvecLeSignePlus(std::string mot) {
	for(int i=1; i<=(int)mot.size(); i++) {
		std::string nouveauMot = mot.substr(0, i) + '+' + mot.substr(i, mot.size()-1);	
		int index = nouveauMot.find('+');
		for (int j = 0; j<(int)nouveauMot.substr(0, index).size()/2; j++) {
			std::swap(nouveauMot[j], nouveauMot[nouveauMot.substr(0, index).size() - j - 1]); 
		}
		// insererMotRec(nouveauMot, racine);
		insererMotIt(nouveauMot);
	}
}


void ArbreDico::remplirArbre() {
	std::cout << "Remplissage du dictionnaire" << std::endl;
	std::ifstream fichierDico("dico.txt");  
    if(fichierDico) { 
    	std::string mot;
      	while(fichierDico >> mot) {
	   		genereEtInsereMotsAvecLeSignePlus(mot);
      	}
      std::cout << "L'ArbreDico est rempli :)" << std::endl;
   	} else {
    	std::cout << "ERREUR: Impossible d'ouvrir le fichier en lecture." << std::endl;
    }
}

void ArbreDico::afficherArbre(Noeud *n, std::string s) { 
	if(n != nullptr) {
		if(n->finale) {
			std::cout << s << std::endl;
			for(int i=0; i<27; i++) {
				if(n->tabNoeud[i] != nullptr) {
					afficherArbre(n->tabNoeud[i], s+n->tabNoeud[i]->lettre);
				}
			}
		}
		if(!n->finale) {
			for(int i=0; i<27; i++) {
				if(n->tabNoeud[i] != nullptr) {
					afficherArbre(n->tabNoeud[i], s+n->tabNoeud[i]->lettre);
				}
			}
		}
	}
}

bool ArbreDico::verifieSiMotExiste(std::string &str) {
	Noeud * n = racine;
	for(unsigned int i=0; i<str.length(); i++) {
		int ind; 
		if(str[i] == '+') {
			ind = 26;
		} else {
			ind = str[i]-65;
		}
		if(n->tabNoeud[ind] != nullptr) {
			n = n->tabNoeud[ind];

		} else {
			return false;
		}
	}
	return n->finale;
}





