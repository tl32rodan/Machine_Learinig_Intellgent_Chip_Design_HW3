#include "systemc.h"
#include "Tile.h"
#include <iostream>


using namespace std;

SC_MODULE(NoC) {
	
	sc_in <bool> clk;
	sc_in <bool> reset;
	
	sc_out<sc_lv<32> > data_out1,data_out2,data_out3,data_out4,data_out5,data_out6,data_out7,data_out8,data_out9;
	
	void noc();

    SC_CTOR(NoC) {
		SC_METHOD(noc);
		sensitive << clk.pos();
    }
};
