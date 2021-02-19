
/******************************************************************************************
*
* Name: example.cpp
*
* Created by  Brian Sullender
*
* Description:
*  Demonstrate the CAPI string and arithmetic funtions.
*
*******************************************************************************************/

#include "CAPI.h"

#ifdef UNICODE
#define app_fgets fgetws
#define app_printf wprintf
#else
#define app_fgets fgets
#define app_printf printf
#endif

void ExampleApp_OutMessage(const UNIT* Msg, bool NewLine)
{
	app_printf(STR("%s"), Msg);
	if (NewLine == TRUE) app_printf(STR("\n\r"));
	fflush(stdout);
}

// ***************************************** //
// *                                       * //
// *  Example Using the SMART string type  * //
// *                                       * //
// ***************************************** //

// Be sure to use the STR macro for string constants for cross-platform compatibility

void Run_SMART_example(SMART equ1, SMART equ2)
{
	SMART string, comp1, comp2;
	float Fvar;

	Fvar = (float)1.3;
	string = STR("SMART type example: ");
	string += equ1 + STR(" + ") + equ2 + STR(" = ");

	// Variables are automatically converted into string
	string += Fvar + 6.4;

	// You can compare a SMART string like any other native variable type
	comp1 = STR("some text");
	comp2 = STR("some text");
	if (comp1 == comp2)
	{
		string += STR(" - ");
		string += STR("comp1 and comp2 are EQUAL!");
	}

	ExampleApp_OutMessage(string, TRUE);
}

// ************************************ //
// *                                  * //
// *  Example Using the String class  * //
// *                                  * //
// ************************************ //

// Again, be sure to use the STR macro for string constants for cross-platform compatibility!

#define STR_VAR_BUFFER 128
#define STR_OUTPUT_BUFFER 512

void Run_String_Class_example()
{
	U64 var1, var2;
	UNIT equation[] = STR("83628012 + 4289102");
	UNIT* pEqu = equation;
	UNIT myString[STR_OUTPUT_BUFFER];
	UNIT varBuffer[STR_VAR_BUFFER];

	// Do some string setup

	String::Copy(myString, STR_OUTPUT_BUFFER, STR("String class example: "));
	String::Append(myString, STR_OUTPUT_BUFFER, equation);
	String::Append(myString, STR_OUTPUT_BUFFER, STR(" = "));

	// Read numbers from equation string

	FromString(&var1, equation, 0, &pEqu);
	while (String::PullChar(&pEqu) != '+');
	FromString(&var2, pEqu);

	// Print the result

	ToString(varBuffer, STR_VAR_BUFFER, var1 + var2);
	String::Append(myString, STR_OUTPUT_BUFFER, varBuffer);

	ExampleApp_OutMessage(myString, TRUE);
}

// *********************************** //
// *                                 * //
// *  Example Using U128/I128 types  * //
// *                                 * //
// *********************************** //

// Be sure to use the NUM macro for U128 and I128 constants, this allows the full range of the type constants to be read

void Run_U128_type_example(U128 var1, U128 var2)
{
	SMART string;

	string = STR("U128 / I128 example: 7e+24 + 4e+24 + 3e+24 / 8e+23 = ");

	var1 = var1 + NUM(4000000000000000000000000) + var2 / NUM(800000000000000000000000);
	string += var1;

	ExampleApp_OutMessage(string, TRUE);
}

// *********************** //
// *                     * //
// *  Application Entry  * //
// *                     * //
// *********************** //

int main()
{
	UNIT input[128];

	Run_String_Class_example();

	SMART equ1, equ2;

	equ1 = STR("1.3");
	equ2 = STR("6.4");
	Run_SMART_example(equ1, equ2);

	U128 num1, num2;

	num1 = NUM(7000000000000000000000000);
	num2 = NUM(3000000000000000000000000);
	Run_U128_type_example(num1, num2);

	ExampleApp_OutMessage(STR(""), TRUE);
	ExampleApp_OutMessage(STR("Press the return key to exit..."), TRUE);
	app_fgets(input, 127, stdin);

	return 0;
}
