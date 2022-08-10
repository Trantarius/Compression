 
g++ -std=c++17 $(find src -name "*.cpp") -o exe -IUtil
echo
echo compiled
echo
./exe
rm exe
