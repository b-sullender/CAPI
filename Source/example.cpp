
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
	STRING myString[STR_OUTPUT_BUFFER];
	STRING varBuffer[STR_VAR_BUFFER];

	String::Copy(myString, STR_OUTPUT_BUFFER, STR("Did you know 83628012 + 4289102 = "));

	U32 var1 = 83628012 + 4289102;

	ToString(varBuffer, STR_VAR_BUFFER, var1);
	String::Append(myString, STR_OUTPUT_BUFFER, varBuffer);

	ExampleApp_OutMessage(myString, TRUE);

	while (true) {};
}
