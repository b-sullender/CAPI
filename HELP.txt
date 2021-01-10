
# Using the TestApp console to test CAPI
```
	bmp - Display BMP test images
		NO PARAMETERS
	jpg - Display JPG test images
		NO PARAMETERS
	png - Display PNG test images
		NO PARAMETERS
	ico - Display ICO test images
		NO PARAMETERS
	stretch - Stretch and display test image
		image - This parameter can be one of the following:
			big - Uses the stretchTestBig.jpg image for the stretch test
			small - Uses the stretchTestSmall.png image for the stretch test
		algorithm - This parameter can be one of the following:
			linear - Stretches the image using the Linear Interpolation algorithm
			boxsamp - Stretches the image using the Box Sampling algorithm
			all - Stretches the image using both algorithms (displays linear above boxsamp)
		size - The new size of the image to stretch to
			The size parameter is the width followed by the height
			Use a 'x' or 'X' to seperate the witdth and height (E.g. 640x360 or 640X360)
	draw - Test drawing functions
		NO PARAMETERS
	test - Run programmer defined test code
		NO PARAMETERS - Can be added (NOTE: parameters are separated by a space)
	cls - Clear console
		NO PARAMETERS
	exit - Exit the test program
		NO PARAMETERS
```

# Examples of using the stretch command
```
	stretch big all 480x270
	stretch small all 768x432
```

# Install command list for setting up a Linux workstation
```
	Install the GNU Compiler Collection
		sudo yum install gcc
	Install the GNU optimising compiler for C++
		sudo yum install g++
	Install MinGW dev package (for the debugger)
		sudo yum install mingw32-gcc-c++
	Install X11 dev libraries (TestApp uses X11)
		sudo yum install libX11-devel
	Install Code::Blocks (IDE)
		sudo yum install codeblocks
```
