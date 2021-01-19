
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
#define app_printf wprintf
#else
#define app_printf printf
#endif

void ExampleApp_OutMessage(const STRING* Msg, BOOL NewLine)
{
	app_printf(Msg);
	if (NewLine == TRUE) app_printf(STR("\n\r"));
	fflush(stdout);
}

#define STR_VAR_BUFFER 128
#define STR_OUTPUT_BUFFER 512

int main()
{
	U32 var1, var2;
	STRING equation[] = STR("83628012 + 4289102");
	STRING* pEqu = equation;
	STRING myString[STR_OUTPUT_BUFFER];
	STRING varBuffer[STR_VAR_BUFFER];

	// Do some string setup

	String::Copy(myString, STR_OUTPUT_BUFFER, STR("Did you know "));
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

	while (true) {};
}
