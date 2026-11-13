#!/bin/sh
# Usage: check-health.sh STATUS_FILE EXPECTED_PID
[ "$#" -eq 2 ] || exit 2
case "$2" in ''|*[!0-9]*) exit 2 ;; esac
[ -r "$1" ] || exit 1
read -r uptime rest < /proc/uptime
awk -F= -v now="$uptime" -v expected="$2" '
$1 == "state" { state=$2; seen++ }
$1 == "pid" { pid=$2; seen++ }
$1 == "monotonic_ms" { stamp=$2; seen++ }
$1 == "main_frames" { main=$2; seen++ }
$1 == "sub_frames" { subframes=$2; seen++ }
$1 == "main_last_ms" { lastmain=$2; seen++ }
$1 == "sub_last_ms" { lastsub=$2; seen++ }
END {
 if(seen!=7 || state!="running" || pid!=expected || main!~/^[0-9]+$/ || subframes!~/^[0-9]+$/ || stamp!~/^[0-9]+$/ || lastmain!~/^[0-9]+$/ || lastsub!~/^[0-9]+$/) exit 1;
 now*=1000;
 if(main<=0 || subframes<=0 || now<stamp || now-stamp>5000 || now<lastmain || now-lastmain>5000 || now<lastsub || now-lastsub>5000) exit 1;
 exit 0;
}' "$1"
