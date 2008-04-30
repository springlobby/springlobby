grep -R "_(" ../src/* | cut -d ':' -f 1 | sort -u | sed -e 's/..\///' > ../po/POTFILES.in
