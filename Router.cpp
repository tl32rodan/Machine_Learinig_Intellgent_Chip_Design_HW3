#include "Router.h"

int Router::decoder(sc_lv<32> flit){
	// Decode the flit using id
	// Return: 0~4 corresponding to East,West,North,South,Local respectively.
	// XY routing, so east & west first
	
	//cout<<"Flit: "<<flit<<endl;

/*************************** To be modify *****************************************/
	/*
	 *	1 | 2 | 3
	 * -----------
	 *  4 | 5 | 6
	 * -----------
	 *  7 | 8 | 9
	 */

	int dest = flit.range(25,22).to_uint();
	if( (dest-1)%3 > (id-1)%3){
		//East
		return 0;
	}
	else if( (dest-1)%3 < (id-1)%3){
		//West
		return 1;
	}
	else if( dest < id){
		//North
		return 2;
	}
	else if( dest > id){
		//South
		return 3;
	}
	else if( dest == id){
		//Local
		return 4;
	}
	else{
		return 5;
	}
}

void Router::write_output_port(int dest, sc_lv<32> flit){
	//Return true if write sucess; false otherwise.
	switch(dest){
		case 0:
			East_out.write(flit);
			Req_r[0].write(true);
			break;
		case 1:
			West_out.write(flit);
			Req_r[1].write(true);
			break;
		case 2:
			North_out.write(flit);
			Req_r[2].write(true);
			break;
		case 3:
			South_out.write(flit);
			Req_r[3].write(true);
			break;
		case 4: 
			/*if(Ack_p.read()){
				//Recieve Ack from PE, so local buffer can pop 1 data
				// Warning: No recovering the data read from source_fifo in previous iteration is implemented.
			}
			else{// Send request signal & data to NI 
				Req_r.write(true);
				Local_out.write(flit);
			}*/
			//cout<<"Here"<<endl;
			// cout<<"Flit to local = "<<flit<<endl;
			Local_out.write(flit);
			Req_r[4].write(true);
			
			break;
	}
}


/*********************************************************************************/

void Router::router(){
	// Zeroning the wires 	
	for(int i=0;i<5;i++){
		Req_r[i].write(false);
		Ack_r[i].write(false);	
	}

	/**********************************/
	// If the Ack is received, pop 1 element from each buffer -> Unable to do this if sc_fifo is used
	/*
	sc_lv<32> tmp;
	if(Ack_p[0])East_in_buffer.nb_read(tmp);
	if(Ack_p[1])West_in_buffer.nb_read(tmp);
	if(Ack_p[2])North_in_buffer.nb_read(tmp);
	if(Ack_p[3])South_in_buffer.nb_read(tmp);
	if(Ack_p[4])Local_in_buffer.nb_read(tmp);
	*/
	/**********************************/
	
	// Read next flit into buffers
	// Check if there is a data write request && buffer is not full
	if(Req_p[0].read()){
		sc_lv<32> tmp;
		if(East_in_buffer.num_free()==0){
			Ack_r[0].write(false);
		}
		else{
			tmp = East_in.read();
			if(!East_in_buffer.nb_write(tmp)){
				// Should not execute because previous if statement should have checked
				cout<<"East_in_buffer full!"<<endl;
			}
			Ack_r[0].write(true);
		}
	}
	if(Req_p[1].read()){
		sc_lv<32> tmp;
		if(West_in_buffer.num_free()==0){
			Ack_r[1].write(false);
		}
		else{
			tmp = West_in.read();
			if(!West_in_buffer.nb_write(tmp)){
				// Should not execute because previous if statement should have checked
				cout<<"West_in_buffer full!"<<endl;
			}
			Ack_r[1].write(true);
		}
	}
	if(Req_p[2].read()){
		sc_lv<32> tmp;
		if(North_in_buffer.num_free()==0){
			Ack_r[2].write(false);
		}
		else{
			tmp = North_in.read();
			if(!North_in_buffer.nb_write(tmp)){
				// Should not execute because previous if statement should have checked
				cout<<"North_in_buffer full!"<<endl;
			}
			Ack_r[2].write(true);
		}
	}
	if(Req_p[3].read()){
		sc_lv<32> tmp;
		if(South_in_buffer.num_free()==0){
			Ack_r[3].write(false);
		}
		else{
			tmp = South_in.read();
			if(!South_in_buffer.nb_write(tmp)){
				// Should not execute because previous if statement should have checked
				cout<<"South_in_buffer full!"<<endl;
			}
			Ack_r[3].write(true);
		}
	}
	if(Req_p[4].read()){
		sc_lv<32> tmp;
		if(Local_in_buffer.num_free()==0){
			Ack_r[4].write(false);
		}
		else{
			tmp = Local_in.read();
			if(!Local_in_buffer.nb_write(tmp)){
				// Should not execute because previous if statement should have checked
				cout<<"Local_in_buffer full!"<<endl;
			}
			Ack_r[4].write(true);
		}
	}

/*
	cout<<"East_in_buffer.num_available()="<<East_in_buffer.num_available()<<endl;
	cout<<"West_in_buffer.num_available()="<<West_in_buffer.num_available()<<endl;
	cout<<"North_in_buffer.num_available()="<<North_in_buffer.num_available()<<endl;
	cout<<"South_in_buffer.num_available()="<<South_in_buffer.num_available()<<endl;
	cout<<"Local_in_buffer.num_available()="<<Local_in_buffer.num_available()<<endl;
*/	

	for(int i=0; i<5; i++)
		output_channel_occupied[i] = false;

	// Read a flit from each buffer & determined the output port & check if conflict
	// If the output port is occupied, no poping of the flit
	// Buffer priority when conflicts occur at output ports: East>West>North>South>Local

	//East
	East_channel();

	//West
	West_channel();

	//North
	North_channel();

	//South
	South_channel();

	//Local
	Local_channel();

	
}

