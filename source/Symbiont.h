#ifndef SYMBIONT_H
#define SYMBIONT_H

#include "../../Empirical/include/emp/math/Random.hpp"
#include "../../Empirical/include/emp/tools/string_utils.hpp"
#include "SymWorld.h"
#include "Host.h"
#include <set>
#include <iomanip> // setprecision
#include <sstream> // stringstream


class Symbiont {
protected:  
  double interaction_val;
  double points;
  double sym_h_res = 100;
  bool h_trans = true;
  double mut_rate = 0.002;
  emp::Random &random;
  SymWorld &my_world;
  Host &my_host;

public:

  Symbiont(emp::Random &_random, SymWorld &_world, Host &_host, double _intval=0.0, double _points = 0.0, double _h_res = 100.0, bool _h_trans = true, double _mut_rate = 0.002) : interaction_val(_intval), points(_points), random(_random), my_world(_world), my_host(_host), sym_h_res(_h_res), h_trans(_h_trans), mut_rate(_mut_rate) {;}
  Symbiont(const Symbiont &) = default;
  Symbiont(Symbiont &&) = default;


  Symbiont & operator=(const Symbiont &) = default;
  Symbiont & operator=(Symbiont &&) = default;

  double GetIntVal() const {return interaction_val;}
  double GetPoints() {return points;}
  //  std::set<int> GetResTypes() const {return res_types;}



  void SetIntVal(double _in) { interaction_val = _in;}
  void SetPoints(double _in) { points = _in;}
  void AddPoints(double _in) { points += _in;}
  void SetHost(Host& _in) {my_host = _in;}
  //void SetResTypes(std::set<int> _in) {res_types = _in;}


  //TODO: change everything to camel case
  void mutate(){
    interaction_val += random.GetRandNormal(0.0, mut_rate);
    if(interaction_val < -1) interaction_val = -1;
    else if (interaction_val > 1) interaction_val = 1;
  }

  void process(size_t location) {
    if (h_trans) { //non-lytic horizontal transmission enabled
      if(GetPoints() >= sym_h_res) {
        // symbiont reproduces independently (horizontal transmission) if it has >= 100 resources (by default)
        // new symbiont in this host with mutated value
        SetPoints(0); //TODO: test just subtracting points instead of setting to 0
        Symbiont * sym_baby = new Symbiont(*this);
        sym_baby->SetPoints(0);
        sym_baby->mutate();
        mutate();
        
        my_world->SymDoBirth(sym_baby, location)

      }
    }
  }
  


};

std::string PrintSym(Symbiont  org){
  if (org.GetPoints() < 0) return "-";
  double out_val = org.GetIntVal();  
  
  // this prints the symbiont with two decimal places for easier reading
  std::stringstream temp;
  temp << std::fixed << std::setprecision(2) << out_val;
  std::string formattedstring = temp.str();
  return formattedstring;
  
  // return emp::to_string(out_val);  // creates a string without specifying format
}//Symbiont
#endif