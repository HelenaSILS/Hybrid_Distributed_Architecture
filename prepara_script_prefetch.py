import os
import sys


samplesfile = sys.argv[1];
cmdsfile = sys.argv[2];
outfile = sys.argv[3];

p=0

samples = open(samplesfile, 'r')
commands = open(cmdsfile, 'r')
output = open(outfile, 'w')

linesSample = samples.readlines()
linesCommands = commands.readlines()

for srr in linesSample:
    for cmd in linesCommands:
        new = cmd.replace("$", srr.rstrip())
        new = new.rstrip() + "&\n"
        output.write(new)
            # p=p+1
            # p=p%6
            # if p==0:
            #     print("$EXEC -X 100000000 " + srr.rstrip() + " -O /scratch/inova-covd19/helena.silva/MeusTestes/experimento01 ;")
            # else:
            #     print("$EXEC -X 100000000 " + srr.rstrip() + " -O /scratch/inova-covd19/helena.silva/MeusTestes/experimento01 &")
samples.close()
commands.close()

        