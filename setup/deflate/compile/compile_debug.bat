call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild -verbosity:minimal -t:Rebuild -p:Configuration="Debug" "C:\work\wingstall\setup\deflate\program\setup.vcxproj" > "C:\work\wingstall\setup\deflate\compile\debug_stdout.txt" 2> "C:\work\wingstall\setup\deflate\compile\debug_stderr.txt"
