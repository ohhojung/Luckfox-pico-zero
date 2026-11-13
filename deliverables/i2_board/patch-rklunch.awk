# Only replace the two verified camera launch statements; preserve OEM setup.
BEGIN { count=0 }
/^[[:space:]]*rkipc( -a \/oem\/usr\/share\/iqfiles)? &[[:space:]]*$/ {
    print "        if [ -e /userdata/cctv-i2.enabled ]; then"
    print "                touch /tmp/cctv-oem-ready"
    print "                /etc/init.d/S59cctv start"
    print "        else"
    print $0
    print "        fi"
    count++
    next
}
{ print }
END { if (count != 2) exit 1 }
