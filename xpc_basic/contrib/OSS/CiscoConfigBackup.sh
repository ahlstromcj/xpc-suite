#!/bin/bash
#########################################################################
# Name: cisco_tftp_backup
# Author: Steve Cowles <scowles@infohiiway.com>
#
# Revision: Created 10/1/2005 SWC
# Revision: 03/26/06 - SWC 
#           Added capability to save config data by date.
#
# Description: Shell Script to backup cisco startup-config files
#              using tftp and store in pre-defined directory structure
#
# Directory structure for script is:
#   base_dir             /backup/cisco/{date}
#     device type           /router
#        hostname              /r1
#           filename              startup-config
#        hostname              /r2
#           filename              startup-config
#     device type           /switch
#        hostname              /s1
#           filename              startup-config
#        hostname              /s2
#           filename              startup-config
#
# Note 1: Cisco IOS requires you to configure tftp server access prior
#         to adding a router or switch to this script
# Something like:
#   access-list 55 remark PERMIT hosts requesting TFTP access
#   access-list 55 permit host 10.1.100.201
#   tftp-server nvram:startup-config 55
#
# 10.1.100.201 is the IP address of the system running this script
# nvram:startup-config is the only file allowed to be copied
#########################################################################

####################################################
# Error handlers
error_no_tftp_exec ()
{
        echo "ERROR - Filename '$1' does not exist"
        exit
}

error_no_backup ()
{
        echo "ERROR - Unable to backup Host '$1'"
}

error_zero_length ()
{
        echo "ERROR - Hostname '$1' backup has a zero size"
}
####################################################

# If tftp executable does not exist... then exit
CMD=tftp ; TFTP=`which ${CMD} 2>/dev/null`
[ ! -x "${TFTP}" ] && error_no_tftp_exec ${CMD}

####################################################
# Variable Section, edit below to meet requirements
####################################################

# Define/load a variable to store the device information
# of all routers/switches to backup using tftp.
#
# Values are separated by colons (:)
#     Value 1 = Device Type        (sub-dir created)
#     Value 2 = Device Hostname    (sub-dir created)
#     Value 3 = Device IP address
DEVICES="
router:allar1:10.100.12.1
switch:allsw1:10.100.12.2
router:lewar1:10.100.16.1
switch:lewsw1:10.100.16.2
"

# Should an error occur during execution, e-mail
# errors to following person. i.e. If run as cronjob
MAILTO="scowles@infohiiway.com"

# Define the base directory where you want to store
# files retreived from all devices. All subordinate
# directories will created relative to this base
BASELOG=/var/log/cisco_configs
BASEDIR=${BASELOG}/`date +%m.%d.%y`

# Define the filename to (get) from device using tftp
# See note 1 above
FILENAME=startup-config

####################################################
# END Variable Section, No servicable parts below
####################################################

###################################################################
# Begin Executable Section (do NOT edit below)
###################################################################

# If base directory does not exist, create it
[ ! -d ${BASEDIR} ] && mkdir ${BASEDIR}

# Update the symbolic link to point to the current BASEDIR directory
if [ -d ${BASELOG} ] ; then
        # Router link
        rm ${BASELOG}/router
        ln -s ${BASEDIR}/router ${BASELOG}/router

        # Switch link
        rm ${BASELOG}/switch
        ln -s ${BASEDIR}/switch ${BASELOG}/switch
fi

########################################################
# Setup loop for each device listed in $DEVICES variable
########################################################
for device in $DEVICES ; do
        # Separate DEVICE TYPE/HOSTNAME/IP into separate varaibles
        DEVICE=`echo ${device}   | cut -d ':' -f 1`
        HOSTNAME=`echo ${device} | cut -d ':' -f 2`
        IP=`echo ${device}       | cut -d ':' -f 3`

        # Set and Create the sub-directories to store files
        SUBDIR=${BASEDIR}/${DEVICE}
        [ ! -d ${SUBDIR} ] && mkdir ${SUBDIR}
        [ ! -d ${SUBDIR}/${HOSTNAME} ] && mkdir ${SUBDIR}/${HOSTNAME}

        # Set the fullpath to store file retreived during tftp
        FULLPATH=${SUBDIR}/${HOSTNAME}/${FILENAME}

        # tftp $FILENAME from device/IP to directory/FULLPATH
        ${TFTP} $IP -c get ${FILENAME} ${FULLPATH} >/dev/null 2>&1 ||
        error_no_backup ${HOSTNAME}

        # Since tftp return values don't seem to include zero length
        # gets, test for this condition
        [ ! -s ${FULLPATH} ] && error_zero_length ${HOSTNAME}
done
