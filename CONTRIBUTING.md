# Coding Style

**Formatting**

TODO...

**Data Types**

Do not use normal C/C++ data types unless a library or function specifically asks for that type. instead use one of the following approved types:  

- Signed
  - I8
  - I16
  - I32
  - I64

- Unsigned
  - U8
  - U16
  - U32
  - U64

- Strings
  - ASCII
  - UTF8
  - UTF16
  - UTF32

**Third-party libraries**

You can request a specific library be added to the project at [Discussions General](https://github.com/b-sullender/CAPI/discussions/categories/general) or via capi.project.brian@gmail.com to the project leader.  
Please provide the reason for the request. To keep CAPI lightweight, your request may be denied if the library has dependencies or the codebase is too large.  

We do not use std for anything.