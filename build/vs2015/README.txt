T�l�charger msys2 pour application 32�bits.
http://sourceforge.net/projects/msys2/files/Base/i686/msys2-i686-20150512.exe/download
Laisser le dossier d'installation par d�faut C:\mingw32

Ex�cuter msys
mingw32_shell.bat

Et la commande suivante :
pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime

Fermer Bash et relancer mingw32_shell.bat

Et les commandes suivantes :
pacman -S mingw-w64-i686-suitesparse mingw-w64-i686-gtkmm3 automake1.15 autoconf libtool gcc make libiconv-devel

Enfin, le projet est maintenant utilisable en lan�ant le fichier 2lgc.sln.

libeurocode et libprojet pr�sentant une d�pendance circulaire, il est
n�cessaire de r�aliser les op�rations de compilation suivante dans l'ordre :
1 libundo (g�n�re .dll et .lib)
2 libprojet (g�n�re .lib)
3 libeurocode (g�n�re .dll et .lib)
4 libprojet (g�n�re .dll et .lib)
5 codegui (g�n�re codegui)
