#pragma once

#include "spot.hpp"
#include "Etat.hpp"

#include <iostream>
#include <vector>
#include <string>

/* Scrabble board */

struct Board {

  std::vector<unsigned int> caseQueOnPeutRemplir(); 
  void placerUnMotSurLePlateau(std::stringstream & ss, Etat *e);


  //default initialization of a scrabble board
  Board() ;

  //i/o to files
  void save(std::ostream& out) ;
  void load(std::istream& in) ;

  //access to the spots by coordinates
  Spot operator()(unsigned char l, unsigned char c) const ;
  Spot& operator()(unsigned char l, unsigned char c) ;

  //spots are public, and can therefore also be accessed b index
  Spot spots[225] ;
  bool flags[225];
} ;

//board display on the console
std::ostream& operator<<(std::ostream& out, const Board& b) ;
