rm -r build/*
rm -r release/bin/*
rm -r release/lib/*
rm -r debug/bin/*
rm -r debug/lib/*

cd build

cmake -DDEBUG=ON .. && make -j12

cd ..