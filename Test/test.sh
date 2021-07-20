i=1;
for user in "$@" 
do
   
    i=$((i + 1));
done

rm -rf graph.log
rm -rf graph.dot
rm -rf graph.png
grep 'dot graph' check.log -A1   >> graph.log

SRC=';'
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log
SRC='--'
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log
SRC='"'
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log
SRC='%'
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log
SRC='\'
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph START"
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph entry "
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target entry end"
DST='"'"->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target line desc "
DST='['"label="'"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target line desend"
DST='"'" ]"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target color desc "
DST='['"color="'"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target color desend"
DST='"'" ]"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log



SRC="dot graph target loc start "
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target loc end"
DST='"'"->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target loc en1"
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log



SRC="dot graph target basicblock start "
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target basicblock end"
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph target basicblock en1"
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log




SRC="dot graph start relate form "
DST="->"'"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph start relate for1 "
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph end relate end"
DST='"'"->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph end relate en1"
DST='"'"->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph relate stmt start "
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph relate stm1 start "
DST="->"'"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph relate stm2 start "
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph relate end"
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph relate en1"
DST='"'"->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph arrow"
DST="->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph stmt start "
DST="->"'"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph stmt star1 "
DST="->"'"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log

SRC="dot graph stmt end"
DST='"'
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log


SRC="dot graph stmt en1"
DST='"'"->"
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log


SRC="dot graph END"
DST=""
SRC=$(echo $SRC)
DST=$(echo $DST)
sed -i "s/$SRC/$DST/g" ./graph.log



value=`cat graph.log`
echo "digraph {rankdir = LR; rank=same; nodesep=1.0;" "$value" "}" >> graph.dot
dot -Tsvg -o graph2.svg  graph.dot 
gvpr -f graphinfo.gvpr graph.dot 
echo "Username: $user";