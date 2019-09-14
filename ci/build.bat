C:\msys64\msys2_shell.cmd -mingw64 -lc "C:/projects/rotwkl/ci/build_win C:/projects/rotwkl C:/Qt/5.13.0/mingw73_64"

SET BUILD_RESULT=%ERRORLEVEL%

type C:\projects\rotwkl\output.log
del C:\projects\rotwkl\output.log

EXIT /b %BUILD_RESULT%
