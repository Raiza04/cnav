# build.ps1
$ErrorActionPreference = "Stop"

cmake -B build
cmake --build build --clean-first --parallel