#include <iostream>
#include <bitset>	////bitset<>
#include <string>
#include <stack>

std::stack<std::string> __err;
uint8_t __err_bit = 0;


inline unsigned int AsInt( float f ){
    return *( unsigned int *) &f;
}
inline float AsFloat( unsigned int i ){
    return *( float * ) &i;
}

inline std::string AsString(uint32_t number){
	return std::to_string(number);
}
inline uint32_t AsInt(std::string number){
	return std::stoi(number);
}

///Use two integer t create a float, doubleIntToFloat(45,45) = 45.45f
uint32_t doubleIntToFloat(const int significand, const uint32_t decimal){
	if((int) decimal < 0){
		__err.push("[DITF] \t Error, No negative decimal numbers allowed.");
		__err_bit += 1;
		return 0;
	}

	///Due to the way computers work, the significant bits are already known,
	//however the decimal bits are not.

	///Count the number of digits the decimal has to know where the dot stands, 
	//if the following multiplications go above it, react accordingly, 
	//ex:  45.45 -> 0.45 ...
	//
	//0.45 * 2 = 0.9 |(0)
	//0.9  * 2 = 1.8 |(1) 
	//0.8  * 2 ...
	//
	
	const uint8_t numSignificantDigits = AsString(decimal).length(); ///ex: 45.45 = 2, 45.456 = 3 ...
	uint32_t _mantissaBuffer = 0x00000000;	///One of the buffers used to hold the mantissa bits
	uint32_t _decimalBuffer = decimal;	////decimal is constant, buffer used to work its bits

	for(uint8_t I = 0 ; I < 32 ; I++){	///The above equation table is calculated here
		uint32_t _timesTwo = _decimalBuffer * 2;	///(0.45 * 2 = _timesTwo)
		uint8_t _setBit = 0;				///_timesTwo = X.YY (if X = 1 then _setBit = 1)
		if(AsString(_timesTwo).length() > numSignificantDigits){
			_setBit = 1;
			{
				std::string __stringBuffer = AsString(_timesTwo);	///Easier to deal with strings
				__stringBuffer[0] = ' ';		////Remove first digit
				_decimalBuffer = AsInt(__stringBuffer);	////Save the digits after point
									///*See comment above*
			}
		}else{
			_decimalBuffer = _timesTwo;
		}
		_mantissaBuffer = (_mantissaBuffer << 1) | _setBit;	///Shift left and add the bit
	}

	uint32_t _totalMantissa = 0x00000000;	///Total bits the mantissa could have,
						///it will hold 30 bits, but real mantissa holds only 23
	uint32_t _exponent = 0;

	{
		uint32_t __significandBuffer;	///Required buffer for bit shifting
		if((int)significand < 0){	///In case significand is negative, must complement
						///otherwise the vector will fill with 111...
			__significandBuffer = (significand*-1);
		}else{
			__significandBuffer = significand;
		}

		uint32_t __mantissaBuffer = _mantissaBuffer;	///Required buffer for bit shifting
	
		uint8_t numBitsSignificandOccupies = 32; ///The number of bits the signficand occupies
							 ///After squished to the end
		///Shove significand to the end
		//00000000000000000000000000010100
		//to
		//10100000000000000000000000000000
		while(!(__significandBuffer & 0b10000000000000000000000000000000)){	///Check if last bit is set
			__significandBuffer = (__significandBuffer << 1);
			numBitsSignificandOccupies--;
		}
		///Now it must remove the last set bit by shifting it
		//101101 -> X01101 -> 011010

		__significandBuffer = __significandBuffer << 1;	////Shifted to remove inferred bit
		numBitsSignificandOccupies--;	

		///Shift mantissa to the right to hold the significand, after that the buffers can
		//be merged to create the mantissa
		__mantissaBuffer = (__mantissaBuffer >> numBitsSignificandOccupies);

		_totalMantissa = (__significandBuffer | __mantissaBuffer);	///Merge
		_exponent = numBitsSignificandOccupies + 127;			///Bias the exponent
	}

	uint32_t ret = 0x00000000;	///Put everything together

	ret = (_totalMantissa >> 9); ///Put the mantissa in and leave space for the exponent and sign
	///1 bit for sign, 8 for exponent

	ret = ((_exponent << 23) | ret);	///Exponent buffer shifted to the end and merged
						///with the mantissa
	
	if((int)significand < 0) ret |= 0b10000000000000000000000000000000; ///Set the sign if necessary	

	return ret;
}


