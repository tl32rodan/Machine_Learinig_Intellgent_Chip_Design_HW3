#include "systemc.h"
#include <iostream>

#define FLIT_LENGTH 32
#define Port_NUM 5
#define FIFO_DEPTH 3
#define FIFO_LENGTH 8

SC_MODULE(Router) {
	
	sc_in <bool> clk;
	sc_in <bool> reset;
	sc_in <sc_lv<32> > East_in, West_in, North_in, South_in, Local_in;
	sc_in <bool> Ack_p [5];
	sc_in <bool> Req_p [5];
	sc_out <sc_lv<32> > East_out, West_out, North_out, South_out, Local_out;
	sc_out <bool> Ack_r [5];
	sc_out <bool> Req_r [5];
	
	
    
	SC_HAS_PROCESS(Router);
	
	Router(sc_module_name _router, int ID):
		sc_module(_router), id(ID)
	{
		for( int i=0;i<5;i++) output_channel_occupied[i]=false;
		sc_fifo<sc_lv<32> > East_in_buffer(FIFO_LENGTH), West_in_buffer(FIFO_LENGTH), North_in_buffer(FIFO_LENGTH), South_in_buffer(FIFO_LENGTH), Local_in_buffer(FIFO_LENGTH);
		SC_METHOD(router);
		sensitive << reset.pos() <<clk.pos();
	}
	private:
		void router();
		int decoder(sc_lv<32> flit);
		void write_output_port(int dest, sc_lv<32> flit);

		void East_channel();
		void West_channel();
		void North_channel();
		void South_channel();
		void Local_channel();

		int id;
		bool output_channel_occupied[5];
		sc_fifo<sc_lv<32> > East_in_buffer, West_in_buffer, North_in_buffer, South_in_buffer, Local_in_buffer;
};
