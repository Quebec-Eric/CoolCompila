#!/bin/bash


flex_file="cool.flex"


temp_file="temp.l"

last_line=$(grep -n '^%%$' $flex_file | tail -n 1 | cut -d : -f 1)

head -n $(($last_line - 1)) $flex_file > $temp_file
tail -n +$(($last_line + 1)) $flex_file > temp_after.txt

for ref_file in "$@"; do
  if [ -f "$ref_file" ]; then
    echo "/*" >> $temp_file
    echo "  * --------------------------------------------" >> $temp_file
    echo "  * Arquivo $ref_file :" >> $temp_file
    echo "  * --------------------------------------------" >> $temp_file
    echo " */" >> $temp_file
    cat $ref_file >> $temp_file
    echo -e "\n" >> $temp_file
  
  fi
done


echo -e "\n%%" >> $temp_file
cat temp_after.txt >> $temp_file


mv $temp_file $flex_file

rm temp_after.txt

echo "O arquivo $flex_file foi atualizado."
