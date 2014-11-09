#!/bin/tcsh
# DAMON: JULY 4, 2007
# runs the ssearch program and creates varius outputs
# echo USAGE: ./run-okapi-2007.sh followed by arguments as follows
# database-name        : the name of database okapi opens
# passage-index-name   : the index used for retrieval of terms
# passageID-index-name : the index used for retrieval of passages given passage id
# topics-file          : file that contains the topics
# feedback-weight      : weight given to feedback terms
# number-of-outputs    : number of passages retrieved for each topic
# #-of-top-passages    : number of top passages returned in a file for manual view
# expanded weight      : this factor will be multiplied by the weight of each expanded terms
# expanded file         : this file contains the starting index of expanded terms
# run-name             : run tag of each output for trec purposes

#source env.cshrc
java -Djava.library.path=. ssearch $1 $2 $3 $4 $5 $6 $7 $8 $9 > ssearch-output
cd ./$1-$2-$3-$4-$5-$6-$7-$8/
cat doc* > combined-doc
cat txt* > combined-txt
rm doc*
rm txt*
cd ..
javac TopicTermCounter.java
java -Djava.library.path=. TopicTermCounter $1 $2 $3 $4 ./$1-$2-$3-$4-$5-$6-$7-$8/combined-doc $6 > ./$1-$2-$3-$4-$5-$6-$7-$8/term-count-of-docs
java -Djava.library.path=. TopicTermCounter $1 $2 $3 $4 ./$1-$2-$3-$4-$5-$6-$7-$8/combined-txt $6 > ./$1-$2-$3-$4-$5-$6-$7-$8/term-count-of-txts
javac FeedbackTermCounter.java
java -Djava.library.path=. FeedbackTermCounter $1 $2 $3 ./$1-$2-$3-$4-$5-$6-$7-$8/feedback-terms ./$1-$2-$3-$4-$5-$6-$7-$8/combined-txt $6 > ./$1-$2-$3-$4-$5-$6-$7-$8/term-count-of-feedbacks
cd ./$1-$2-$3-$4-$5-$6-$7-$8/
cp ../OutputFormatter.java .
javac OutputFormatter.java
java OutputFormatter combined-doc $10
java OutputFormatter combined-txt $10
echo doc term statistics > statistics-of-term-counts
cat term-count-of-docs | grep '##' >> statistics-of-term-counts
echo >> statistics-of-term-counts
echo txt term statistics >> statistics-of-term-counts
cat term-count-of-txts | grep '##' >> statistics-of-term-counts
echo >> statistics-of-term-counts
echo feedback term statistics >> statistics-of-term-counts
cat term-count-of-feedbacks | grep '##' >> statistics-of-term-counts
echo >> statistics-of-term-counts
rm *.java
rm *.class
#rm combined*
mv ../ssearch-output .
cd ..
javac retrievePassages.java
java -Djava.library.path=. retrievePassages $1 $3 ./$1-$2-$3-$4-$5-$6-$7-$8/top-passageIDs
mv top-passages ./$1-$2-$3-$4-$5-$6-$7-$8/
cd ./$1-$2-$3-$4-$5-$6-$7-$8/
chmod 644 *
