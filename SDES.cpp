#include <iostream>
#include <string>
#include <ctime>
#include <stdio.h>
using namespace std;

string ECB(string, string, int);
string ECBDecryption(string, string, int);
string SDESEncryption(string, string, int);
string SDESDecryption(string, string, int);
string findKey(string, int);
string functionF(string, string);
string XOR(string, string);
string S1Box(string);
string S2Box(string);
int bitsDifferent(string, string);

int main()
{
	string ciphertext;
	string ciphertext2;
	string plaintext2;
	string decryption;
	//Initialize plaintext
	string plaintext = "100010110101";

	plaintext2 = plaintext;

	//Initialize 9 bit key
	string key = "111000111";


	//Print out original plaintext 
	cout << "Original Plaintext:\t" << plaintext << endl
		<< "\nKey:\t\t\t" << key << endl << endl
		<< "Encryption:" << endl << endl;
	//Encrypt plaintext (4 rounds of SDES)
	ciphertext = ECB(key, plaintext, 4);
	cout << endl << "Ciphertext:\t\t" << ciphertext << endl << endl;

	//Decrypt ciphertext (4 rounds of SDES)
	decryption = ECBDecryption(key, ciphertext, 4);
	cout << "\nPlaintext:\t\t" << decryption << endl << endl;
	if (bitsDifferent(decryption, plaintext) == 0)
		cout << "Original plaintext obtained!" << endl << endl;
	else
		cout << "Something went wrong!" << endl << endl;

	system("pause");
	return 0;
}

////////////////////////////////////////////////////////////////
//Encryption
////////////////////////////////////////////////////////////////
string ECB(string key, string plaintext, int rounds)
{
	string ciphertext1;
	string ciphertext;
	//Divide plaintext
	ciphertext1.append(plaintext, 0, 12);

	//SDES 4x
	for (int i = 1; i <= rounds; i++)
		ciphertext1 = SDESEncryption(key, ciphertext1, i);

	// Return the ciphertext
	return ciphertext1;
}

////////////////////////////////////////////////////////////////
//SDES Encryption Function
////////////////////////////////////////////////////////////////
string SDESEncryption(string key, string plaintext, int round)
{
	//Declare variables
	string Li;
	string Ri;
	string Ln;
	string Rn;
	string K;
	string f;
	//Find the key for the round
	K = findKey(key, round);
	//Split the plaintext into initial Li and Ri
	Li.append(plaintext, 0, 6);
	Ri.append(plaintext, 6, 6);
	//Step 1: Ln = Ri
	Ln = Ri;
	//Step 2: Rn = L0 XOR f(Ri,Kround)
	//Find f(Ri, Kround)
	f.append(functionF(Ri, K));
	//Now find Rn = Li XOR f(Ri, Kround)
	Rn.append(f);
	Rn = XOR(Li, f);
	//Finish the round by concatenating  and returning Ln+Rn;
	cout << "L" << round << " " << Ln << endl;
	cout << "R" << round << " " << Rn << endl;
	return (Ln + Rn);
}


////////////////////////////////////////////////////////////////
//Decryption
////////////////////////////////////////////////////////////////
string ECBDecryption(string key, string ciphertext, int rounds)
{
	cout << "Decryption:" << endl << endl;
	string plaintext1;

    plaintext1.append(ciphertext, 0, 12);

	//SDES 4x
	for (int i = rounds; i > 0; i--)
		plaintext1 = SDESDecryption(key, plaintext1, i);

	// Return the plaintext
	return (plaintext1);
}


////////////////////////////////////////////////////////////////
//SDES Decryption Function
////////////////////////////////////////////////////////////////
string SDESDecryption(string key, string ciphertext, int round)
{
	//Declare variables
	string Li;
	string Ri;
	string Ln;
	string Rn;
	string K;
	string f;
	//Find the key for the round
	K = findKey(key, round);
	//Split the ciphertext into initial Li and Ri
	Li.append(ciphertext, 0, 6);
	Ri.append(ciphertext, 6, 6);
	//Step 1: Rn = Li
	Rn = Li;
	//Step 2: Ln = Ri XOR f(Ln, K)
	//Find f(Ln, K)
	f.append(functionF(Rn, K));
	//Now find Ln = Ri XOR f(Ln, K)
	Ln.append(f);
	Ln = XOR(Ri, f);
	//Finish the round by concatenating and returning Ln + Rn
	cout << "L" << round << " " << Ln << endl;
	cout << "R" << round << " " << Rn << endl;
	return (Ln + Rn);

}