void printFullFloatData(uint32_t data, std::string indent = ""){
	std::cout << indent << "uint32_t as HEX  :\t" << std::hex << "0x" << data << std::dec << std::endl;
	std::cout << indent << "uint32_t as DEC  :\t" << data << std::dec << std::endl;
	std::cout << indent << "uint32_t as Float:\t" << AsFloat(data) << std::endl;
	std::cout << indent << "uint32_t as Bits :\t";
	uint32_t data_buff = data;

	for(uint32_t I = 0 ; I < 32 ; I++){
		if(data_buff & 0x80000000) std::cout << "1";
		else std::cout << "0";
		if(I == 8) std::cout << " ";
		if(I == 0) std::cout << " ";
		data_buff = data_buff << 1;
	}
	std::cout << std::endl;
	std::cout << indent << "                  \t";

	data_buff = data;
	if(data_buff & 0x80000000) std::cout << "N ";
	else std::cout << "P ";

	uint32_t exp_t = ( data_buff & 0x7F800000 ) >> 23;
	std::cout << std::hex << "0x" << exp_t << "\t\t";
	std::cout << std::hex << "0x" << (( data_buff & 0x7FFFFF )) << std::dec << std::endl;
}

void printFullFloatData(float data, std::string indent = ""){
	std::cout << indent << "float as HEX  :\t" << std::hex << "0x" << AsInt(data) << std::dec << std::endl;
	std::cout << indent << "float as DEC  :\t" << AsInt(data) << std::dec << std::endl;
	std::cout << indent << "float as Float:\t" << data << std::endl;
	std::cout << indent << "float as Bits :\t";
	uint32_t data_buff = AsInt(data);

	for(uint32_t I = 0 ; I < 32 ; I++){
		if(data_buff & 0x80000000) std::cout << "1";
		else std::cout << "0";
		if(I == 8) std::cout << " ";
		if(I == 0) std::cout << " ";
		data_buff = data_buff << 1;
	}
	std::cout << std::endl;
	std::cout << indent << "               \t";

	data_buff = AsInt(data);
	if(data_buff & 0x80000000) std::cout << "N ";
	else std::cout << "P ";

	uint32_t exp_t = ( data_buff & 0x7F800000 ) >> 23;
	std::cout << std::hex << "0x" << exp_t << "\t\t";
	std::cout << std::hex << "0x" << (( data_buff & 0x7FFFFF )) << std::dec << std::endl;
}



