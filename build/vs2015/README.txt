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

Editer le fichier C:\msys32\mingw32\include\libintl.h et changer la ligne 312 de
#if !0
en
#if 0

Enfin, le projet est maintenant utilisable en lan�ant le fichier 2lgc.sln.