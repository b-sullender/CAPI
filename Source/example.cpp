
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

#ifndef BOOL
#define BOOL int
#endif

#ifdef UNICODE
#define app_fgets fgetws
#define app_printf wprintf
#else
#define app_fgets fgets
#define app_printf printf
#endif

void ExampleApp_OutMessage(const UNIT* Msg, BOOL NewLine)
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

void Run_SMART_example()
{
	SMART string, comp1, comp2;
	float Fvar;

	Fvar = (float)1.3;
	string = STR("SMART type example: 1.3 + 6.4 = ");

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

// *********************** //
// *                     * //
// *  Application Entry  * //
// *                     * //
// *********************** //

int main()
{
	UNIT input[128];

	Run_String_Class_example();
	Run_SMART_example();

	ExampleApp_OutMessage(STR(""), TRUE);
	ExampleApp_OutMessage(STR("Press the return key to exit..."), TRUE);
	app_fgets(input, 127, stdin);

	return 0;
}