///Add two floats together
unsigned int AsInt_FloatAdd( unsigned int float1 , unsigned int float2 ){

	///	First check simple exceptions like "0.0 and -0.0" , "inf -inf"
	/// Not the most gracious error handling as both floats can be defective but thats the programmers
	///problem.
	//////////////////////////////////////////////////////////////////////////////
	{
		if( float1 == 0x00000000 || float1 == 0x80000000 ) return float2;	///"0.0 and -0.0"
		if( float2 == 0x00000000 || float2 == 0x80000000 ) return float1;	///"0.0 and -0.0"

		if( float1 == 0x7f800000 || float1 == 0xff800000 ) return float2;	///"inf -inf"
		if( float2 == 0x7f800000 || float2 == 0xff800000 ) return float1;	///"inf -inf"
	}
	//////////////////////////////////////////////////////////////////////////////


	///Now separate all components from the floats into their vectors
	///Keep in ind that some of the bit vectors necessary are:
	///		sign		- 1 bit
	///		exp			- 8 bits
	///     mantissa	- 25 bits  (extras for addition)

	///But as the processor must not call for these aparticular sizes, some bit shifting is required 
	/*--------------------------------------------------------------------------*/
	unsigned char exp1 = 0;     ///011111111
	unsigned char exp2 = 0;

	unsigned int mantissa1 = 0; ///00000000011111111111111111111111
	unsigned int mantissa2 = 0;

	bool sign1 = 0; ///10000....
	bool sign2 = 0;

	{
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
	}
	/*--------------------------------------------------------------------------*/


	///	Place 1 at bit 24 of mantissas, float(IEE754) has omitted a bit for the sake of storage space,
	///it must be added back for the addition in the future
	//////////////////////////////////////////////////////////////////////////////
	{
		mantissa1 = mantissa1 | 0x800000;
		mantissa2 = mantissa2 | 0x800000;
	}
	//////////////////////////////////////////////////////////////////////////////


	///	The mantissas must be normalised ( make exponents the same value ), thus the mantissas
	///must be shifted to account for the change.
	///	In the meantime, taking advantage that its already making comparisons, check for wich value
	///is bigger, if the biggest value is negative, the end float must change accordingly (2's comp).
	///	Keep in mid that normal comparisons ( X > Y ) also works on mantissas, as the biggest fraction
	///is the first one, and the sum of all lower fraction doesnt overpower the first
	/*--------------------------------------------------------------------------*/
	unsigned char final_sign;
	{
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
	}
	/*--------------------------------------------------------------------------*/


	///	To maintain the adder only, 2's complement whoever is negative
	//////////////////////////////////////////////////////////////////////////////
	{
		if( sign1 ) mantissa1 = (~mantissa1 + 1) ;
		if( sign2 ) mantissa2 = (~mantissa2 + 1) ;
	}
	//////////////////////////////////////////////////////////////////////////////

	///	Adding mantissas, keep in mind that if the float is negative, the mantissa will be in 2' comp,
	///so the same process acan be used to subtract.
	///	Due to the fact that if the bigger number is negative the end result will have to be 2' comp
	///again and the final float to be returned will be signed.
	///	Also, 25 bits is all you need, as no addition can be bigger than 25 bits. (24 bits + 24 bits)
	/*--------------------------------------------------------------------------*/
	unsigned int mantissa3 = 0;
	{
		mantissa3 = ( mantissa1 + mantissa2 ) & 0x1FFFFFF;
		if( final_sign ) mantissa3 = ( (~mantissa3 + 1)  & 0x1FFFFFF );
	}
	/*--------------------------------------------------------------------------*/


	///	Since the addition could end up in an un-normalized value ( first one in vector at bit 24 ),
	///it must be shifted and the exponent ( wich are the same for both ) must be changed accordingly.
	//////////////////////////////////////////////////////////////////////////////
	{
		if( mantissa3 != 0 ){

			if( mantissa3 & 0x1000000 ) {mantissa3 = ( mantissa3 >> 1 ); exp1++;}
		
			while( true ){
				if( !(mantissa3 & 0x800000) ){	///if bit 24 isnt 1, 'underflow', keep shifting until it fits
					mantissa3 = ( mantissa3 << 1 );
					exp1--;
				}else{
					break;
				}
			}
		
		}
	}
	//////////////////////////////////////////////////////////////////////////////


	/// Now that the mantissa is normalised, and possibly 2' comp, and the exp is corrected, just
	///assemble and return.
	///	The working mantissa (mantissa3) can be trimmed of its bit 24 as it is left omitted by IEE(754)
	///standard.
	///	And if the biggest number is negative, flip sign to return negative.
	/*--------------------------------------------------------------------------*/
	{
		mantissa3 = ( mantissa3 & 0x7FFFFF );
		
		unsigned int exp_assem = 0;
		
		exp_assem = ( exp_assem | exp1 );	///00000000000000000000000011111111
		exp_assem = ( exp_assem << 23 );	///01111111100000000000000000000000
		
		mantissa3 = ( mantissa3 | exp_assem );
		
		if( final_sign ) mantissa3 = ( mantissa3 | 0x80000000 );

		return mantissa3;
	}
	/*--------------------------------------------------------------------------*/

}
