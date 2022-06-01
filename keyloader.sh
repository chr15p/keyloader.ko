#!/bin/bash
ISNUMERIC='^[0-9]+$'

if [ -f "$1" ]; then
	CERTFILE=$1
else
	echo "Usage: $0 [certfile] [descripton]"
	exit 1
fi

DESCRIPTION=$2
if [ -z "$DESCRIPTION" ]; then
	# if $2 is not set then just use the cert filename as the description for convenience
	DESCRIPTION=$CERTFILE
fi


# get the ID of the user keyring
USERKEYRING=$(keyctl show @u | awk '/keyring: _uid/{print $1}')
if ! [[ $USERKEYRING =~ $ISNUMERIC ]] ; then
	echo "failed to retrieve user keyring"
	exit 128
fi

# get the id of the .platform keyring
PLATFORMID=$(keyctl show %:.platform | awk '/keyring: .platform/{print $1}')
if ! [[ $USERKEYRING =~ $ISNUMERIC ]] ; then
	echo "failed to retrieve platform keyring"
	exit 128
fi

# add our key to the userkeyring
KEYID=$(cat $CERTFILE |  keyctl padd  asymmetric "$DESCRIPTION" $USERKEYRING)
if ! [[ $USERKEYRING =~ $ISNUMERIC ]] ; then
	echo "failed to add $CERTFILE to user keyring"
	exit 128
fi

# load the keyloader kernel mod to link our key 
# from the user keyring to the .platform keyring
# (currectly echo'd out for testing)
echo insmod ./keyloader-kmod.ko keyid=$KEYID platformid=$PLATFORMID

# Now our key is in the platform keyring and any kmods signed with it will load without error
