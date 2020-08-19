startline="0"
logowidth="53"
readarray -t fulloutput < ascii-logo.ans

cnt=${#fulloutput[@]}
for ((i=0;i<cnt;i++)); do
    fulloutput[i]="${fulloutput[i]} %s"
done

labelcolor=$c2

