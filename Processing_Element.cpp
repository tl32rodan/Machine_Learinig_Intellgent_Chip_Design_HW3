#include "Processing_Element.h"

void Processing_Element::PE(){
	request.write(false);

	if(reset.read()){
		/* Reset buffers */
		return;
	}

	//cout<<"Data_in.num_available()="<<Data_in.num_available()<<endl;
	// If the data buffer has 4 data, read them & process them & send the result to partial buffer
	if(Data_in.num_available() == 4){
		//cout<<"Here"<<endl;
		/* Read them in & process them */
		sc_uint<22> tmp1 = 0, tmp2 = 0;

		/*		
		cout<<"Data[0]="<<Data_in.read()<<endl;
		cout<<"Data[1]="<<Data_in.read()<<endl;
		cout<<"Data[2]="<<Data_in.read()<<endl;
		cout<<"Data[3]="<<Data_in.read()<<endl;
		*/
		
		tmp1 += Data_in.read().to_uint();
		tmp1 *= Data_in.read().to_uint();

		//cout<<"tmp1="<<tmp1<<endl;
		tmp2 += Data_in.read().to_uint();
		tmp2 *= Data_in.read().to_uint();
		//cout<<"tmp2="<<tmp2<<endl;

		/* Put the result into partial sum buffer */
		sc_lv<22> process_result(tmp1+tmp2);
		//cout<<"process_result="<<process_result<<endl;
		sc_lv<32> final_result(0);

		final_result.range(29,26) = sc_lv<4>(id);
		final_result.range(25,22) = sc_lv<4>(dest);
		final_result.range(21,0) = process_result;
		//cout<<"final_result="<<final_result<<endl;


		if(!Data_out.nb_write(final_result)){
			cout<<"Partial sum buffer full when PE writing!"<<endl;
		}

		/* Send Request to NI */
		request.write(true);
	}
	
}