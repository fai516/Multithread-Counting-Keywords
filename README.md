# Multithread-Counting-Keywords
This is a simple c++ program that can counting keywords in multiple files (using multithread feature)

##Usage
`make`
You can choose the number of the reader(mapper), counter(reducer), and the keyword

##how it works
1.The program will first grab all the file name from `files.dat` and put it into the `file queue`.
2.Depends on the number of threads you choose, they will start to read file from the `file queue`.
3.Each `mapper` will be designated a single file and start to count. Then it will push the result into the `count queue`
4.Cocurrenly, each `reducer` will grab the `first 2 number` from the `count queue`, sum them up and push it back to `count queue`
5.The program will stop when the size of `file queue` is equal to `0` and the size of `count queue` is equal to `1`
6.The result(total count of keyword among text files) will be the only element of `count queue`

##how to add new file in program
1.Put the text files in side the data folder
2.Add a new line for the name(includes extension) of the text file in `files.dat`
3.Run the program

##Difficulty
Since both the `mapper` and `reducer` threads run concurrenly, there might have a chance that the `reducer` threads run ahead the `mapper` threads. 

The `reducer` thread will stop when the size of `count queue` is equal `1`. But what if some files are too large to read that the `mapper` threads take very long time? The `reducer` will stop before the `mapper` push the new count, which resulting the `count queue` ended up more than one element.

To solve this, I added a positive integer value(`currentMapperWorking`) representing the number of `mapper` thread that are currently working. `0` means no `mapper` is working and `>0` means some `mappers` are working. The `reducer` thread only stop working when `currentMapperWorking` is equal to `0` and the size of `count queue` is equal to `1`