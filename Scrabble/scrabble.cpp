#include "board.hpp"
#include "sacLettre.hpp"
#include "LettreJoueur.hpp"
#include "ArbreDico.hpp"
#include "Etat.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <deque>

std::string creationChaineSansSignePlus(Etat *e) {
	std::string mot = "";
	for(unsigned int l=0; l<e->chaineDeLettresDejaParcouruPourArriveACetEtat.size(); l++) {
		char c = e->chaineDeLettresDejaParcouruPourArriveACetEtat[l];
	    if(c != '+') { mot.push_back(c); }
	}
	return mot;
}

int main() {
  srand(time(NULL));
  Board b;
  std::stringstream ss;

  for(int i=0; i<15; i++) {
  	ss << "..............." << std::endl;
  }

  b.load(ss);

  std::cout << b << std::endl;

  //initialisation du jeu
  SacLettre *sac = new SacLettre();
  LettreJoueur lettreDuJoueur(sac);

  //initialisation de l'arbre contenant toutes les versions de mots avec le signe '+''
  ArbreDico arbre;
  arbre.remplirArbre();
  // arbre.afficherArbre(arbre.racine, "");
  bool impossibleDeContinuerAJouer = false; //true lorsque le joueur a encore des lettres (et eventuellement il y a encore des lettres dans le sac) mais que il n'y a plus aucun mot jouable a partir de ces lettres on peut mettre a false pour arreter la boucle (on detecte ce souci quand l'etat avec le meilleur score est nullptr)
  unsigned int scoreMax = 0;

while(!sac->estVide()) { //when game is over (check also if joueur just cannot play anymore letters)
    // std::cout << "nombre de lettres dans sac = " << sac->sacDeLettrePourLesJoueurs.size() << std::endl; 

    if((!sac->estVide() && lettreDuJoueur.lettreDuJoueur.size()<7)) {
    	lettreDuJoueur.rechargerLesLettresDuJoueur(sac);
    } 

    //si aucunes lettres ne peuvent etre jouees pour créer un mot valide on pioche de nouvelles lettres
    if(impossibleDeContinuerAJouer /*&& sac->sacDeLettrePourLesJoueurs.size() > 7*/) {
    	//on a pas trouver de mot a jouer donc on pioche de nouvelles lettres
    	std::cout << "pas de mot possible, on pioche de nouvelles lettres" << std::endl;
    	lettreDuJoueur.lettreDuJoueur.clear();
    	lettreDuJoueur.rechargerLesLettresDuJoueur(sac);
    	//il faudrait apres avoir changer les lettres replacer celles d'avant dans le sac.. pour respecter la regle du jeu
    	//remettre les lettres enlevées dans le sac
    	//si on met ça il faudra une condition different pour la premiere boucle while pour eviter une boucle infinie
    }

    // affiche les lettres du joueur
    // for(int n=0; n<lettreDuJoueur.lettreDuJoueur.size(); n++) {
    // 	std::cout << "lettreDuJoueur.lettreDuJoueur = " << lettreDuJoueur.lettreDuJoueur[n] << std::endl;
    // }

    //recuperer les cases vides a cote des cases occupées:
    std::vector<unsigned int> casesVidesACoteDeCasesOccupees;
    casesVidesACoteDeCasesOccupees = b.caseQueOnPeutRemplir();
	
    //la pile pour mettre les etats:
    std::deque<Etat *> pile;    

    //construit 2 etats initiaux pour chaque case vide sur lesquelles on peut jouer un mot (un pour horizontal et un pour vertical) et on les met dans la pile
    for(unsigned int i=0; i<casesVidesACoteDeCasesOccupees.size(); i++) {
    	// std::cout << "casesVidesACoteDeCasesOccupees = " << casesVidesACoteDeCasesOccupees[i] << std::endl;
	    LettreJoueur *l;
	    l = &lettreDuJoueur;
	    std::vector<unsigned int> v;
	    Etat *etatInitialVertical = new Etat(casesVidesACoteDeCasesOccupees[i], arbre.racine, l, "", v, 1);
	    Etat *etatInitialHorizontal = new Etat(casesVidesACoteDeCasesOccupees[i], arbre.racine, l, "", v, 0);
    	pile.push_front(etatInitialVertical);
    	pile.push_front(etatInitialHorizontal);
    }

    scoreMax = 0;
    Etat* etatContenantMotDonnantLeMeilleurScore = nullptr;

    while(!pile.empty()) { //tant que la pile d'etats n'est pas vide
    	Etat *etatCourant = pile.front(); //dépiler un sommet "courant":
    	pile.pop_front();

    	if(etatCourant->noeud != nullptr) {
    		if(etatCourant->noeud->finale) {
		    	if(b.spots[etatCourant->numCaseSuivante].letter == 0) { // == 0 to make sure there isn't any other letter after that could make the word wrong
		    	  //calcul le score de chaque etat qui a un mot valide et compare le score avec le score max
		    		std::string mot = creationChaineSansSignePlus(etatCourant);
		    		unsigned int score = 0, wordBonnus2 = 225, wordBonnus3 = 225;
		   			for(unsigned int p=0; p<mot.size(); p++) {
		    			char c = mot[p];
		    			//calcul le score de chaque lettre de la chaine
		    			score+=sac->donnerLeNombreDePoint(c);
			    		int ind = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[p];
			    		//verifie si il y a des bonus mot ou bonus lettre sur les lettres de la chaine qui ne sont pas deja sur le plateau
			   			if(b.spots[ind].letter == 0) {
				   			if(b.spots[ind].bonus.word_factor == 3 && b.spots[ind].letter == 0) {
				  				wordBonnus3 = ind;
			   				}
				    		if(b.spots[ind].bonus.word_factor == 2 && b.spots[ind].letter == 0) {
								wordBonnus2 = ind;
			   				}
			   				if(b.spots[ind].bonus.letter_factor == 2 || b.spots[ind].bonus.letter_factor == 3) {
			    				score+=b.spots[ind].bonus.apply_letter(sac->donnerLeNombreDePoint(c));
			    			}
			    		}		    				
		    		}
		    		unsigned int scoreInitial = score; //on applique les bonus mot si il y en a
		   			if(wordBonnus2 < 225) {
		    			score+=b.spots[wordBonnus2].bonus.apply_word(scoreInitial);
		    		}
		   			if(wordBonnus3 < 225) {
			   			score+=b.spots[wordBonnus3].bonus.apply_word(scoreInitial);
			   		}
		    		if(score > scoreMax) {
		    			scoreMax = score;
		    			etatContenantMotDonnantLeMeilleurScore = etatCourant;
			    	}
		    	}
	    	}
	    	//verifie si etatCourant->chaine.. contient le signe '+' ou pas
	  		bool checkPlusSign = false;
	    	for(unsigned int t=0; t<etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat.size(); t++) {
	   			if(etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat[t] == '+') { checkPlusSign = true; }
	   		}
	    	if(!checkPlusSign) { // etats sans signe '+'
	    		if(etatCourant->sensDexploration) { // sens vertical
	   				if(b.spots[etatCourant->numCaseSuivante].letter == 0) { //verifie si la caseSuivante est vide ou pas vide pour choisir quelle lettre mettre dans noeud de etat
	   					int numCaseSuivante; //la case est vide donc on creer un etat avec le signe +
		    			if(etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() != 0) { //verifie si la chaine n'est pas vide pour ne pas faire une chaine commençant par le sign '+' car ça n'existe pas dans l'arbre
		    				numCaseSuivante = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[0]; //on revient a la premiere case et on part de cette case-la avec le '+'
		   					if(etatCourant->noeud->tabNoeud[26] != nullptr && numCaseSuivante+15 < 209) { //verifie que on est pas au bord en bas du plateau
		   						char s = '+';
		   						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		    					str.push_back(s);
			 					Etat *nouvelEtat = new Etat(numCaseSuivante+15, etatCourant->noeud->tabNoeud[26], etatCourant->lettresRestantesDuJoueur, str, etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine, 1);
						   		pile.push_front(nouvelEtat);
					    	}
					    }
	   					if(/*&& etatCourant->numCaseSuivante+1 < 225 &&*/ b.spots[etatCourant->numCaseSuivante-1].letter == '\0' && b.spots[etatCourant->numCaseSuivante+1].letter == '\0') { 
	   					//verifie si autres cases en a droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres
					    	//on creer tous les autres etats possibles avec les lettres restantes du joueur
					    	if(etatCourant->numCaseSuivante-15 > 14) { // si < 14 on est sur le rebord du plateau (tout en haut du plateau)
	    						for(int j=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; j>=0; j--) {
		    						int index = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j] - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						char s = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j];
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
			    						str.push_back(s);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != j) {
										   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
										   	}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
								    	pile.push_front(nouvelEtat);
								    }
								}
	   						}
	   					} else if(b.spots[etatCourant->numCaseSuivante-1].letter == 0 && b.spots[etatCourant->numCaseSuivante+1].letter != 0) {
	    					unsigned int i = 1;
	    					std::string testString = "";
	    					bool f = true;
	    					while(f) { //construit la chaine a verifier pour voir si le mot est valide
		    					//si il n'y a plus de lettres ou si on est arrivé au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante+i].letter == 0 || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
		    						f = false;
		    					} else {
		    						testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
		    					}
		   						i++;
		    				}
	    					for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
	    						std::string temp = testString;
	    						char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
	    						temp.insert(0, 1, c);
	    						if(arbre.verifieSiMotExiste(temp)) { //si mot est valide on construit l'etat 
		    						int index = c - 65;
			    					if(etatCourant->noeud->tabNoeud[index] != nullptr) {
				   						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
				   						str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
										   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
										   	}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
									    pile.push_front(nouvelEtat);
		    						}
	    						}
	   						}
	  					} else if(b.spots[etatCourant->numCaseSuivante-1].letter != 0 && b.spots[etatCourant->numCaseSuivante+1].letter == 0) {
	    					unsigned int i = 1;
	    					std::string testString = "";
	    					bool f = true;
	    					while(f) { //construit la chaine a verifier pour voir si le mot est valide tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante-i].letter == 0 || (etatCourant->numCaseSuivante-i)%15 == 0) {
		    						f = false;
		    					} else {
		    						testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
		    					}
		   						i++;
		   					}
	   						for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
	   							std::string temp = testString;
	    						char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
	    						temp.push_back(c);
	    						if(arbre.verifieSiMotExiste(temp)) {
		    						int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
			    						str.push_back(c);
										std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
										   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
										   	}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
								    	pile.push_front(nouvelEtat);
		    						}
	    						}
	    					}
	    				} else if(b.spots[etatCourant->numCaseSuivante-1].letter != 0 && b.spots[etatCourant->numCaseSuivante+1].letter != 0) {
	    					unsigned int i = 1, j= 1;
	    					std::string testString = "", testString1 = "";
	   						bool f = true, ff = true;
	   						while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante+i].letter == 0 || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
		    						f = false;
		    					} else {
		    						testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
		    					}
		    					i++;
		    				}
		    				while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante-j].letter == 0 || (etatCourant->numCaseSuivante-j)%15 == 0) {
		    						ff = false;
		    					} else {
		    						testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-j].letter);
		    					}
		    					j++;
		   					}
		   					for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		   						std::string temp = testString;
		   						std::string temp1 = testString1;
	   							char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
	    						temp1.push_back(c);
	    						temp1+=temp;
	    						if(arbre.verifieSiMotExiste(temp1)) {
		    						int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
				    					str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
										   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
										   	}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
								    	pile.push_front(nouvelEtat);
		    						}
	    						}
	    					}
	    				}
	    			} else { //si caseSuivante n'est pas vide on creer un etat avec la lettre qui est deja sur cette case:
	   					char letter = b.spots[etatCourant->numCaseSuivante].letter;
	   					int index = letter - 65;
	   					if(etatCourant->noeud->tabNoeud[index] != nullptr) {
		    				std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		    				str.push_back(letter);
						    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
						    v.push_back(etatCourant->numCaseSuivante);	
						    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-15, etatCourant->noeud->tabNoeud[index], etatCourant->lettresRestantesDuJoueur, str, v, 1);
					    	pile.push_front(nouvelEtat);
						}
    				}
	    		} else { //horizontal
	   				if(b.spots[etatCourant->numCaseSuivante].letter == 0) { //verifie si la caseSuivante est vide ou pas vide pour choisir quelle lettre mettre dans noeud de etat
	    				int numCaseSuivante; //on fait un etat contenant le '+'
	   					if(etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() != 0) {
		   					numCaseSuivante = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[0]; //on revient a la premiere case et on part de la avec le '+'
	    					if(etatCourant->noeud->tabNoeud[26] != nullptr && 14-(numCaseSuivante-((numCaseSuivante/15)*15)) != 0) {//14-(numCaseSuivante-((numCaseSuivante/15)*15)) != 0 verifie que on est pas sur le rebord droit du plateau
			    				char s = '+';
			   					std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
			   					str.push_back(s);
							    Etat *nouvelEtat = new Etat(numCaseSuivante+1, etatCourant->noeud->tabNoeud[26], etatCourant->lettresRestantesDuJoueur, str, etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine, 0);
							    pile.push_front(nouvelEtat);
							}
						}
	    				if(/*etatCourant->numCaseSuivante-15 >= 0 && etatCourant->numCaseSuivante+15 < 225 &&*/ b.spots[etatCourant->numCaseSuivante-15].letter == 0 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) { //verifie si autres cases en a droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres et si oui check if word is valid
							if(etatCourant->numCaseSuivante%15 != 0) { //verifie qu'on est pas sur le rebord gauche du plateau
		    					for(int j=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; j>=0; j--) {
		    						int index = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j] - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
		    							char s = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j];
		    							std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		    							str.push_back(s);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
										v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
									    for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
									    	if(a != j) {
									    		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
									    	}
										}
									    LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
								    	pile.push_front(nouvelEtat);
								    }
		    					}
		   					}
	    				} else if(b.spots[etatCourant->numCaseSuivante-15].letter != 0 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) {
		   					unsigned int i = 15;
		    				std::string testString = "";
		    				bool f = true;
		    				while(f) { //construit la chaine a verifier tant qu il y a des lettres ou que l'on arive au bord du plateau		    					
			   					if(b.spots[etatCourant->numCaseSuivante-i].letter == 0 || etatCourant->numCaseSuivante-i < 0) {
			   						f=false;
			   					} else {
			   						char c = b.spots[etatCourant->numCaseSuivante-i].letter;
			   						testString.insert(0, 1, c);
			   					}
			  					i+=15;
		   					}
		    				for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		    					std::string temp = testString;
		    					char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		   						temp.push_back(c);
		    					if(arbre.verifieSiMotExiste(temp)) { 
			    					int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					    				str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
												lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
										pile.push_front(nouvelEtat);
			    					}
		    					}
		    				}
	   					} else if(b.spots[etatCourant->numCaseSuivante-15].letter == 0 && b.spots[etatCourant->numCaseSuivante+15].letter != 0) {
	   						unsigned int i = 15;
		    				std::string testString = "";
		    				bool f = true;
		    				while(f) { //construit la chaine a verifier. Tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante+i].letter == 0 || (etatCourant->numCaseSuivante+i) > 225) {
		    						f =false;
		    					} else {
		    						testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
		    					}
			  					i+=15;
		   					}
		    				for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		    					char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		   						std::string temp = testString;
		   						temp.insert(0, 1, c);
		    					if(arbre.verifieSiMotExiste(temp)) {
			    					int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					    				str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
												lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
										pile.push_front(nouvelEtat);
			    					}
		    					} 
		    				}
	   					} else if(b.spots[etatCourant->numCaseSuivante-15].letter != 0 && b.spots[etatCourant->numCaseSuivante+15].letter != 0) {
	   						unsigned int i = 15, j = 15;
		    				std::string testString = "", testString1 = "";
		    				bool f = true, ff = true;
		    				while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante-i].letter == 0 || etatCourant->numCaseSuivante-i < 0) {
		    						f = false;
		    					} else {
		    						testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
		    					}
			  					i+=15;
		   					}
		   					while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
			   					if(b.spots[etatCourant->numCaseSuivante+j].letter == 0 || (etatCourant->numCaseSuivante+j) > 225) {
		    						ff = false;
		    					} else {
		    						testString.push_back(b.spots[etatCourant->numCaseSuivante+j].letter);
		    					}
			  					j+=15;
		   					}
		   					for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		    					std::string temp = testString;
		    					std::string temp1 = testString1;
		    					char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		   						temp.push_back(c);
		   						temp+=temp1;
		    					if(arbre.verifieSiMotExiste(temp)) {
			    					int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					    				str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
												lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
										pile.push_front(nouvelEtat);
			    					}
		    					}
		    				}
	   					}
		   			} else { //si caseSuivante n'est pas vide on creer un etat avec la lettre de la case:
	    				char letter = b.spots[etatCourant->numCaseSuivante].letter;
	    				int index = letter - 65;
	    				if(etatCourant->noeud->tabNoeud[index] != nullptr) {
		   					std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		   					str.push_back(letter);
						    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
						    v.push_back(etatCourant->numCaseSuivante);	
							Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], etatCourant->lettresRestantesDuJoueur, str, v, 0);
						    pile.push_front(nouvelEtat);
						}
	    			}
	    		}
	    	} else { // have plus sign
	   			if(etatCourant->sensDexploration) { //verifie horizontal ou vertical pour +-1 ou +-15: ici on est en vertical
	    			if(etatCourant->numCaseSuivante < 209) { //(si <209 means not on the edge, si > 209 means it's on the edge so only do etat '+') verifie si la case numero de l'etatCoutant est au bout du plateau (la derniere case de la ligne ou de la colonne) dans ce cas on peut faire seulement un etat '+' pas d'autres
	   					if(b.spots[etatCourant->numCaseSuivante].letter == '\0') { //(ici vide)verifie si la caseSuivante est vide ou pas vide pour choisir quelle lettre mettre dans noeud de etat
	    					if(b.spots[etatCourant->numCaseSuivante-1].letter == 0 && b.spots[etatCourant->numCaseSuivante+1].letter == 0) { //verifie si autres cases en a droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres et si oui check if word is valid
	   							for(int j=etatCourant->lettresRestantesDuJoueur->lettreDuJoueur.size()-1; j>=0; j--) {
	   								int index = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j] - 65;
    								if(etatCourant->noeud->tabNoeud[index] != nullptr) {
		    							char s = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j];
		    							std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		    							str.push_back(s);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
									    	if(a != j) {
										    	lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
										    }
										}
									    LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
								    	pile.push_front(nouvelEtat);
								    }
	    						}
	    					} else if(b.spots[etatCourant->numCaseSuivante-1].letter == 0 && b.spots[etatCourant->numCaseSuivante+1].letter != 0) {
		    					unsigned int i = 1;
		    					std::string testString = "";
		    					bool f = true;
		    					while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
			    					if(b.spots[etatCourant->numCaseSuivante+i].letter == 0 || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
			    						f = false;
			    					} else {
			    						testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
			    					}
			   						i++;
			    				}
		    					for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		    						std::string temp = testString;
		    						char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		    						temp.insert(0, 1, c);
		    						if(arbre.verifieSiMotExiste(temp)) {
			    						int index = c - 65;
				    					if(etatCourant->noeud->tabNoeud[index] != nullptr) {
					   						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					   						str.push_back(c);
										    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
										    v.push_back(etatCourant->numCaseSuivante);		
										    std::vector<char> lettreRestantes;
											for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
											    if(a != k) {
											   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											   	}
											}
											LettreJoueur *l = new LettreJoueur(lettreRestantes);
										    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
										    pile.push_front(nouvelEtat);
			    						}
		    						}
		   						}
		  					} else if(b.spots[etatCourant->numCaseSuivante-1].letter != 0 && b.spots[etatCourant->numCaseSuivante+1].letter == 0) {
		    					unsigned int i = 1;
		    					std::string testString = "";
		    					bool f = true;
		    					while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
			    					if(b.spots[etatCourant->numCaseSuivante-i].letter == 0 || (etatCourant->numCaseSuivante-i)%15 == 0) {
			    						f = false;
			    					} else {
			    						testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
			    					}
			   						i++;
			   					}
		   						for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		   							std::string temp = testString;
		    						char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		    						temp.push_back(c);
		    						if(arbre.verifieSiMotExiste(temp)) {
			    						int index = c - 65;
			    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
				    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
				    						str.push_back(c);
											std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
										    v.push_back(etatCourant->numCaseSuivante);		
										    std::vector<char> lettreRestantes;
											for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
											    if(a != k) {
											   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											   	}
											}
											LettreJoueur *l = new LettreJoueur(lettreRestantes);
										    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
									    	pile.push_front(nouvelEtat);
			    						}
		    						}
		    					}
		    				} else if(b.spots[etatCourant->numCaseSuivante-1].letter != 0 && b.spots[etatCourant->numCaseSuivante+1].letter != 0) {
		    					unsigned int i = 1, j = 1;
		    					std::string testString = "", testString1 = "";
		   						bool f = true, ff = true;
		   						while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
			    					if(b.spots[etatCourant->numCaseSuivante+i].letter == 0 || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
			    						f = false;
			    					} else {
			    						testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
			    					}
			    					i++;
			    				}
			    				while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
			    					if(b.spots[etatCourant->numCaseSuivante-j].letter == 0 || (etatCourant->numCaseSuivante-j)%15 == 0) {
			    						ff = false;
			    					} else {
			    						testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-j].letter);
			    					}
			    					j++;
			   					}
			   					for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
			   						std::string temp = testString;
			   						std::string temp1 = testString1;
		   							char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		    						temp1.push_back(c);
		    						temp1+=temp;
		    						if(arbre.verifieSiMotExiste(temp1)) {
			    						int index = c - 65;
			    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
				    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					    					str.push_back(c);
										    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
										    v.push_back(etatCourant->numCaseSuivante);		
										    std::vector<char> lettreRestantes;
											for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
											    if(a != k) {
											   		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											   	}
											}
											LettreJoueur *l = new LettreJoueur(lettreRestantes);
										    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+15, etatCourant->noeud->tabNoeud[index], l, str, v, 1);
									    	pile.push_front(nouvelEtat);
			    						}
		    						}
		    					}
		    				}
	    				} else { //si caseSuivante n'est pas vide on creer un etat avec la lettre de la case:
	    					char letter = b.spots[etatCourant->numCaseSuivante].letter;
	   						int index = letter - 65;
	   						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
	    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		    					str.push_back(letter);
							    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
							    v.push_back(etatCourant->numCaseSuivante);	
							    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+15, etatCourant->noeud->tabNoeud[index], etatCourant->lettresRestantesDuJoueur, str, v, 1);
						    	pile.push_front(nouvelEtat);
						    }
	    				}
	   				}
	   			} else { //horizontal
	    			if(14-(etatCourant->numCaseSuivante-((etatCourant->numCaseSuivante/15)*15)) != 0) { //verifie si la case numero de l'etatCoutant est sur le rebord droit du plateau (c est la cas i ==0 est vrai
	   					if(b.spots[etatCourant->numCaseSuivante].letter == 0) { //(ici vide)verifie si la caseSuivante est vide ou pas vide pour choisir quelle lettre mettre dans noeud de etat
	    					if(b.spots[etatCourant->numCaseSuivante-15].letter == 0 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) { //verifie si autres cases en a droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres et si oui check if word is valid
	    						for(int j=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; j>=0; j--) {
	   								int index = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j] - 65;
	   								if(etatCourant->noeud->tabNoeud[index] != nullptr) {
		    							char s = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[j];
		    							std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
		    							str.push_back(s);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);
									    std::vector<char> lettreRestantes;
									    for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
									    	if(a != j) {
									    		lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
									    	}
									    }
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
								    	pile.push_front(nouvelEtat);
								    }
	    						}
	    					} else if(b.spots[etatCourant->numCaseSuivante-15].letter != 0 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) {	
			   					unsigned int i = 15;
			    				std::string testString = "";
			    				bool f = true;
			    				while(f) { //tant qu il y a des lettres ou que l'on arive au bord du plateau
				   					if(b.spots[etatCourant->numCaseSuivante-i].letter == 0 || etatCourant->numCaseSuivante-i < 0) {
				   						f=false;
				   					} else {
				   						char c = b.spots[etatCourant->numCaseSuivante-i].letter;
				   						testString.insert(0, 1, c);
				   					}
				  					i+=15;
			   					}
			    				for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
			    					std::string temp = testString;
			    					char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
			   						temp.push_back(c);
			    					if(arbre.verifieSiMotExiste(temp)) {
				    					int index = c - 65;
			    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
				    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
						    				str.push_back(c);
										    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
										    v.push_back(etatCourant->numCaseSuivante);		
										    std::vector<char> lettreRestantes;
											for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
											    if(a != k) {
													lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
												}
											}
											LettreJoueur *l = new LettreJoueur(lettreRestantes);
										    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
											pile.push_front(nouvelEtat);
				    					}
			    					}
			    				}
			    			}
	   					} else if(b.spots[etatCourant->numCaseSuivante-15].letter == 0 && b.spots[etatCourant->numCaseSuivante+15].letter != 0) {
	   						unsigned int i = 15;
		    				std::string testString = "";
		    				bool f = true;
		    				while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante+i].letter == 0 || (etatCourant->numCaseSuivante+i) > 225) {
		    						f =false;
		    					} else {
		    						testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
		    					}
			  					i+=15;
		   					}
		    				for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
		    					char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		   						std::string temp = testString;
		   						temp.insert(0, 1, c);
		    					if(arbre.verifieSiMotExiste(temp)) {
			    					int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					    				str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
												lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
										pile.push_front(nouvelEtat);
			    					}
		    					} 
		    				}
	   					} else if(b.spots[etatCourant->numCaseSuivante-15].letter != 0 && b.spots[etatCourant->numCaseSuivante+15].letter != 0) {
	   						unsigned int i = 15, j = 15;
		    				std::string testString = "", testString1 = "";
		    				bool f = true, ff =  true;
		    				while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
		    					if(b.spots[etatCourant->numCaseSuivante-i].letter == 0 || etatCourant->numCaseSuivante-i < 0) {
		    						f = false;
		    					} else {
		    						testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
		    					}
			  					i+=15;
		   					}
		   					while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
			   					if(b.spots[etatCourant->numCaseSuivante+j].letter == 0 || (etatCourant->numCaseSuivante+j) > 225) {
		    						ff = false;
		    					} else {
		    						testString.push_back(b.spots[etatCourant->numCaseSuivante+j].letter);
		    					}
			  					j+=15;
		   					}
		   					for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--){
		    					std::string temp = testString;
		    					std::string temp1 = testString1;
		    					char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
		   						temp.push_back(c);
		   						temp+=temp1;
		    					if(arbre.verifieSiMotExiste(temp)) {
			    					int index = c - 65;
		    						if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    						std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
					    				str.push_back(c);
									    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									    v.push_back(etatCourant->numCaseSuivante);		
									    std::vector<char> lettreRestantes;
										for(unsigned int a=0; a<etatCourant->lettresRestantesDuJoueur->nombreDeLettre(); a++) {
										    if(a != k) {
												lettreRestantes.push_back(etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[a]);
											}
										}
										LettreJoueur *l = new LettreJoueur(lettreRestantes);
									    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
										pile.push_front(nouvelEtat);
			    					}
		    					}
		    				}
	   					} else { //si caseSuivante n'est pas vide on creer un etat avec la lettre de la case:
	    					char letter = b.spots[etatCourant->numCaseSuivante].letter;
		   					int index = letter - 65;
		   					if(etatCourant->noeud->tabNoeud[index] != nullptr) {
			    				std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
			    				str.push_back(letter);
							    std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
							    v.push_back(etatCourant->numCaseSuivante);		
							    Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+1, etatCourant->noeud->tabNoeud[index], etatCourant->lettresRestantesDuJoueur, str, v, 0);
						    	pile.push_front(nouvelEtat);
							}
	   					}
	   				}
	   			}
	   		}
	    }
    }
	if(etatContenantMotDonnantLeMeilleurScore == nullptr) {
		std::cout << "etatContenantMotDonnantLeMeilleurScore = nullptr" << std::endl;
		//si on a pas trouver de mot a jouer on va piocher d'autres lettres ou le jeu s'arrete si il n'y a plus de lettres dans le sac
		impossibleDeContinuerAJouer = true;
	} else { //on retire les lettres que on vient de jouer avec le mot qui a le meilleur score
		std::string mot = creationChaineSansSignePlus(etatContenantMotDonnantLeMeilleurScore);
		for(unsigned int i=0; i<mot.size(); i++) {
			std::vector<char>::iterator it = std::find(lettreDuJoueur.lettreDuJoueur.begin(), lettreDuJoueur.lettreDuJoueur.end(), mot[i]);
			if (it != lettreDuJoueur.lettreDuJoueur.end()) {
				int index = std::distance(lettreDuJoueur.lettreDuJoueur.begin(), it);
				if(b.spots[etatContenantMotDonnantLeMeilleurScore->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[i]].letter == 0) {
					lettreDuJoueur.lettreDuJoueur.erase(lettreDuJoueur.lettreDuJoueur.begin()+index, lettreDuJoueur.lettreDuJoueur.begin()+index+1);
				}
			}
		}
		std::cout << "chaine du meilleure score = " << etatContenantMotDonnantLeMeilleurScore->chaineDeLettresDejaParcouruPourArriveACetEtat << std::endl;
		std::cout << "score = " << scoreMax << " pour le mot: " << mot << std::endl;
		b.placerUnMotSurLePlateau(ss, etatContenantMotDonnantLeMeilleurScore);
		b.load(ss);
		std::cout << b << std::endl;
	}
}
  return 0;
}
