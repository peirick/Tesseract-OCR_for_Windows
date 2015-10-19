git clone git://github.com/tesseract-ocr/tesseract.git

call "%programfiles% (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
cd leptonica
msbuild build.proj
cd ..
xcopy "leptonica\release\*.*" "." /E /Y

xcopy "vs2013" "tesseract\vs2013\" /E /Y
cd tesseract\vs2013
msbuild tesseract.sln /property:Configuration=LIB_Release /property:Platform=Win32
msbuild tesseract.sln /property:Configuration=LIB_Debug /property:Platform=Win32
cd ..\..


copy "lib\Win32\*-static*.lib" "..\lib\" /Y
copy "lib\libtesseract*-static*.lib" "..\lib\" /Y
xcopy "include\*.*" "..\include\" /E /Y
xcopy "tesseract\api\*.h" "..\include\tesseract\" /E /Y
xcopy "tesseract\ccmain\*.h" "..\include\tesseract\" /E /Y
xcopy "tesseract\ccstruct\*.h" "..\include\tesseract\" /E /Y
xcopy "tesseract\ccutil\*.h" "..\include\tesseract\" /E /Y
xcopy "tesseract\vs2013\port\*.h" "..\include\tesseract\" /E /Y