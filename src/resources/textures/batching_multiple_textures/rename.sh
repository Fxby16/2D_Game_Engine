#!/bin/bash

count=1

for file in *.png; do
  new_name=$(printf "%d.png" "$count")
  mv "$file" "$new_name"
  count=$((count+1))
done

