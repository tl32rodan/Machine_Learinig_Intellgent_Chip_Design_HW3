#include "systemc.h"
#include "Processing_Element.h"
#include "Router.h"
#include <iostream>


using namespace std;

SC_MODULE(Tile) {
	
	sc_in <bool> clk;
	sc_in <bool> reset;
	
	
    
    SC_CTOR(Tile) {
		
    }
};
