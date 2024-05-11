#!/bin/bash

file="$1"
izolated_space_dir="$2"

echo "Analizarea fișierului $file..."


if [ ! -r "$file" ] || [ ! -w "$file" ] || [ ! -x "$file" ]; then
    echo "Fișierul $file are drepturi lipsă. Se va realiza analiza sintactică..."

   
    if grep -qE 'corrupted|dangerous|risk|attack|malware|malicious' "$file" || LC_ALL=C grep -q '[^ -~]' "$file"; then
        echo "Fișierul $file este potențial periculos sau corupt. Va fi izolat în $izolated_space_dir."
        mv "$file" "$izolated_space_dir"
    else
        echo "Fișierul $file nu prezintă semne de maleficență sau corupție."
    fi
else
    echo "Fișierul $file are toate drepturile și nu poate fi verificat."
fi

echo "Analiza fișierului $file finalizată."

