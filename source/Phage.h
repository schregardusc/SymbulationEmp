#ifndef PHAGE_H
#define PHAGE_H

#include "Symbiont.h"

class Phage: public Symbiont {
protected:
  double burst_timer = 0;
  bool lysis = true;
  double burst_time = 60;
  double sym_lysis_res = 15;


public:

  Phage(emp::Random _random, SymWorld _world, Host _host, double _intval=0.0,
    double _points = 0.0, double _h_res = 100.0, bool _h_trans = true, 
    double _mut_rate = 0.002, double _bt = 60, bool _lysis = true, double _l_res = 15) 
    : interaction_val(_intval), points(_points), random(_random), my_world(_world), 
    my_host(_host), sym_h_res(_h_res), h_trans(_h_trans), mut_rate(_mut_rate), burst_time(_bt),
    sym_lysis_res(_l_res) {}
  Phage(const Phage &) = default;
  Phage(Phage &&) = default;

  double GetBurstTimer() {return burst_timer;}
  void IncBurstTimer() {

    burst_timer += random.GetRandNormal(1.0, 0.5);
    
  }
  void SetBurstTimer(int _in) {burst_timer = _in;}


  double GetIntVal() const {
    return -1; //non-lysogenized lytic phage shuts down host reproduction if possible
    
  }

  void process(size_t location) {
    if(lysis) { //lysis enabled, checking for lysis
      if(GetBurstTimer() >= burst_time) { //time to lyse!
        emp::vector<Symbiont>& repro_syms = my_host->GetReproSyms();
        //Record the burst size
	      // update this for my_world: data_node_burst_size -> AddDatum(repro_syms.size());
        for(size_t r=0; r<repro_syms->size(); r++) {
          my_world->SymDoBirth(repro_syms[r], location);
        }
        my_world->DoDeath(location);
        
      } else {
        IncBurstTimer();
        if(sym_lysis_res == 0) {
          std::cout << "Lysis with a sym_lysis_res of 0 leads to an
           infinite loop, please change" << std::endl;
          std::exit(1);
        }
        while(GetPoints() >= sym_lysis_res) {
          Symbiont *sym_baby = new Phage(this);
          sym_baby->SetPoints(0);
          sym_baby->mutate();
          mutate(); // TODO: test removing this since it's weird
          my_host->AddReproSym(*sym_baby);
          SetPoints(GetPoints() - sym_lysis_res);
        }
      }
    }
  }

};

#endif