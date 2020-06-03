#include "Monitor.h"
#include "NoC.h"
#include "clockreset.h"
#include <iostream>

using namespace std;

int sc_main(int argc,char* argv[])
{
	sc_signal<bool>clk, reset;
	sc_signal<sc_lv<32> >  data_out1,data_out2,data_out3,data_out4,data_out5,data_out6,data_out7,data_out8,data_out9;
	
	NoC noc("noc");
	Monitor mon("mon");
	Clock clock("clock",5,50);
	Reset rst("reset",20);
	
	clock(clk);
	rst(reset);
	noc(clk,reset,data_out1,data_out2,data_out3,data_out4,data_out5,data_out6,data_out7,data_out8,data_out9);
	mon(clk,reset,data_out1,data_out2,data_out3,data_out4,data_out5,data_out6,data_out7,data_out8,data_out9);
	
	
	sc_start(500,SC_NS);
	return 0;

}











