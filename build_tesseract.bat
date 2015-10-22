git clone --depth=1 --single-branch git://github.com/tesseract-ocr/tesseract.git  --branch="master" "tesseract_master"

call "%programfiles% (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
msbuild tesseract.sln /property:Configuration=Release /property:Platform=Win32
