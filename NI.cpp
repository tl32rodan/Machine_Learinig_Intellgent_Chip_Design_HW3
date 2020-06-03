#include "NI.h"

void Network_Interface::NI(){
	/*
		0. Zeroning Req_p, Ack_p
		Interact with router:
		1. Receive Req_r from router
		2. Check if data buffer is not full; if so, read in filt & send Sck_p
		3. Send the filt to data buffer
		
		Interact with PE:
		1. Receive request signal from PE
		2. Send Req_p & a filt from partial sum buffer to router
		3. If Ack_r is received, partial sum buffer pops 1 filt.
	*/

	// For fear of router reads Req_p & Ack_p when NI doesn't writing it, zeroing them.
	Req_p.write(false);
	Ack_p.write(false);

	// Receive Req_r from router
	if(Req_r.read()){
		// Having received the write request from router
		// Check if the data buffer is full; if not, get data & put it into data buffer
		sc_lv<32> tmp_data_in = flit_in.read();
		//cout<<"tmp_data_in="<<tmp_data_in<<endl;
		if(Data_in.nb_write(tmp_data_in.range(21,0))){
			// Write sucess! Return Ack_p=1 to router
			Ack_p.write(true);
		}
		else{
			// Write data to data buffer failed due to buffer had been full. Return Ack_p=0 to router
			Ack_p.write(false);
		}
	}

	// Receive request from PE
	if(request.read()){
		// Having received the request from PE

		sc_lv<32> tmp_data_out;
		// Read 1 data from partial sum buffer
		if(!Data_out.nb_read(tmp_data_out)){
			// Unexpected action
			cout<<"Reading partial buffer error! the buffer is empty & it should be PE to secure there is something."<<endl;
			return;
		} 
		else{
			// Send Req_p & filt to local router
			/* 
				Warning: There are no guarantees that the request will be received, but neither checking for 
			             ack nor temporary storing tmp_data_out across calling SC_METHODs was implemented. 
			*/
			flit_out.write(tmp_data_out);
			Req_p.write(true);
		}
	}
	
}