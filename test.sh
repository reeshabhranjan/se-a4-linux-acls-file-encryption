echo "---BUILDING FILES---"
echo ""

make build && sudo make perm
cd files

echo ""
echo ""
echo "---TESTING FPUT---"
echo ""

echo "Reeshabh" > input.txt
echo ">> Calling myfput"
./myfput abc.txt < input.txt
echo ">> Calling myfget"
./myfget abc.txt

echo ""
echo ">> Changing checksum, keeping the length same..."
echo ""

num_bytes=$(wc -c abc.txt.sign | awk {'print $1'})
echo "" > abc.txt.sign
for ((i = 1; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo ">> Calling myfget"
./myfget abc.txt

echo ""
echo ">> Changing checksum, keeping the length different..."
echo ""

echo "" > abc.txt.sign
for ((i = 0; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo ">> Calling myfget"
./myfget abc.txt

echo ""
echo ">> Removing checksum file..."
echo ""

rm abc.txt.sign
echo ">> Calling myfget"
./myfget abc.txt


echo ""
echo ""
echo "---TESTING FPUT_ENCRYPT---"
echo ""

echo "Reeshabh" > input.txt
echo ">> Calling fput_encrypt"
./fput_encrypt abc.txt < input.txt
echo ">> Calling fget_decrypt"
./fget_decrypt abc.txt

echo ""
echo ">> Changing checksum, keeping the length same..."
echo ""

num_bytes=$(wc -c abc.txt.sign | awk {'print $1'})
echo "" > abc.txt.sign
for ((i = 1; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo ">> Calling fget_decrypt"
./fget_decrypt abc.txt

echo ""
echo ">> Changing checksum, keeping the length different..."
echo ""

echo "" > abc.txt.sign
for ((i = 0; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo ">> Calling fget_decrypt"
./fget_decrypt abc.txt

echo ""
echo ">> Removing checksum file..."
echo ""

rm abc.txt.sign
echo ">> Calling fget_decrypt"
./fget_decrypt abc.txt


echo ""
echo ""
echo "---TESTING FPUT_ENCRYPT_RSA---"
echo ""

echo "Reeshabh" > input.txt
echo ">> Calling fput_encrypt_rsa"
./fput_encrypt_rsa abc.txt < input.txt
echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt

echo ""
echo ">> Changing checksum, keeping the length same..."
echo ""

num_bytes=$(wc -c abc.txt.sign | awk {'print $1'})
echo "" > abc.txt.sign
for ((i = 1; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt

echo ""
echo ">> Changing checksum, keeping the length different..."
echo ""

echo "" > abc.txt.sign
for ((i = 0; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt

echo ""
echo ">> Removing checksum file..."
echo ""

rm abc.txt.sign
echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt

echo ""
echo ">> Again calling fput_encrypt_rsa to test changes with credential files..."
echo ""

echo ">> Calling fput_encrypt_rsa"
./fput_encrypt_rsa abc.txt < input.txt

echo ""
echo ">> Removing reeshabh.private, reeshabh.public..."
echo ""
mv part-2/reeshabh.private part-2/reeshabh1.private
mv part-2/reeshabh.public part-2/reeshabh1.public

echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt

echo ""
echo ">> Restoring reeshabh.private, reeshabh.public..."
echo ""
mv part-2/reeshabh1.private part-2/reeshabh.private
mv part-2/reeshabh1.public part-2/reeshabh.public

echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt