#!/bin/bash
# Black Hat stuff that I wrote for some reason
USER=${USERNAME}
PASS=${USERPASS}

SHADOW=`grep "^"$USER":" /etc/shadow`
IFS=':'
read -r -a ITEM <<< "$SHADOW"
encryption=${ITEM[1]}
IFS='\$'
read -r -a ENCODING <<< "$encryption"
METHOD=${ENCODING[1]}
SALT=${ENCODING[2]}
CODING=${ENCODING[3]}
if [ $DEBUG ]
then
        echo $METHOD
        echo $SALT
        echo $CODING
fi
if [ ${METHOD} -eq 1 ] ## MD5 hash Algorithm
then
        ENCRYPTED=$(openssl passwd -1 -salt TgShKSRP ppclab)
elif [ ${METHOD} -eq 6 ] ## SHAR-512 Algorithm
then
        ENCRYPTED=$(python -c "import crypt, getpass, pwd; print crypt.crypt('$PASS', '\$6\$${SALT}\$')")
fi

if [ "${ENCRYPTED}" == "${encryption}" ]
then
        echo "passwords match"
        exit 0
fi
