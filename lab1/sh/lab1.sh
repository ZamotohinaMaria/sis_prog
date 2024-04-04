rm -R ROOT
mkdir ROOT
mkdir ROOT/a_1
mkdir ROOT/a_2
mkdir ROOT/a_3
mkdir ROOT/a_2/b_2
mkdir ROOT/a_3/b_1

cd ROOT
truncate -s 227 a_0.bin

cd a_3
cd b_1
touch c_0.bin
cd ..
cd ..

ln -s a_3/b_1/c_0.bin a_1/b_0.bin
ln -s a_3/b_1 a_2/b_2/c_1.bin
ln a_0.bin a_2/b_2/c_2.bin

cd ..
ls -R -h -l ROOT