////////////////////////////////////////////////////////////////
//A function to find the key based on what round it is
////////////////////////////////////////////////////////////////
string findKey(string key, int round)
{
	//Get the key for the round
	string K;
	if (round == 1)
		K.append(key, 0, 8);
	else if (round == 2)
		K.append(key, 1, 8);
	else if (round == 3)
	{
		K.append(key, 2, 7);
		K.append(key, 0, 1);
	}
	else if (round == 4)
	{
		K.append(key, 3, 6);
		K.append(key, 0, 2);
	}
	return K;
}

////////////////////////////////////////////////////////////////
//The F Function (f(Ri, Ki+1))
////////////////////////////////////////////////////////////////
string functionF(string R, string K)
{
	char tmp;
	string s1;
	string s2;
	//Expand Ri to find E(Ri)
	R.append(R, 4, 2);
	tmp = R[3];
	R[5] = R[2];
	R[4] = tmp;
	R[3] = R[2];
	R[2] = tmp;
	//XOR E(Ri) with Kround to find S-Box inputs
	R = XOR(R, K);
	s1.append(R, 0, 4);
	s2.append(R, 4, 4);
	//Find f(Ri, Kround) by concatenating S1Box(s1) + S2Box(s2)
	return S1Box(s1) + S2Box(s2);
}

////////////////////////////////////////////////////////////////
//The XOR Function
////////////////////////////////////////////////////////////////
string XOR(string x, string y)
{
	for (unsigned int i = 0; i < x.length(); i++)
	{
		if (x[i] == y[i])
			x[i] = '0';
		else if (x[i] != y[i])
			x[i] = '1';
	}
	return x;
}

////////////////////////////////////////////////////////////////
//S1-Box Function
////////////////////////////////////////////////////////////////
string S1Box(string s1)
{
	//Define S1-Box rows 1 and 2
	string row1[8] = { "101", "010", "001", "110", "011", "100", "111", "000" };
	string row2[8] = { "001", "100", "110", "010", "000", "111", "101", "011" };
	int column = 0;
	//If bits start with a 0, use row 1
	if (s1[0] == '0')
	{
		//Convert remaining 3 bits to decimal
		if (s1[1] == '1')
			column += 4;
		if (s1[2] == '1')
			column += 2;
		if (s1[3] == '1')
			column += 1;
		//Return the 3 bit pattern in the respective column
		return row1[column];
	}
	//If bits start with a 1, use row 2
	else if (s1[0] == '1')
	{
		//Convert remaining 3 bits to decimal
		if (s1[1] == '1')
			column += 4;
		if (s1[2] == '1')
			column += 2;
		if (s1[3] == '1')
			column += 1;
		//Return the 3 bit pattern in the respective column
		return row2[column];
	}
	else
		//Something went wrong (very wrong)
		return "ERROR";
}

////////////////////////////////////////////////////////////////
//S2-Box Function
////////////////////////////////////////////////////////////////
string S2Box(string s2)
{
	//Define S1-Box rows 1 and 2
	string row1[8] = { "100", "000", "110", "101", "111", "001", "011", "010" };
	string row2[8] = { "101", "011", "000", "111", "110", "010", "001", "100" };
	int column = 0;
	//If bits start with a 0, use row 1
	if (s2[0] == '0')
	{
		//Convert the remaining 3 bits to decimal
		if (s2[1] == '1')
			column += 4;
		if (s2[2] == '1')
			column += 2;
		if (s2[3] == '1')
			column += 1;
		//Return the 3 bit pattern in the respective column
		return row1[column];
	}
	//If bits start with a 1, use row 2
	else if (s2[0] == '1')
	{
		//Convert the remaining 3 bits to decimal
		if (s2[1] == '1')
			column += 4;
		if (s2[2] == '1')
			column += 2;
		if (s2[3] == '1')
			column += 1;
		//Return the 3 bit pattern in the respective column
		return row2[column];
	}
	else
		//Something went wrong (very wrong)
		return "ERROR";
}

int bitsDifferent(string s1, string s2)
{
	string t = XOR(s1, s2);
	int ctr = 0;
	for (unsigned int i = 0; i < t.length(); i++)
		if (t[i] == '1')
			ctr++;
	return ctr;
}