from unidecode import unidecode


file_read = open("rawdata.txt","r")
file_write = open("dcodedata.txt","w")
string = file_read.read()
# while ((str = file_read.readline()) == None):
print(string)
string = unidecode(string)
file_write.write(string)

file_write.close()
file_read.close()
count = 0
temp = []
weight = 1
print("=============================================================")
with open("dcodedata.txt","r") as file_read:
    line = file_read.readlines()
    while(count< len(line)):
        if  (count%3 == 0):
            temp = line[count]
            print(temp)
        if  (count%3 == 1):
            add = "data/" + "f" + temp.rstrip() + ".txt"
            file_write = open(add,"w")
            content = line[count].split(" - ")
            print(content,end = '.')
            c = 0
            for dot in content :
                dot = dot.rstrip()
                if(c != 0):
                    file_write.write(str(weight))
                else :
                    file_write.write("0");
                    c+=1
                file_write.write(" : ")
                file_write.write(dot)
                file_write.write("\n")
                print(dot)
            file_write.close()
        if  (count%3 == 2):
            add = "data/" + "b" + temp.rstrip() + ".txt"
            file_write = open(add,"w")
            content = line[count].split(" - ")
            print(content,end = '.')
            c = 0
            for dot in content :
                dot = dot.rstrip()
                if(c != 0):
                    file_write.write(str(weight))
                else :
                    file_write.write("0");
                    c+=1
                file_write.write(" : ")
                file_write.write(dot)
                file_write.write("\n")
                print(dot)
            file_write.close()
        count += 1
file_read.close()