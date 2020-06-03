#include "systemc.h"
#include <iostream>

using namespace std;

SC_MODULE(Monitor)
{	
	sc_in_clk clock;
	sc_in<bool> reset;
	sc_in<sc_lv<32> > data_out1,data_out2,data_out3,data_out4,data_out5,data_out6,data_out7,data_out8,data_out9;
	

	void monitor();
	
	SC_CTOR(Monitor)
	{	
		SC_METHOD(monitor);
		sensitive << clock.pos();
	}
};

