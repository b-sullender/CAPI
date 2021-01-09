
/******************************************************************************************
*
* Name: testapp.h
*
* Created by  Brian Sullender
*
* Description:
*  Main header file used by testapp.c
*
*******************************************************************************************/

#ifndef EXAMPLE_H
#define EXAMPLE_H

#ifdef __GNUC__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <pthread.h>
#endif

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include "CAPI.h"

#define MAX_THREAD_WINDOWS 32
#define PATH_BUFFER_SIZE 1024
#define IMAGE_DEFAULT_ALIGNMENT 8
#define IMAGE_DESCRIPTION_SIZE 1024

#ifdef UNICODE
#define str_fopen _wfopen
#define str_len wcslen
#define str_cmp wcscmp
#define str_copy_s wcscpy_s
#define str_append_s wcscat_s
#define app_fgets fgetws
#define app_scanf wscanf
#define app_sscanf_s swscanf_s
#define app_printf wprintf
#define app_sprintf_s swprintf_s
#define ConsoleEntryPoint() wmain(int argc, wchar_t* argv[], wchar_t* envp[])
#define ApplicationEntryPoint() int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
#else
#define str_fopen fopen
#define str_len strlen
#define str_cmp strcmp
#ifdef __GNUC__
#define str_copy_s(dst, dst_size, src) strcpy(dst, src)
#define str_append_s(dst, dst_size, src) strcat(dst, src)
#define app_sscanf_s(src, format, dst) sscanf(src, format, dst)
#define ConsoleEntryPoint() int main(int argc, char** argv)
#define ApplicationEntryPoint() int main(int argc, char** argv)
#endif
#ifdef _MSC_VER
#define str_copy_s strcpy_s
#define str_append_s strcat_s
#define app_sscanf_s sscanf_s
#define ConsoleEntryPoint() main(int argc, char* argv[], char* envp[])
#define ApplicationEntryPoint() int WINAPI WinMain (_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#endif
#define app_fgets fgets
#define app_scanf scanf
#define app_printf printf
#define app_sprintf_s sprintf_s
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifdef __GNUC__
#define ConsoleClear() system("clear")
#define SetConsoleTitle(NewTitle) printf(STR("%c]0;%s%c"), '\033', NewTitle, '\007');
#endif

#ifdef _MSC_VER
#define ConsoleClear() system("cls");
#endif

#define WinClassName STR("TestAppCAPI")
#define WinTitle STR("Test App CAPI")

#define TESTAPP_WINDOW_BMP_WIDTH 1507
#define TESTAPP_WINDOW_BMP_HEIGHT 647

#define TESTAPP_WINDOW_JPG_WIDTH 832
#define TESTAPP_WINDOW_JPG_HEIGHT 94

#define TESTAPP_WINDOW_PNG_WIDTH 250
#define TESTAPP_WINDOW_PNG_HEIGHT 622

#define TESTAPP_WINDOW_ICO_WIDTH 358
#define TESTAPP_WINDOW_ICO_HEIGHT 670

#define TESTAPP_WINDOW_DRAW_WIDTH 960
#define TESTAPP_WINDOW_DRAW_HEIGHT 540

#define APP_COMMAND_EXIT       0  // Exit the Test Application
#define APP_COMMAND_CLS        1  // Clear console
#define APP_COMMAND_BMP        2  // Display the BMP test images
#define APP_COMMAND_JPG        3  // Display the JPG test images
#define APP_COMMAND_PNG        4  // Display the PNG test images
#define APP_COMMAND_ICO        5  // Display the ICO test images
#define APP_COMMAND_STRETCH    6  // Stretch and display test images
#define APP_COMMAND_DRAW       7  // Test drawing functions
#define APP_COMMAND_TEST       9  // Run programmer defined test code

#endif /* EXAMPLE_H */
