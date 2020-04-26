redcolor='\033[0;31m'
yellowcolor='\033[1;33m'
resetcolor='\033[0m'
echo -e "${redcolor}---BUILDING FILES---${resetcolor}"
echo ""

make build && sudo make perm
cd files

echo ""
echo ""
echo -e "${redcolor}---TESTING FPUT---${resetcolor}"
echo ""

echo "Reeshabh" > input.txt
echo ">> Calling myfput"
./myfput abc.txt < input.txt
echo ">> Calling myfget"
./myfget abc.txt

echo ""
echo -e "${yellowcolor}>> Changing checksum, keeping the length same...${resetcolor}"
echo ""

num_bytes=$(wc -c abc.txt.sign | awk {'print $1'})
echo "" > abc.txt.sign
for ((i = 1; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo -e "${yellowcolor}>> Calling myfget${resetcolor}"
./myfget abc.txt

echo ""
echo -e "${yellowcolor}>> Changing checksum, keeping the length different...${resetcolor}"
echo ""

echo "" > abc.txt.sign
for ((i = 0; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo -e "${yellowcolor}>> Calling myfget${resetcolor}"
./myfget abc.txt

echo ""
echo -e "${yellowcolor}>> Removing checksum file...${resetcolor}"
echo ""

rm abc.txt.sign
echo -e "${yellowcolor}>> Calling myfget${resetcolor}"
./myfget abc.txt


echo ""
echo ""
echo -e "${redcolor}---TESTING FPUT_ENCRYPT---${resetcolor}"
echo ""

echo "Reeshabh" > input.txt
echo -e "${yellowcolor}>> Calling fput_encrypt${resetcolor}"
./fput_encrypt abc.txt < input.txt
echo ">> Calling fget_decrypt"
./fget_decrypt abc.txt

echo ""
echo -e "${yellowcolor}>> Changing checksum, keeping the length same...${resetcolor}"
echo ""

num_bytes=$(wc -c abc.txt.sign | awk {'print $1'})
echo "" > abc.txt.sign
for ((i = 1; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo -e "${yellowcolor}>> Calling fget_decrypt${resetcolor}"
./fget_decrypt abc.txt

echo ""
echo -e "${yellowcolor}>> Changing checksum, keeping the length different...${resetcolor}"
echo ""

echo "" > abc.txt.sign
for ((i = 0; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo -e "${yellowcolor}>> Calling fget_decrypt${resetcolor}"
./fget_decrypt abc.txt

echo ""
echo -e "${yellowcolor}>> Removing checksum file...${resetcolor}"
echo ""

rm abc.txt.sign
echo -e "${yellowcolor}>> Calling fget_decrypt${resetcolor}"
./fget_decrypt abc.txt


echo ""
echo ""
echo -e "${redcolor}---TESTING FPUT_ENCRYPT_RSA---${resetcolor}"
echo ""

echo "Reeshabh" > input.txt
echo -e "${yellowcolor}>> Calling fput_encrypt_rsa${resetcolor}"
./fput_encrypt_rsa abc.txt < input.txt
echo ">> Calling fget_decrypt_rsa"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Changing checksum, keeping the length same...${resetcolor}"
echo ""

num_bytes=$(wc -c abc.txt.sign | awk {'print $1'})
echo "" > abc.txt.sign
for ((i = 1; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Changing checksum, keeping the length different...${resetcolor}"
echo ""

echo "" > abc.txt.sign
for ((i = 0; i < $num_bytes; i++));
do
    echo -n "a" >> abc.txt.sign
done
echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Removing checksum file...${resetcolor}"
echo ""

rm abc.txt.sign
echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Again calling fput_encrypt_rsa to test changes with credential files...${resetcolor}"
echo ""

echo ">> Calling fput_encrypt_rsa"
./fput_encrypt_rsa abc.txt < input.txt

echo ""
echo -e "${yellowcolor}>> Removing reeshabh.private, reeshabh.public...${resetcolor}"
echo ""
mv part-2/reeshabh.private part-2/reeshabh1.private
mv part-2/reeshabh.public part-2/reeshabh1.public

echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Restoring reeshabh.private, reeshabh.public...${resetcolor}"
echo ""
mv part-2/reeshabh1.private part-2/reeshabh.private
mv part-2/reeshabh1.public part-2/reeshabh.public

echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt