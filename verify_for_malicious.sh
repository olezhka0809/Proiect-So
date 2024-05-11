#!/bin/bash

file="$1"
izolated_space_dir="$2"

echo "Analizarea fișierului $file..."

if [ ! -r "$file" ] || [ ! -w "$file" ] || [ ! -x "$file" ]; then
    echo "Fișierul $file are drepturi lipsă. Se va realiza analiza sintactică..."

    
    min_lines=3
    max_words=1000
    max_chars=2000

    
    line_count=$(wc -l < "$file")
    word_count=$(wc -w < "$file")
    char_count=$(wc -m < "$file")

    
    if (( line_count < min_lines )) && (( word_count > max_words )) && (( char_count > max_chars )); then
        echo "Fișierul $file este potențial periculos sau corupt. Va fi izolat în $izolated_space_dir."
        mv "$file" "$izolated_space_dir"
    else
        
        if LC_ALL=C grep -q '[^ -~]' "$file"; then
            echo "Fișierul $file conține caractere non-ASCII. Este suspect și va fi izolat în $izolated_space_dir."
            mv "$file" "$izolated_space_dir"
        else
            
            if grep -qE 'corrupted|dangerous|risk|attack|malware|malicious' "$file"; then
                echo "Fișierul $file conține cuvinte cheie asociate fișierelor periculoase. Este suspect și va fi izolat în $izolated_space_dir."
                mv "$file" "$izolated_space_dir"
            else
                echo "Fișierul $file nu prezintă semne de maleficență sau corupție."
            fi
        fi
    fi
else
    echo "Fișierul $file are toate drepturile și nu poate fi verificat."
fi

echo "Analiza fișierului $file finalizată."

