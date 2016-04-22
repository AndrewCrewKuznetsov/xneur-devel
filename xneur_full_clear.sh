#!/bin/bash

UID_ROOT=0
if [ "$UID" -ne "$UID_ROOT" ]
then
zenity --error --text="Use sudo ./xneur_full_clear.sh" --title="Access denied"
exit
fi

echo "Find and delete xneur, gxneur and kxneur directories"
find /usr -name "*xneur*" -type d -print | xargs /bin/chmod 0777 -R
find /usr -name "*xneur*" -type d -print | xargs /bin/rm -Rf

echo "Find and delete xneur, gxneur and kxneur files"
find /usr -name "*xneur*" -type f -print | xargs /bin/chmod a+rw 
find /usr -name "*xneur*" -type f -print | xargs /bin/rm -f

echo "Find and delete xneur libraries links"
find /usr -name "*libxnconfig*" -type l -print | xargs /bin/chmod a+rw
find /usr -name "*libxnconfig*" -type l -print | xargs /bin/rm -f

echo "Find and delete xneur libraries"
find /usr -name "*libxnconfig*" -type f -print | xargs /bin/chmod a+rw
find /usr -name "*libxnconfig*" -type f -print | xargs /bin/rm -f

echo "Find and delete xneur libraries links"
find /usr -name "*libxneur*" -type l -print | xargs /bin/chmod a+rw
find /usr -name "*libxneur*" -type l -print | xargs /bin/rm -f

echo "Find and delete xneur libraries"
find /usr -name "*libxneur*" -type f -print | xargs /bin/chmod a+rw
find /usr -name "*libxneur*" -type f -print | xargs /bin/rm -f

echo "Find and delete xneur libraries links"
find /usr -name "*libxntest*" -type l -print | xargs /bin/chmod a+rw
find /usr -name "*libxntest*" -type l -print | xargs /bin/rm -f

echo "Find and delete xneur libraries"
find /usr -name "*libxntest*" -type f -print | xargs /bin/chmod a+rw
find /usr -name "*libxntest*" -type f -print | xargs /bin/rm -f

echo " "
echo "All Done!"
