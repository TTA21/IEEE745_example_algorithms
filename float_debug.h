
#include <iostream>
#include <bitset>	////bitset<>
#include <algorithm>
#include <cmath>
#include <stack>
#include <string> ///Remember, not for production

//std::stack<std::string> __err;

//std::string _err;
//uint8_t _err_bit = 0;

unsigned int AsInt_FloatAdd_debug(  unsigned int float1 , unsigned int float2  ){

	if( float1 == 0 || float1 == 0x80000000 ) return float2;    ///Deal with zeros
	if( float2 == 0 || float2 == 0x80000000 ) return float1;

    unsigned char exp1 = 0;     ///011111111
	unsigned char exp2 = 0;

	unsigned int mantissa1 = 0; ///00000000011111111111111111111111
	unsigned int mantissa2 = 0;

	bool sign1 = 0; ///10000....
	bool sign2 = 0;

    mantissa1 = ( float1 & 0x7FFFFF );	///00000000011111111111111111111111
	mantissa2 = ( float2 & 0x7FFFFF );	///00000000011111111111111111111111

	{
		unsigned int exp_t = 0;
		exp_t = ( float1 & 0x7F800000 ) >> 23 ;	///01111111100000000000000000000000 -> 00000000000000000000000011111111
		exp1 = exp_t;				            ///00000000000000000000000011111111 -> 11111111
		exp_t = ( float2 & 0x7F800000 ) >> 23 ;	///01111111100000000000000000000000 -> 00000000000000000000000011111111
		exp2 = exp_t;				            ///00000000000000000000000011111111 -> 11111111
	}

	{
		unsigned int pos_t = 0;
		pos_t = ( float1 & 0x80000000 );	///10000000000000000000000000000000
		if(pos_t) sign1 = 1;
		pos_t = ( float2 & 0x80000000 );	///10000000000000000000000000000000
		if(pos_t) sign2 = 1;
	}

	cout<<"Float1:             |" << bitset<32>(float1) << endl;
	cout<<"Extracted exp1:     | " << bitset<8>(exp1) <<endl;
	cout<<"Extracted mantissa: |" << bitset<32>(mantissa1) <<endl;
	cout<<"Extracted pos1:     |" << bitset<8>(sign1) << endl;
	cout<<"\nFloat2:             |" << bitset<32>(float2) << endl;
	cout<<"Extracted exp2:     | " << bitset<8>(exp2) <<endl;
	cout<<"Extracted mantissa: |" << bitset<32>(mantissa2) <<endl;
	cout<<"Extracted pos2:     |" << bitset<8>(sign2) << endl;
	cout<<"00---------------------------------------------------00"<<endl<<endl;

	///place 1 at bit 24 of mantissas
	mantissa1 = mantissa1 | 0x800000;
	mantissa2 = mantissa2 | 0x800000;

	///normalise ( exp1 == exp2 )
	unsigned char final_sign;
	if( exp2 > exp1 ) {
		mantissa1 = ( mantissa1 >> abs( exp1 - exp2 ) ); 
		if( sign2 ) final_sign = 1; 
		exp1 = exp2;
	}
	if( exp1 > exp2 ) {
		mantissa2 = ( mantissa2 >> abs( exp1 - exp2 ) ); 
		if( sign1 ) final_sign = 1; 
		exp2 = exp1;
	};

	if( exp2 == exp1 ){
		if( (mantissa1 > mantissa2) && (sign1) ) final_sign = 1;
		if( (mantissa2 > mantissa1) && (sign2) ) final_sign = 1;
	}

	cout<<"Mantissa1 after: |" << bitset<32>(mantissa1) << endl;
	cout<<"Mantissa2 after: |" << bitset<32>(mantissa2) << endl;

	///To maintain the adder only, 2's complement whoever is negative
	if( sign1 ) mantissa1 = (~mantissa1 + 1) ;
	if( sign2 ) mantissa2 = (~mantissa2 + 1) ;

	unsigned int mantissa3 = ( mantissa1 + mantissa2 ) & 0x1FFFFFF;

	cout<<"Mantissa3 after addition:   |" << bitset<32>(mantissa3) << endl;

	if( final_sign ) mantissa3 = ( (~mantissa3 + 1)  & 0x1FFFFFF );	///Rmebember

	cout<<"Mantissa3 after complement: |" << bitset<32>(mantissa3) << endl;
	
	if( mantissa3 != 0 ){

		if( mantissa3 & 0x1000000 ) {mantissa3 = ( mantissa3 >> 1 ); exp1++;}	///check if bit 25 is set, if so
		///overflow so shift the first one back to 24
	
		while( true ){
			if( !(mantissa3 & 0x800000) ){	///if bit 24 isnt 1, 'underflow', keep shifting until it fits
				mantissa3 = ( mantissa3 << 1 );
				exp1--;
			}else{
				break;
			}
		}
		
	}
	


	mantissa3 = ( mantissa3 & 0x7FFFFF );
	
	unsigned int exp_assem = 0;
	
	exp_assem = ( exp_assem | exp1 );	///00000000000000000000000011111111
	exp_assem = ( exp_assem << 23 );	///01111111100000000000000000000000
	
	mantissa3 = ( mantissa3 | exp_assem );
	
	if( final_sign ) mantissa3 = ( mantissa3 | 0x80000000 );

    cout<<"\n\n\tThe end\n"<<endl;
	return mantissa3;

}
