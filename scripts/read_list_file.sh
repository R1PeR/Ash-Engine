lines=`cat $1`
formattedLines=""
for line in $lines
do
    if [ $# -eq 1 ] 
    then
        currentLine="${line}"
    else
        currentLine="$2${line}"
    fi
    formattedLines="$currentLine $formattedLines"
done
formattedLines=`echo ${formattedLines} | sed 's/ *$//'`
echo "$formattedLines"
