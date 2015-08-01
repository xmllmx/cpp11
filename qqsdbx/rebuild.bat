@echo off

start /MIN /W cleanup.bat
build -w
start /MIN /W copypdb.bat
start /MIN /W cleanup.bat