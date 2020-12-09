#ifndef HOST_H
#define HOST_H

#include "../../Empirical/include/emp/math/Random.hpp"
#include "../../Empirical/include/emp/tools/string_utils.hpp"
#include <set>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include "Organism.h"


class Host: public Organism {
private:
  double interaction_val;
  emp::vector<Organism> syms; //change to emp::vector<emp::Ptr<Symbiont>>
  emp::vector<Organism> repro_syms;
  std::set<int> res_types;
  double points;
  double mut_rate = 0.002;
  emp::Random &random;

public:
  Host(emp::Random &_random, double _intval =0.0, emp::vector<Organism> _syms = {},emp::vector<Organism> _repro_syms = {}, std::set<int> _set = std::set<int>(), double _points = 0.0) : random(_random), interaction_val(_intval), syms(_syms), res_types(_set), points(_points) { ; }
  Host(const Host &) = default;
  Host(Host &&) = default;

  Host & operator=(const Host &) = default;
  Host & operator=(Host &&) = default;
  bool operator==(const Host &other) const { return (this == &other);}
  bool operator!=(const Host &other) const {return !(*this == other);}


  double GetIntVal() const { return interaction_val;}
  emp::vector<Organism>& GetSymbionts() { return syms;}
  emp::vector<Organism>& GetReproSymbionts() {return repro_syms;}
  std::set<int> GetResTypes() const { return res_types;}
  double GetPoints() { return points;}


  void SetIntVal(double _in) {interaction_val = _in;}
  void SetSymbionts(emp::vector<Organism> _in) {syms = _in;}
  void SetResTypes(std::set<int> _in) {res_types = _in;}
  void SetPoints(double _in) {points = _in;}


  void AddPoints(double _in) {points += _in;}


  void AddSymbionts(Organism _in, int sym_limit) {
    if(syms.size() < sym_limit){
      syms.push_back(_in);
    }
  }  
  void AddReproSym(Organism _in) {repro_syms.push_back(_in);}
  
  bool HasSym() {
    return syms.size() != 0;
  }



  void mutate(){
    interaction_val += random.GetRandNormal(0.0, mut_rate);
    if(interaction_val < -1) interaction_val = -1;
    else if (interaction_val > 1) interaction_val = 1;
  }
  
  void DistribResources(double resources, double synergy) { 
    double hostIntVal = interaction_val; //using private variable because we can
    
    //In the event that the host has no symbionts, the host gets all resources not allocated to defense or given to absent partner.
    if(syms.empty()) {

      if(hostIntVal >= 0){
	double spent = resources * hostIntVal;
        this->AddPoints(resources - spent);
      }
      else {
        double hostDefense = -1.0 * hostIntVal * resources;
        this->AddPoints(resources - hostDefense);
      }
      return; //This concludes resource distribution.
    }

    //Otherwise, split resources into equal chunks for each symbiont
    int num_sym = syms.size();
    double sym_piece = (double) resources / num_sym;

    for(size_t i=0; i < syms.size(); i++){
      double symIntVal = syms[i].GetIntVal();
     
      double hostPortion = 0.0;
      double hostDonation = 0.0;
      double symPortion = 0.0;
      double symReturn = 0.0;
      double bonus = synergy; 

  

      if (hostIntVal >= 0 && symIntVal >= 0)  {  
        hostDonation = sym_piece * hostIntVal;
        hostPortion = sym_piece - hostDonation;  
            
        symReturn = (hostDonation * symIntVal) * bonus;  
        symPortion = hostDonation - (hostDonation * symIntVal);

        hostPortion += symReturn;
            
        syms[i].AddPoints(symPortion);
        this->AddPoints(hostPortion);
      
      } else if (hostIntVal <= 0 && symIntVal < 0) {
        double hostDefense = -1.0 * (hostIntVal * sym_piece);
        double remainingResources = 0.0;
        remainingResources = sym_piece - hostDefense;
             
        // if both are hostile, then the symbiont must be more hostile than in order to gain any resources 
        if (symIntVal < hostIntVal) { //symbiont overcomes host's defenses
          double symSteals = (hostIntVal - symIntVal) * remainingResources;

          symPortion = symSteals;
          hostPortion = remainingResources - symSteals;

        } else { // symbiont cannot overcome host's defenses         
          symPortion = 0.0;
          hostPortion = remainingResources;
        }

        syms[i].AddPoints(symPortion);
        this->AddPoints(hostPortion);
         
      } else if (hostIntVal > 0 && symIntVal < 0) {
        hostDonation = hostIntVal * sym_piece;
        hostPortion = sym_piece - hostDonation;
        sym_piece = sym_piece - hostDonation;
          
        double symSteals = -1.0 * (sym_piece * symIntVal);
        hostPortion = hostPortion - symSteals;
        symPortion = hostDonation + symSteals;
          
        syms[i].AddPoints(symPortion);
        this->AddPoints(hostPortion);
    
    
      } else if (hostIntVal < 0 && symIntVal >= 0) {
        double hostDefense = -1.0 * (hostIntVal * sym_piece);
        hostPortion = sym_piece - hostDefense;
          
        // symbiont gets nothing from antagonistic host
        symPortion = 0.0;
          
        syms[i].AddPoints(symPortion);
        this->AddPoints(hostPortion);
      } else {

        //TODO: add error here
        std::cout << "This should never happen." << std::endl;

      }
      
    } //end syms[i] for loop
  
  } //end DistribResources

  void Process(emp::Random &random, double resources_per_host_per_update, double synergy) {
    //Currently just wrapping to use the existing function
    DistribResources(resources_per_host_per_update, synergy); 
  }
  
};//Host

#endif
