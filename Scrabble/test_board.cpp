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
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

std::string creationChaineSansSignePlus(Etat *e) {
	std::string mot = "";
	for(unsigned int l=0; l<e->chaineDeLettresDejaParcouruPourArriveACetEtat.size(); l++) {
		char c = e->chaineDeLettresDejaParcouruPourArriveACetEtat[l];
		if(c != '+') { 
			mot.push_back(c); 
		}
	}
	return mot;
}

int main() {
	srand(time(NULL));

	//initialisation de l'arbre contenant toutes les versions de mots avec le signe '+''
	ArbreDico arbre;
	arbre.remplirArbre();
	// arbre.afficherArbre(arbre.racine, "");

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

	bool impossibleDeContinuerAJouer = false; //devient vrai si le joueur ne peut jouer aucune de ses lettres
	unsigned int scoreMax = 0; //calcul du meilleur mot possible à chaque tour
	unsigned int scoreTotal = 0; //score total de tous les mots du plateau

	//on commence le jeu
	while(!sac->estVide() && (!impossibleDeContinuerAJouer || lettreDuJoueur.nombreDeLettre() > 0)) { 

		if(lettreDuJoueur.nombreDeLettre() < 7) {
			lettreDuJoueur.rechargerLesLettresDuJoueur(sac);
		} 

		//si aucunes lettres ne peuvent être jouées pour créer un mot valide on pioche de nouvelles lettres
		//et on remet les autres lettres dans le sac
		if(impossibleDeContinuerAJouer) {
			std::cout << "Pas de coup possible, on pioche donc de nouvelles lettres" << std::endl;
			std::vector<char> lettresARemettreDansSac;
			for(int l=0; l<lettreDuJoueur.nombreDeLettre(); l++) {
				lettresARemettreDansSac.push_back(lettreDuJoueur.lettreDuJoueur[l]);
			}
			lettreDuJoueur.lettreDuJoueur.clear();
			lettreDuJoueur.rechargerLesLettresDuJoueur(sac);
			sac->reremplirLeSac(lettresARemettreDansSac);
			if(lettreDuJoueur.nombreDeLettre() == 7) {
				impossibleDeContinuerAJouer = false;
			}
		}

		// affiche les lettres du joueur
		std::cout << std::endl;
		std::cout << "lettres: ";
		for(int n=0; n<lettreDuJoueur.lettreDuJoueur.size(); n++) {
			std::cout << lettreDuJoueur.lettreDuJoueur[n] << " ";
		}
		std::cout << std::endl;
		
		sleep(2);

		//on recupère les cases vides a côtê des cases occupées:
		std::vector<unsigned int> casesVidesACoteDeCasesOccupees = b.caseQueOnPeutRemplir();

		//la pile pour mettre les etats:
		std::deque<Etat *> pile;    

		//construit 2 etats initiaux (un pour horizontal et un pour vertical) 
		//pour chaque case vide sur lesquelles on peut jouer un mot et on les met dans la pile
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

		//tant que la pile d'etats n'est pas vide on continue à chercher tous les états possibles
		while(!pile.empty()) { 

			Etat *etatCourant = pile.front();
			pile.pop_front();
			if(etatCourant->noeud != nullptr) {
				if(etatCourant->noeud->finale) {
					std::vector<unsigned int> cases = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
					sort(cases.begin(), cases.end()); 
					// on verifie qu'il n'y a pas de lettre sur le plateau après la fin de la chaine qu'on va tester 
					// ou si l'etat est horizontal on verifie qu'on est au bout de la ligne et donc que la prochaine case n'importe pas si elle est vide ou non
					if((etatCourant->numCaseSuivante >= 0 && b.spots[etatCourant->numCaseSuivante].letter == 0) || (etatCourant->numCaseSuivante < 0 && etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat[etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat.size()-1] == '+') || (etatCourant->sensDexploration == 0 && ((cases[0]%15 == 0 && etatCourant->numCaseSuivante == cases[0]-1) || (14-(cases[cases.size()-1]-((cases[cases.size()-1]/15)*15)) == 0 && etatCourant->numCaseSuivante-1  == cases[cases.size()-1])))) { 
						//on calcul le score de chaque etat qui a un mot valide et compare le score avec le score max
						std::string mot = creationChaineSansSignePlus(etatCourant);
						unsigned int score = 0, wordBonnus2 = 0, wordBonnus3 = 0, septLettresBonus = 0;;
						for(unsigned int p=0; p<mot.size(); p++) {
							char c = mot[p];
							score+=sac->donnerLeNombreDePoint(c);
							int ind = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[p];
							if(b.spots[ind].letter == 0) {
								septLettresBonus++;
								if(b.spots[ind].bonus.word_factor == 3) {
									wordBonnus3++;
								}
								if(b.spots[ind].bonus.word_factor == 2) {
									wordBonnus2++;
								}
								if(b.spots[ind].bonus.letter_factor == 2) {
									score+=(sac->donnerLeNombreDePoint(c));
								}
								if(b.spots[ind].bonus.letter_factor == 3) {
									score+=(sac->donnerLeNombreDePoint(c))*2;
								}
							}		    				
						}
						unsigned int scoreInitial = score; //on applique les bonus mot si il y en a
						if(wordBonnus2 > 0) {
							score+=scoreInitial*wordBonnus2*2;
						}
						if(wordBonnus3 > 0) {
							score+=scoreInitial*wordBonnus3*3;
						}
						if(septLettresBonus == 7) { //si on arrive a placer les 7 lettres du jeu on a un bonus de 50 points
							score+=50;
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
					if(etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat[t] == '+') { 
						checkPlusSign = true; 
					}
				}
				if((etatCourant->numCaseSuivante >= 0 && etatCourant->numCaseSuivante < 225) || (etatCourant->sensDexploration && !checkPlusSign)) {
					if(!checkPlusSign) { // etats sans signe '+'
						if(etatCourant->sensDexploration) { // etats verticaux sans signe '+'
							if((etatCourant->numCaseSuivante >= 0 && b.spots[etatCourant->numCaseSuivante].letter == 0) || (etatCourant->numCaseSuivante < 0 && etatCourant->numCaseSuivante+15 < 15)) { //verifie si la caseSuivante est vide ou pas vide pour choisir quelle lettre mettre dans noeud de etat
								//on créer un état avec un signe '+'
								if(etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() != 0 && etatCourant->noeud->tabNoeud[26] != nullptr) { 
									char s = '+';
									std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
									str.push_back(s);
									Etat *nouvelEtat = new Etat(etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[0]+15, etatCourant->noeud->tabNoeud[26], etatCourant->lettresRestantesDuJoueur, str, etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine, 1);
									pile.push_front(nouvelEtat);
								}
								//on verifie si les cases à droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres ou pas
								if(((etatCourant->numCaseSuivante-1 >= 0 && b.spots[etatCourant->numCaseSuivante-1].letter == 0) || (14-((etatCourant->numCaseSuivante-1)-(((etatCourant->numCaseSuivante-1)/15)*15)) == 0 && etatCourant->numCaseSuivante%15 == 0) || (etatCourant->numCaseSuivante-1 < 0 && etatCourant->numCaseSuivante == 0)) 
									&& ((etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante+1].letter == 0) || ((etatCourant->numCaseSuivante+1)%15 == 0 && (14-((etatCourant->numCaseSuivante)-(((etatCourant->numCaseSuivante)/15)*15)) == 0)) || (etatCourant->numCaseSuivante+1 > 225 && etatCourant->numCaseSuivante == 224))) { 
									//on créer tous les autres etats possibles avec les lettres restantes du joueur
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
								} else if(((etatCourant->numCaseSuivante-1 >= 0 && b.spots[etatCourant->numCaseSuivante-1].letter == 0) || (etatCourant->numCaseSuivante-1 < 0 && etatCourant->numCaseSuivante == 0)) && (etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante+1].letter != 0)) {
									unsigned int i = 1;
									std::string testString = "";
									bool f = true;
									while(f) { //construit la chaine a verifier pour voir si le mot est valide
										//si il n'y a plus de lettres ou si on est arrivé au bord du plateau
										if(etatCourant->numCaseSuivante+i > 224 || (etatCourant->numCaseSuivante+i < 225 && b.spots[etatCourant->numCaseSuivante+i].letter == 0) || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
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
										int index = c - 65;
										if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) { //si mot est valide on construit l'etat 
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
								} else if((etatCourant->numCaseSuivante-1 >= 0 && etatCourant->numCaseSuivante-1 < 225 && b.spots[etatCourant->numCaseSuivante-1].letter != 0) && ((etatCourant->numCaseSuivante+1 > 224 && etatCourant->numCaseSuivante == 224) || (etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante+1].letter == 0))) {
									unsigned int i = 1;
									std::string testString = "";
									bool f = true;
									while(f) { //construit la chaine a verifier pour voir si le mot est valide tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante-i < 0 || etatCourant->numCaseSuivante-i > 224 || (etatCourant->numCaseSuivante-i >= 0 && etatCourant->numCaseSuivante-i < 225 && b.spots[etatCourant->numCaseSuivante-i].letter == 0) || (etatCourant->numCaseSuivante-i)%15 == 0) {
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
										int index = c - 65;
										if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
								} else if(etatCourant->numCaseSuivante-1 >= 0 && etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante-1].letter != 0 && b.spots[etatCourant->numCaseSuivante+1].letter != 0) {
									unsigned int i = 1, j= 1;
									std::string testString = "", testString1 = "";
									bool f = true, ff = true;
									while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante+i > 224 || (etatCourant->numCaseSuivante+i < 225 && b.spots[etatCourant->numCaseSuivante+i].letter == 0) || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
											f = false;
										} else {
											testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
										}
										i++;
									}
									while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante-j < 0 || etatCourant->numCaseSuivante-j > 224 || (etatCourant->numCaseSuivante-j >= 0 && b.spots[etatCourant->numCaseSuivante-j].letter == 0) || (etatCourant->numCaseSuivante-j)%15 == 0) {
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
										int index = c - 65;
										if(arbre.verifieSiMotExiste(temp1) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
						} else { //horizontal sans '+'
							if(b.spots[etatCourant->numCaseSuivante].letter == 0) { //verifie si la caseSuivante est vide ou pas vide pour choisir quelle lettre mettre dans noeud de etat
								//On créer un état avec un signe '+'
								if(etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() != 0 && etatCourant->noeud->tabNoeud[26] != nullptr) {
									char s = '+';
									std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
									str.push_back(s);
									Etat *nouvelEtat = new Etat(etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine[0]+1, etatCourant->noeud->tabNoeud[26], etatCourant->lettresRestantesDuJoueur, str, etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine, 0);
									pile.push_front(nouvelEtat);
								}
								//verifie si on a atteint le bout de la ligne horizontale ou pas 
								if((14-(etatCourant->numCaseSuivante-((etatCourant->numCaseSuivante/15)*15)) != 0) || (14-(etatCourant->numCaseSuivante-((etatCourant->numCaseSuivante/15)*15)) == 0 && etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() == 0)) {
									//verifie si autres cases en a droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres ou non
									if(((etatCourant->numCaseSuivante-15 >=0 && b.spots[etatCourant->numCaseSuivante-15].letter == 0) || (etatCourant->numCaseSuivante-15 < 0 && etatCourant->numCaseSuivante < 15)) && ((etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) || (etatCourant->numCaseSuivante+15 > 225 && etatCourant->numCaseSuivante > 209))) { 
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
									} else if(((etatCourant->numCaseSuivante-15 >= 0 && b.spots[etatCourant->numCaseSuivante-15].letter != 0)) && ((etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) || (etatCourant->numCaseSuivante+15 > 225 && etatCourant->numCaseSuivante > 209))) {
										unsigned int i = 15;
										std::string testString = "";
										bool f = true;
										while(f) { //tant qu il y a des lettres ou que l'on arive au bord du plateau
											if(etatCourant->numCaseSuivante-i > 224 || etatCourant->numCaseSuivante-i < 0 || (etatCourant->numCaseSuivante-i >= 0 && etatCourant->numCaseSuivante-i < 225 && b.spots[etatCourant->numCaseSuivante-i].letter == 0)) {
												f=false;
											} else {
												testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
											}
											i+=15;
										}
										for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
											std::string temp = testString;
											char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
											temp.push_back(c);
											int index = c - 65;
											if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) { 
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
									} else if(((etatCourant->numCaseSuivante-15 >= 0 && b.spots[etatCourant->numCaseSuivante-15].letter == 0) || (etatCourant->numCaseSuivante-15 < 0 && etatCourant->numCaseSuivante < 15)) && (etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter != 0)) {
										unsigned int i = 15;
										std::string testString = "";
										bool f = true;
										while(f) { //construit la chaine a verifier. Tant qu il y a des lettres ou que l'on arrive au bord du plateau
											if(etatCourant->numCaseSuivante+i > 224 || (etatCourant->numCaseSuivante+i < 225 && b.spots[etatCourant->numCaseSuivante+i].letter == 0)) {
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
											int index = c - 65;
											if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
									} else if(etatCourant->numCaseSuivante-15 >= 0 && etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante-15].letter != 0 && b.spots[etatCourant->numCaseSuivante+15].letter != 0) {
										unsigned int i = 15, j = 15;
										std::string testString = "", testString1 = "";
										bool f = true, ff = true;
										while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
											if(etatCourant->numCaseSuivante-i < 0 || etatCourant->numCaseSuivante-i > 224 || (etatCourant->numCaseSuivante-i >= 0 && etatCourant->numCaseSuivante-i < 225 && b.spots[etatCourant->numCaseSuivante-i].letter == 0)) {
												f = false;
											} else {
												testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
											}
											i+=15;
										}
										while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
											if((etatCourant->numCaseSuivante+j) > 224 || (etatCourant->numCaseSuivante+j < 225 && b.spots[etatCourant->numCaseSuivante+j].letter == 0)) {
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
											int index = c - 65;
											if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
								if(etatCourant->noeud->tabNoeud[index] != nullptr && ((14-(etatCourant->numCaseSuivante-((etatCourant->numCaseSuivante/15)*15)) != 0) || (14-(etatCourant->numCaseSuivante-((etatCourant->numCaseSuivante/15)*15)) == 0 && etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() == 0))) {
									std::string str = etatCourant->chaineDeLettresDejaParcouruPourArriveACetEtat;
									str.push_back(letter);
									std::vector<unsigned int> v = etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine;
									v.push_back(etatCourant->numCaseSuivante);	
									Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante-1, etatCourant->noeud->tabNoeud[index], etatCourant->lettresRestantesDuJoueur, str, v, 0);
									pile.push_front(nouvelEtat);
								}
							}
						}
					} else { // etats contenant le signe '+'
						if(etatCourant->sensDexploration) { //vertical avec signe '+'
							if(b.spots[etatCourant->numCaseSuivante].letter == 0) { 
								// verifie si les cases à droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres
									if(((etatCourant->numCaseSuivante-1 >= 0 && b.spots[etatCourant->numCaseSuivante-1].letter == 0) || (14-((etatCourant->numCaseSuivante-1)-(((etatCourant->numCaseSuivante-1)/15)*15)) == 0 && etatCourant->numCaseSuivante%15 == 0) || (etatCourant->numCaseSuivante-1 < 0 && etatCourant->numCaseSuivante == 0)) 
										&& ((etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante+1].letter == 0) || ((etatCourant->numCaseSuivante+1)%15 == 0 && (14-((etatCourant->numCaseSuivante)-(((etatCourant->numCaseSuivante)/15)*15)) == 0)) || (etatCourant->numCaseSuivante+1 > 225 && etatCourant->numCaseSuivante == 224))) {
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
								} else if(((etatCourant->numCaseSuivante-1 >= 0 && b.spots[etatCourant->numCaseSuivante-1].letter == 0) || (etatCourant->numCaseSuivante-1 < 0 && etatCourant->numCaseSuivante == 0)) && (etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante+1].letter != 0)) {
									unsigned int i = 1;
									std::string testString = "";
									bool f = true;
									while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante+i > 224 || (etatCourant->numCaseSuivante+i < 225 && b.spots[etatCourant->numCaseSuivante+i].letter == 0) || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
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
										int index = c - 65;
										if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
								} else if((etatCourant->numCaseSuivante-1 >= 0 && b.spots[etatCourant->numCaseSuivante-1].letter != 0) && ((etatCourant->numCaseSuivante+1 > 225 && etatCourant->numCaseSuivante == 224) || b.spots[etatCourant->numCaseSuivante+1].letter == 0)) {
									unsigned int i = 1;
									std::string testString = "";
									bool f = true;
									while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante-i < 0 || etatCourant->numCaseSuivante-i > 224 || (etatCourant->numCaseSuivante-i >= 0 && etatCourant->numCaseSuivante-i < 225 && b.spots[etatCourant->numCaseSuivante-i].letter == 0) || (etatCourant->numCaseSuivante-i)%15 == 0) {
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
										int index = c - 65;
										if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
								} else if(etatCourant->numCaseSuivante-1 >= 0 && etatCourant->numCaseSuivante+1 < 225 && b.spots[etatCourant->numCaseSuivante-1].letter != 0 && b.spots[etatCourant->numCaseSuivante+1].letter != 0) {
									unsigned int i = 1, j = 1;
									std::string testString = "", testString1 = "";
									bool f = true, ff = true;
									while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante+i > 224 || (etatCourant->numCaseSuivante+i < 225 && b.spots[etatCourant->numCaseSuivante+i].letter == 0) || 14-((etatCourant->numCaseSuivante+i)-(((etatCourant->numCaseSuivante+i)/15)*15)) == 0) {
											f = false;
										} else {
											testString.push_back(b.spots[etatCourant->numCaseSuivante+i].letter);
										}
										i++;
									}
									while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
										if(etatCourant->numCaseSuivante-j < 0 || etatCourant->numCaseSuivante-j > 224 || (etatCourant->numCaseSuivante-j >= 0 && etatCourant->numCaseSuivante-j < 225 && b.spots[etatCourant->numCaseSuivante-j].letter == 0) || (etatCourant->numCaseSuivante-j)%15 == 0) {
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
										int index = c - 65;
										if(arbre.verifieSiMotExiste(temp1) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
						} else { //horizontal avec signe '+'
							if(b.spots[etatCourant->numCaseSuivante].letter == 0) { 
								// verifie si on arrive au bout de la ligne horizontale ou non 
								if((etatCourant->numCaseSuivante%15 != 0) || (etatCourant->numCaseSuivante%15 == 0 && etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() == 0)) {
									//verifie si autres cases à droite ou a gauche (haut ou en bas pour horizontal) contiennent des lettres ou pas 
									if(((etatCourant->numCaseSuivante-15 >=0 && b.spots[etatCourant->numCaseSuivante-15].letter == 0) || (etatCourant->numCaseSuivante-15 < 0 && etatCourant->numCaseSuivante < 15)) && ((etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) || (etatCourant->numCaseSuivante+15 > 225 && etatCourant->numCaseSuivante > 209))) { 
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
									} else if(((etatCourant->numCaseSuivante-15 >= 0 && b.spots[etatCourant->numCaseSuivante-15].letter != 0)) && ((etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter == 0) || (etatCourant->numCaseSuivante+15 > 225 && etatCourant->numCaseSuivante > 209))) {
										unsigned int i = 15;
										std::string testString = "";
										bool f = true;
										while(f) { //tant qu il y a des lettres ou que l'on arive au bord du plateau
											if(etatCourant->numCaseSuivante-i < 0 || etatCourant->numCaseSuivante-i > 224 || (etatCourant->numCaseSuivante-i >= 0 && etatCourant->numCaseSuivante-i < 225 && b.spots[etatCourant->numCaseSuivante-i].letter == 0)) {
												f=false;
											} else {
												testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
											}
											i+=15;
										}
										for(int k=etatCourant->lettresRestantesDuJoueur->nombreDeLettre()-1; k>=0; k--) {
											std::string temp = testString;
											char c = etatCourant->lettresRestantesDuJoueur->lettreDuJoueur[k];
											temp.push_back(c);
											int index = c - 65;
											if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
												Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
												pile.push_front(nouvelEtat);
											}
										}
									} else if(((etatCourant->numCaseSuivante-15 >= 0 && b.spots[etatCourant->numCaseSuivante-15].letter == 0) || (etatCourant->numCaseSuivante-15 < 0 && etatCourant->numCaseSuivante < 15)) && (etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter != 0)) {
										unsigned int i = 15;
										std::string testString = "";
										bool f = true;
										while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
											if(etatCourant->numCaseSuivante+i > 224 || (etatCourant->numCaseSuivante+i < 225 && b.spots[etatCourant->numCaseSuivante+i].letter == 0)) {
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
											int index = c - 65;
											if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
												Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
												pile.push_front(nouvelEtat);
											} 
										}
									} else if((etatCourant->numCaseSuivante-15 >= 0 && b.spots[etatCourant->numCaseSuivante-15].letter != 0) && (etatCourant->numCaseSuivante+15 < 225 && b.spots[etatCourant->numCaseSuivante+15].letter != 0)) {
										unsigned int i = 15, j = 15;
										std::string testString = "", testString1 = "";
										bool f = true, ff =  true;
										while(f) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
											if(etatCourant->numCaseSuivante-i < 0 || etatCourant->numCaseSuivante-i > 224 || (etatCourant->numCaseSuivante-i >= 0 && etatCourant->numCaseSuivante-i < 225 && b.spots[etatCourant->numCaseSuivante-i].letter == 0)) {
												f = false;
											} else {
												testString.insert(0, 1, b.spots[etatCourant->numCaseSuivante-i].letter);
											}
											i+=15;
										}
										while(ff) { //tant qu il y a des lettres ou que l'on arrive au bord du plateau
											if((etatCourant->numCaseSuivante+j) > 224 || (etatCourant->numCaseSuivante+j < 225 && b.spots[etatCourant->numCaseSuivante+j].letter == 0)) {
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
											int index = c - 65;
											if(arbre.verifieSiMotExiste(temp) && etatCourant->noeud->tabNoeud[index] != nullptr) {
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
												Etat *nouvelEtat = new Etat(etatCourant->numCaseSuivante+1, etatCourant->noeud->tabNoeud[index], l, str, v, 0);
												pile.push_front(nouvelEtat);
											}
										}
									}
								} 
							} else { //si caseSuivante n'est pas vide on créer un etat avec la lettre de la case:
								char letter = b.spots[etatCourant->numCaseSuivante].letter;
								int index = letter - 65;
								if(etatCourant->noeud->tabNoeud[index] != nullptr && ((etatCourant->numCaseSuivante%15 != 0) || (etatCourant->numCaseSuivante%15 == 0 && etatCourant->numerosDesCasesSurLesquelsSontLesLettresDeLaChaine.size() == 0))) {
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
		//si on a pas trouver de mot à jouer on va piocher d'autres lettres ou le jeu s'arrete si il n'y a plus suffisamment de lettres dans le sac
		if(etatContenantMotDonnantLeMeilleurScore == nullptr) {
			std::cout << "Aucun coup ne peut être joué avec ce jeu de lettres." << std::endl;
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
			std::cout << "On joue la chaine: " << etatContenantMotDonnantLeMeilleurScore->chaineDeLettresDejaParcouruPourArriveACetEtat << " qui donne un score de: " << scoreMax << std::endl;
			scoreTotal+=scoreMax;
			b.placerUnMotSurLePlateau(ss, etatContenantMotDonnantLeMeilleurScore);
			b.load(ss);
			std::cout << b;
			std::cout << "Score total = " << scoreTotal;
		}
	}
	std::cout << std::endl;
	std::cout << "Plus de lettres à jouer, le jeu est terminé." << std::endl;
	return 0;
}
