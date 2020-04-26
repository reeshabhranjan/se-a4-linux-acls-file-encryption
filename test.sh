redcolor='\033[0;31m'
yellowcolor='\033[1;33m'
bluecolor='\033[1;34m'
resetcolor='\033[0m'
echo -e "${redcolor}---BUILDING FILES---${resetcolor}"
echo ""

make build && sudo make perm
cd files

echo ""
echo ""
echo -e "${redcolor}---PREPROCESSING---${resetcolor}"
echo ""
echo -e "${yellowcolor}>> Cleaning temporary files used in this test.${resetcolor}"
sudo rm abc.*
echo ""
echo -e "${yellowcolor}>> Creating a directory with no write permissions.${resetcolor}"
mkdir inaccessible_directory
sudo chmod 000 inaccessible_directory
echo -e "${bluecolor}[ACL of inaccessible_directory]${resetcolor}"
./getacl inaccessible_directory

echo ""
echo ""
echo -e "${redcolor}---TESTING FPUT, FGET---${resetcolor}"
echo ""

echo "Reeshabh" > input.txt
echo -e "${yellowcolor}>> Calling myfput${resetcolor}"
./myfput abc.txt < input.txt
echo -e "${yellowcolor}>> Calling myfget${resetcolor}"
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

echo -e ""
echo -e "${yellowcolor}>> Calling myfput on a large input${resetcolor}"
./myfput abc.txt < large_input.txt
echo -e "${yellowcolor}>> Calling myfget${resetcolor}"
./myfget abc.txt

echo ""
echo -e "${yellowcolor}>> Trying to create a new file in inaccessible_directory (with no write permission)...${resetcolor}"
echo ""

./myfput inaccessible_directory/non_existent.txt

echo ""
echo ""
echo -e "${redcolor}---TESTING FPUT_ENCRYPT, FGET_DECRYPT---${resetcolor}"
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

echo -e ""
echo -e "${yellowcolor}>> Calling fput_encrypt on a large input${resetcolor}"
./fput_encrypt abc.txt < large_input.txt
echo -e "${yellowcolor}>> Calling fget_decrypt${resetcolor}"
./fget_decrypt abc.txt

echo ""
echo -e "${yellowcolor}>> Trying to create a new file in inaccessible_directory (with no write permission)...${resetcolor}"
echo ""

./fput_encrypt inaccessible_directory/non_existent.txt

echo ""
echo ""
echo -e "${redcolor}---TESTING FPUT_ENCRYPT_RSA, FGET_DECRYPT_RSA---${resetcolor}"
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

echo ""
echo -e "${yellowcolor}>> Changing ownership of RSA-files${resetcolor}"
./setacl -m u:reeshabh:--- part-2/reeshabh.private
./setacl -m u:reeshabh:--- part-2/reeshabh.public
echo -e "${bluecolor}[ACL of part-2/reeshabh.private]${resetcolor}"
./getacl part-2/reeshabh.private
echo -e "${bluecolor}[ACL of part-2/reeshabh.public]${resetcolor}"
./getacl part-2/reeshabh.public

echo ""
echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Restoring ownership of RSA-files${resetcolor}"
./setacl -m u:reeshabh:r-- part-2/reeshabh.private
./setacl -m u:reeshabh:r-- part-2/reeshabh.public
echo -e "${bluecolor}[ACL of part-2/reeshabh.private]${resetcolor}"
./getacl part-2/reeshabh.private
echo -e "${bluecolor}[ACL of part-2/reeshabh.public]${resetcolor}"
./getacl part-2/reeshabh.public

echo ""
echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo -e ""
echo -e "${yellowcolor}>> Calling fput_encrypt_rsa on a large input${resetcolor}"
./fput_encrypt_rsa abc.txt < large_input.txt
echo -e "${yellowcolor}>> Calling fget_decrypt_rsa${resetcolor}"
./fget_decrypt_rsa abc.txt

echo ""
echo -e "${yellowcolor}>> Trying to create a new file in inaccessible_directory (with no write permission)...${resetcolor}"
echo ""

./fput_encrypt_rsa inaccessible_directory/non_existent.txt

echo ""
echo ""
echo -e "${redcolor}---POSTPROCESSING---${resetcolor}"
echo ""
echo -e "${yellowcolor}>> Removing build files/${resetcolor}"
sudo rm ./setacl ./getacl ./myfput ./myfget ./fput_encrypt ./fget_decrypt ./fput_encrypt_rsa ./fget_decrypt_rsa ./myls ./mydo_exec ./mycreate_dir
echo ""
echo -e "${yellowcolor}>> Removing temp files${resetcolor}"
sudo rm ./abc.*
echo ""
echo -e "${yellowcolor}>> Removing inaccessible_directory/${resetcolor}"
sudo rm -rf inaccessible_directory