void Router::East_channel(){
	sc_lv<32> tmp ;
	if(East_in_buffer.nb_read(tmp)){
		int tmp_east_out = decoder(tmp);
		
		//Wrong destination
		if(tmp_east_out==5){
			return;
		}
		//cout<<"HERE"<<endl;
		//cout<<"tmp="<<tmp<<", tmp_east_out="<<tmp_east_out<<endl;
		
		// If the output channel is not occupied, write out to that.
		if(!output_channel_occupied[tmp_east_out]){	
			output_channel_occupied[tmp_east_out] = true;
		
			write_output_port(tmp_east_out, tmp);
		}

	}
}
void Router::West_channel(){

	sc_lv<32> tmp ;
	if(West_in_buffer.nb_read(tmp)){
		int tmp_west_out = decoder(tmp);
		
		//Wrong destination
		if(tmp_west_out==5){
			return;
		}
		// If the output channel is not occupied, write out to that.
		if(!output_channel_occupied[tmp_west_out]){
			output_channel_occupied[tmp_west_out] = true;
		
			write_output_port(tmp_west_out, tmp);
		}
	}
}

void Router::North_channel(){
	sc_lv<32> tmp ;
	if(North_in_buffer.nb_read(tmp)){
		int tmp_north_out = decoder(tmp);
		
		//Wrong destination
		if(tmp_north_out==5){
			return;
		}
		// If the output channel is not occupied, write out to that.
		if(!output_channel_occupied[tmp_north_out]){
			output_channel_occupied[tmp_north_out] = true;
		
			write_output_port(tmp_north_out, tmp);
		}	
	}
}
void Router::South_channel(){
	sc_lv<32> tmp ;
	if(South_in_buffer.nb_read(tmp)){
		int tmp_south_out = decoder(tmp);
		
		//Wrong destination
		if(tmp_south_out==5){
			return;
		}
		// If the output channel is not occupied, write out to that.
		if(!output_channel_occupied[tmp_south_out]){
			output_channel_occupied[tmp_south_out] = true;
		
			write_output_port(tmp_south_out, tmp);
		}
	}
}
void Router::Local_channel(){

	sc_lv<32> tmp ;
	if(Local_in_buffer.nb_read(tmp)){
		int tmp_local_out = decoder(tmp);
		
		//Wrong destination
		if(tmp_local_out==5){
			return;
		}
		// If the output channel is not occupied, write out to that.
		if(!output_channel_occupied[tmp_local_out]){
			output_channel_occupied[tmp_local_out] = true;

			write_output_port(tmp_local_out, tmp);
		}
	}
}

/* for debug:
	cout<<"East_buffer_head = "<<East_buffer_head<<";East_buffer_tail="<<East_buffer_tail<<endl;
	cout<<"West_buffer_head = "<<West_buffer_head<<";West_buffer_tail="<<West_buffer_tail<<endl;
	cout<<"North_buffer_head = "<<North_buffer_head<<";North_buffer_tail="<<North_buffer_tail<<endl;
	cout<<"South_buffer_head = "<<South_buffer_head<<";South_buffer_tail="<<South_buffer_tail<<endl;
	cout<<"Local_buffer_head = "<<Local_buffer_head<<";Local_buffer_tail="<<Local_buffer_tail<<endl;


	cout<<East_in_buffer[East_buffer_head]<<endl;
	cout<<West_in_buffer[West_buffer_head]<<endl;
	cout<<North_in_buffer[North_buffer_head]<<endl;
	cout<<South_in_buffer[South_buffer_head]<<endl;
	cout<<Local_in_buffer[Local_buffer_head]<<endl;


	cout<<"East_in_buffer[(East_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] = "<<East_in_buffer[(East_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH]<<endl;
	cout<<"West_in_buffer[(West_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] = "<<West_in_buffer[(West_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] <<endl;
	cout<<"North_in_buffer[(North_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] = "<<North_in_buffer[(North_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH]<<endl;
	cout<<"South_in_buffer[(South_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] = "<<South_in_buffer[(South_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH]<<endl;
	cout<<"Local_in_buffer[(Local_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] = "<<Local_in_buffer[(Local_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH]<<endl;

	cout<<"East_buffer_head = "<<East_buffer_head<<endl;
	cout<<"East_in_buffer[East_buffer_head] = "<<East_in_buffer[East_buffer_head]<<endl;
	cout<<"East_in_buffer[(East_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH] = "<<East_in_buffer[(East_buffer_tail+FIFO_LENGTH-1)%FIFO_LENGTH]<<endl;
*/

