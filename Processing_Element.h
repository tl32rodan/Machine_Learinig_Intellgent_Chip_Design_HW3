#include "systemc.h"
#include <iostream>


using namespace std;

SC_MODULE(Processing_Element) {
	
	sc_in <bool> clk;
	sc_in <bool> reset;
	sc_fifo_in<sc_lv<22> > Data_in;  //Connect to data buffer
	sc_fifo_out<sc_lv<32> > Data_out; //Connect to partial sum buffer
	sc_out <bool> request;
		
    
	SC_HAS_PROCESS(Processing_Element);
	
	Processing_Element(sc_module_name processing_element, int ID, int Dest):
		sc_module(processing_element), id(ID), dest(Dest)
	{	
		SC_METHOD(PE);
		sensitive << reset.pos() <<clk.pos();
	}
	private:
		void PE();
		int id;
		int dest;
};
