#	makefile for wingstall setups
#	run with nmake from x64 Native Tools Command Prompt for VS 2019

all: setups

plain_package:
	wingstall -v --create-package plain/wingstall.package.xml
	
plain_setup:
	wingstall -v --make-setup plain/wingstall.package.bin
	MSBuild -m -verbosity:minimal plain\program\setup.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal plain\program\setup.vcxproj -p:Configuration="Release"
	
plain: plain_package plain_setup
	wnginst -vd plain/program/x64/Debug/setupd.exe plain/bin
	wnginst -vd plain/program/x64/Release/setup.exe plain/bin

deflate_package:
	wingstall -v --create-package deflate/wingstall.package.xml
	
deflate_setup:
	wingstall -v --make-setup deflate/wingstall.package.bin
	MSBuild -m -verbosity:minimal deflate\program\setup.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal deflate\program\setup.vcxproj -p:Configuration="Release"
	
deflate: deflate_package deflate_setup
	wnginst -vd deflate/program/x64/Debug/setupd.exe deflate/bin
	wnginst -vd deflate/program/x64/Release/setup.exe deflate/bin

bzip2_package:
	wingstall -v --create-package bzip2/wingstall.package.xml
	
bzip2_setup:
	wingstall -v --make-setup bzip2/wingstall.package.bin
	MSBuild -m -verbosity:minimal bzip2\program\setup.vcxproj -p:Configuration="Debug"
	MSBuild -m -verbosity:minimal bzip2\program\setup.vcxproj -p:Configuration="Release"
	
bzip2: bzip2_package bzip2_setup
	wnginst -vd bzip2/program/x64/Debug/setupd.exe bzip2/bin
	wnginst -vd bzip2/program/x64/Release/setup.exe bzip2/bin

setups: plain deflate bzip2
