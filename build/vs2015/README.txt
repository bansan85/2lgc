T�l�charger msys2 pour application 64�bits.
http://downloads.sourceforge.net/project/msys2/Base/x86_64/msys2-x86_64-20150512.exe
Laisser le dossier d'installation par d�faut C:\mingw64

Ex�cuter msys
msys2_shell.bat

Et la commande suivante :
pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime

Fermer Bash en fermant la croix de la fen�tre (CTRL+D ou exit peuvent �chouer) et relancer msys2_shell.bat

Et les commandes suivantes :
pacman -S mingw-w64-i686-suitesparse mingw-w64-i686-gtkmm3 mingw-w64-x86_64-suitesparse mingw-w64-x86_64-gtkmm3 automake1.15 autoconf libtool gcc make libiconv-devel mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain 

Enfin, le projet est maintenant utilisable en lan�ant le fichier 2lgc.sln.

Il est n�cessaire de r�aliser les op�rations de compilation suivantes dans l'ordre :
libsouvenir (g�n�re .dll et .lib)
libprojet (g�n�re .dll et .lib)
