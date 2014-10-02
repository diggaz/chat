del *.log /q /f
del *.user /q /f
del *.suo /q /f /a s
del *.sdf /q /f
del *.vcxproj /q /f
del *.filters /q /f
del *.pdb /q /f
del *.idb /q /f
qmake -makefile -spec win32-msvc2012 -tp vc
pause