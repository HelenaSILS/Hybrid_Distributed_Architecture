import os
from xml.etree.ElementTree import tostring
i=1.00002
j=0
while(j<1000000):
    i=i+1
    j=j+1
    #print(i)
    if j%100==0 :
        s = "echo "
        s = s + str(i)
        s = s + " >> out2.txt"
        #print(s)
        os.system(s)