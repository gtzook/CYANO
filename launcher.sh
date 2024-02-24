#!/bin/bash
cd /home/cyano/CYANO/
. /home/cyano/CYANO/.setup
sudopy /home/cyano/CYANO/main.py >> logs/output.txt 2>&1
