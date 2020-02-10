#!/bin/bash

##
## validatePW - use existing password infrastructure to validate a password
## Usage - set environment variables PPSNAME and PPSPASS to name and password
## Execute - validatePW
##
## Rick Hernandez
## 02/02/2017
## version 0.9
##
## TODO
## add methods for other encryption standards as necessary
##

# For security - using environment variables to pass vars between processes
if [ -z ${PPSNAME} ]
then
        echo "PPSNAME required"
        exit 1
fi
if [ -z ${PPSPASS} ]
then
        echo "PPSPASS required"
        exit 2
fi

# root passwords are not advised
if [ ${PPSNAME} == "root" ]
then
        echo "Username may not be root"
        exit 3
fi

# verify user exists
SHADOW=`grep "^${PPSNAME}:" /etc/shadow`
if [ $? -ne 0 ]
then
        echo "Username not found"
        exit 4
fi

# parse shadow line
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

# determine method and use appropriate tools to re-create encoding
if [ ${METHOD} -eq 1 ] ## MD5 hash Algorithm
then
        type openssl >/dev/null 2>&1
        if [ $? -ne 0 ]
        then
                echo "openssl required"
                exit 5
        fi
        ENCRYPTED=$(openssl passwd -1 -salt ${SALT} ${PPSPASS})
elif [ ${METHOD} -eq 6 ] ## SHAR-512 Algorithm
then
        type python >/dev/null 2>&1
        if [ $? -ne 0 ]
        then
                echo "python required"
                exit 6
        fi
        ENCRYPTED=$(python -c "import crypt, getpass, pwd; print crypt.crypt('$PPSPASS', '\$6\$${SALT}\$')")
fi

# verify the encryption is the same
if [ "${ENCRYPTED}" == "${encryption}" ]
then
        echo "passwords match"
        exit 0
fi

# exit error no match
echo "passwords do not match"
exit 255
