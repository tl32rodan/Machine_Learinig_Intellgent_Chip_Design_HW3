#include "systemc.h"
#include "iostream"

using namespace std;
SC_MODULE(Pattern)
{	
	sc_in_clk clock;
	sc_in<bool> reset;
	sc_out<sc_lv<32> > East_in, West_in, North_in, South_in, Local_in;
	sc_uint<32> temp1,temp2,temp3,temp4,temp5;
	sc_lv<32> East_temp,West_temp, North_temp,South_temp,Local_temp;
	ifstream pat_din;
	
	void pattern();
	
	SC_CTOR(Pattern)
	{
		pat_din.open("./input1.txt",ios::in);
		SC_METHOD(pattern);
		sensitive << clock.neg();
	}
};

