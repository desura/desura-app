REM BINPATH path to exe's and bin files
REM SIGNTOOL tool to use to sign exes and dlls
REM CERTPATH cert to use to sign

for %%i in (%BINPATH%\*.exe) do (%SIGNTOOL% sign -f %CERTPATH% -p lindenlab -t http://timestamp.comodoca.com/authenticode %%i)
for %%i in (%BINPATH%\bin\*core.dll) do (%SIGNTOOL% sign -f %CERTPATH% -p lindenlab -t http://timestamp.comodoca.com/authenticode %%i)
