# Coding Style

**Language and Files**

We use the C Language for all internal development with support of C++ constructors and overloads in the primary CAPI.h header file.
C Implementation should be named `.c`. Headers should be named `.h`. Both C and Headers files should contain:
```

/******************************************************************************************
*
* Name: file_name.h
*
* Created by  Your Name Here
*
* Description:
*  This is the Description of what the file is for. Keep it simple!
*
*******************************************************************************************/

```

All headers should contain:
```
#ifndef FILE_NAME_H
#define FILE_NAME_H

// Header code here!

#endif /* FILE_NAME_H*/
```

**Formatting**

TODO...

**Data Types**

Do not use normal C/C++ data types unless a library or function specifically asks for that type. Instead use one of the following approved types:  

- Signed
  - I8
  - I16
  - I32
  - I64
  - ptrdiff_t

- Unsigned
  - U8
  - U16
  - U32
  - U64
  - size_t

- Strings
  - ASCII
  - UTF8
  - UTF16
  - UTF32

- Other Types
  - float
  - double

**System Resources**

To obtain system resources use one of the folllowing approved functions:

- capi_malloc
- capi_realloc
- capi_free
- capi_aligned_malloc
- capi_aligned_free

**Memory Management**

Use one of the following approved functions:

- capi_memset
- capi_memset32
- capi_memcopy

**Structures and Classes**

Use the `STRUCT` macro for defining structures unless its a C++ struct with constructors or overloaded functions. Structures defined with `STRUCT` should have all capitalized letters in the name.
For hardware and file structures that are packed, use the `PACK` macro. 
```
STRUCT(MY_STRUCT_NAME)
{
	U32 someVar;
};

PACK(STRUCT(MY_STRUCT_NAME)
{
	U32 someVar;
});

struct Mystruct
{
	static U32 OverloadedFunction(U32 Var)
	{
	}
};
```

**Comments**

Source code comments are not necessary but welcomed. Start comment lines with `//` instead of `/* ... */`.  
Use `/* ... */` to seperate code sections if desired. Preferred format is:
```
// *                      * //
// **                    ** //
// ***  ASCII versions  *** //
// **                    ** //
// *                      * //
```
All exported functions are required to be documented in the primary CAPI.h header with the function declaration, with the exception of 3rd party library exports.

**Third-party libraries**

You can request a specific library be added to the project at [Discussions General](https://github.com/b-sullender/CAPI/discussions/categories/general) or via capi.project.brian@gmail.com to the project leader.  
Please provide the reason for the request. To keep CAPI lightweight, your request may be denied if the library has dependencies or the codebase is too large.  

We do NOT use std for anything.
