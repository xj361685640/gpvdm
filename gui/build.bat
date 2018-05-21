rd /s /q __pycahce__
rd /s /q dist
pyinstaller --onefile gpvdm.py --icon=..\images\icon.ico
move .\dist\*.exe ..\
