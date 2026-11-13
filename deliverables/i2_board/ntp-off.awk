# Refuse unknown/ambiguous layouts; change only network.ntp enable.
/^[ \t]*\[/ { in_ntp = ($0 ~ /^[ \t]*\[network\.ntp\][ \t]*\r?$/) }
in_ntp && /^[ \t]*enable[ \t]*=/ {
    count++
    if ($0 !~ /^[ \t]*enable[ \t]*=[ \t]*[01][ \t]*\r?$/) bad=1
    sub(/=[ \t]*[01]/, "=0")
}
{ print }
END { if (count != 1 || bad) exit 1 }
