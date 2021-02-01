
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

# Setting up a Linux workstation

- Red Hat-based (Fedora)
  - **sudo yum install gcc** Install the GNU Compiler Collection
  - **sudo yum install g++** Install the GNU optimising compiler for C++
  - **sudo yum install mingw32-gcc-c++** Install MinGW dev package (for the debugger)
  - **sudo yum install libX11-devel** Install X11 dev libraries (TestApp uses X11)
  - **sudo yum install codeblocks** Install Code::Blocks (IDE)
- Debian-based (Ubuntu)
  - **sudo apt install gcc** Install the GNU Compiler Collection
  - **sudo apt install g++** Install the GNU optimising compiler for C++
  - **sudo apt install gcc-mingw-w64-base** Install MinGW dev package (for the debugger)
  - **sudo apt install libx11-dev** Install X11 dev libraries (TestApp uses X11)
  - **sudo apt install codeblocks** Install Code::Blocks (IDE)
  - **sudo apt install make**

# Setting up a macOS workstation

First install Xcode using the App Store, then use the following command list in the terminal

- **xcode-select --install** Install command line developer tools
- **/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"** Install Homebrew
- **brew install --cask xquartz** Install X11 dev libraries (TestApp uses X11) *The terminal may appear frozen while installing, just wait!*
