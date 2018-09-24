import string
import random

myChars = string.letters[:26]

randChars = ""
for i in xrange(10):
    randChars+=random.choice(myChars)
print "file1 contents are ", randChars
f = open('./file1.txt', 'w')
f.write(randChars+'\n')
f.close()

randChars = ""
for i in xrange(10):
    randChars+=random.choice(myChars)
print "file2 contents are ", randChars
f = open('./file2.txt', 'w')
f.write(randChars+'\n')
f.close()

randChars = ""
for i in xrange(10):
    randChars+=random.choice(myChars)
print "file3 contents are ", randChars
f = open('./file3.txt', 'w')
f.write(randChars+'\n')
f.close()

randInt1 = random.randint(1,42)
randInt2 = random.randint(1,42)
myAns = randInt1 + randInt2
print "\nFirst random int = ", randInt1
print "Second random int = ", randInt2
print randInt1," + ",randInt2," = ",myAns
