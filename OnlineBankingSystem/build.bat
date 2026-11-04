@echo off
setlocal

g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\main.cpp src\Account.cpp src\Authentication.cpp src\Admin.cpp src\Bank.cpp src\Customer.cpp -o OnlineBankingSystem.exe
if errorlevel 1 (
    echo Build failed. Check the compiler output above.
    exit /b 1
)

echo Build successful.
echo Run OnlineBankingSystem.exe from this project folder.
OnlineBankingSystem.exe
endlocal
