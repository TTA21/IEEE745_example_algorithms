#include <iostream>
#include <bitset>	////bitset<>
#include "float.h"


using namespace std;
#include "float_debug.h"

int main(){

	//11111111111111111111111111111111
	//00000000000000000000000000000000

	uint32_t float1 = doubleIntToFloat(10,20);
	uint32_t float2 = doubleIntToFloat(-34,41);

	uint32_t float3 = AsInt_FloatAdd(float1, float2);

	printFullFloatData(float3);

    return 0;
}
