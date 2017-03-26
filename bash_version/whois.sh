DOMAIN=$1
SEARCH_DOMAIN="domain $DOMAIN"
OUTPUT=$(whois -h whois.verisign-grs.com "$SEARCH_DOMAIN")

NO_MATCH=$(echo ${OUTPUT} | grep 'No match')

EXP_DATE=$(grep "Expiration Date" <<< "${OUTPUT}" | awk '{print $3}' | sed 's/ //g')
#echo $EXP_DATE

if [[ -z $EXP_DATE ]]; then
    if [[ -z $NO_MATCH ]]; then
	echo -1
    else
	echo 0
    fi
else
    IFS='-' read -a ARR <<< "$EXP_DATE"

    case "${ARR[1]}" in
	"jan") E_MONTH=1;;
	"feb") E_MONTH=2;;
	"mar") E_MONTH=3;;
	"apr") E_MONTH=4;;
	"may") E_MONTH=5;;
	"jun") E_MONTH=6;;
	"jul") E_MONTH=7;;
	"aug") E_MONTH=8;;
	"sep") E_MONTH=9;;
	"oct") E_MONTH=10;;
	"nov") E_MONTH=11;;
	"dec") E_MONTH=12;;
	*) echo "ERR"; exit 1;;
    esac
    DELIM="-"
    FORMATDAY=${ARR[2]}$DELIM$E_MONTH$DELIM${ARR[0]}
#    echo $FORMATDAY
    
    _TODAY=$(date +%s)
    if [ $(uname) == "Darwin" ]; then
	_EXPDAY=$(date -jf "%Y-%m-%d" $FORMATDAY +%s)
    else
	_EXPDAY=$(date -d $FORMATDAY +%s)
    fi
    if [ $_TODAY -gt $_EXPDAY ]; then
	echo 1
    else
	echo -1
    fi
fi