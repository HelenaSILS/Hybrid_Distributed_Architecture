import os
from xml.etree.ElementTree import tostring
import sys


max = sys.argv[1];
name = "file_"+str(max) + ".txt"
print(name)
print(100*int(max))
i=1.00002
j=0
while(j<500):
    i=i+1
    j=j+1
    s = "echo "
    s = s + str(i)
    s = s + " >> "
    s = s + name
    print(s)
    os.system(s)