#!/bin/bash
for i in *.ppm;
  do name=`echo "$i" | cut -d'.' -f1`
  echo "$name"
  pnmtopng "$i" > "${name}.png"
done
