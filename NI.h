#include "systemc.h"
#include <iostream>


using namespace std;

SC_MODULE(Network_Interface) {
	
	sc_in < bool > Req_r, Ack_r;
	sc_in < sc_lv<32> > flit_in;
	sc_fifo_in<sc_lv<32> > Data_out;//Connect to partial sum buffer
	sc_in <bool> request;

	sc_out < bool > Req_p, Ack_p;
	sc_out < sc_lv<32> > flit_out;
	sc_fifo_out<sc_lv<22> > Data_in; //Connect to data buffer in PE
	
		
	void NI();
    
    SC_CTOR(Network_Interface) {
		
		SC_METHOD(NI);
		sensitive << Req_r << Ack_r << flit_in << request;
    }
};
