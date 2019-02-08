#!/bin/bash

[ -z "${P4PORT}" ] && P4PORT=${PARABUILD_P4PORT}
[ -z "${P4USER}" ] && P4USER=${PARABUILD_P4USER}
[ -z "${P4CLIENT}" ] && P4CLIENT=${PARABUILD_P4CLIENT}

OOMA_RELEASE_CHANGE=$(/opt/bin/p4 changes -m1 ... | awk '{print $2}')
OOMA_STRIP_PATH=$(/opt/bin/p4 where ... | awk '{print $1}' | sed 's/.\{3\}$//')
OOMA_RELEASE_FILE=$(/opt/bin/p4 describe ${OOMA_RELEASE_CHANGE} | grep "... //" | head -1 | awk -F# '{print $1}' | awk '{print $NF}' | sed "s/^.\{${#OOMA_STRIP_PATH}\}//")
if [ "$(/opt/bin/p4 where ... | grep gitswarm | wc -l)" != "0" ]; then
   OOMA_SHA1=$(/opt/bin/p4 files //.git-fusion/objects/repos/gitswarm-nw_sw-lte-service/commits/...,${OOMA_RELEASE_CHANGE} | cut -c64-105 | sed "s|/||g")
   while [ "${#OOMA_SHA1}" = "0" ]; do
       sleep 30
       recount=`expr $recount + 1`
       OOMA_SHA1=$(/opt/bin/p4 files //.git-fusion/objects/repos/gitswarm-nw_sw-lte-service/commits/...,${OOMA_RELEASE_CHANGE} | cut -c64-105| sed "s|/||g")
       if [ "${recount}" -gt "20" ]; then
	   echo "Giving up - no valid sha - aborting - daisy - daisy"
	   exit 1
       fi
   done
else
	OOMA_GIT_CHANGE=$(/opt/bin/p4 filelog $(/opt/bin/p4 filelog ${OOMA_RELEASE_FILE} | grep "branch from" | head -1 | awk '{print $NF}') | grep change | head -1 | awk '{print $4}')
	OOMA_SHA1=$(/opt/bin/p4 describe ${OOMA_GIT_CHANGE} | grep ghost-of-sha1 | awk '{print $NF}')
	if [ -z "${OOMA_SHA1}" ]; then
	    OOMA_SHA1=$(/opt/bin/p4 files //.git-fusion/objects/repos/gitswarm-nw_sw-lte-service/commits/...,${OOMA_GIT_CHANGE} | cut -c64-105| sed "s|/||g")
	fi
fi

echo $OOMA_SHA1

