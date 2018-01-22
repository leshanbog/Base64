#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>


typedef unsigned char BYTE;
typedef unsigned int ui;

using namespace std;

const BYTE alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const BYTE paddingSymbol = '=';
const int encodeGroupLength = 3;
const int decodeGroupLength = 4;


enum Regime
{
	Encoding, Decoding
};


// поиск индекса значения в алфавите
int FindIndex(BYTE &c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	else if (c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	else if (c >= '0' && c <= '9')
		return c - '0' + 52;
	else if (c == '+')
		return 62;
	else if (c == '/')
		return 63;
	else  // других символов нет
		throw std::runtime_error("Incorrect symbol\n");
}


vector <BYTE> Encode(vector<BYTE> &inp)
{
	ui i = 0, j = 0, limit = (inp.size() / encodeGroupLength) * encodeGroupLength;  // i - счетчик в str, j - в ans (ans это результат)
	vector <BYTE> ans(decodeGroupLength * ((inp.size() + 2) / encodeGroupLength));


	for (; i < limit; i += encodeGroupLength)  // цикл до числа, кратного трем, меньшего str.size(), ближайшего к str.size()
	{
		int k1 = inp[i] >> 2;
		int k2 = ((inp[i] & 0b0000011) << 4) | (inp[i + 1] >> 4);
		int k3 = ((inp[i + 1] & 0b00001111) << 2) | (inp[i + 2] >> 6);
		int k4 = inp[i + 2] & 0b00111111;
		ans[j++] = alphabet[k1];
		ans[j++] = alphabet[k2];
		ans[j++] = alphabet[k3];
		ans[j++] = alphabet[k4];
	}

	if (inp.size() - i == 1) // не хватает двух символов
	{
		int k1 = inp[i] >> 2;
		int k2 = ((inp[i] & 0b0000011) << 4);
		ans[j++] = alphabet[k1];
		ans[j++] = alphabet[k2];
		ans[j++] = paddingSymbol;
		ans[j++] = paddingSymbol;
	}

	if (inp.size() - i == 2) // не хватает одного символа
	{
		int k1 = inp[i] >> 2;
		int k2 = ((inp[i] & 0b0000011) << 4) | (inp[i + 1] >> 4);
		int k3 = ((inp[i + 1] & 0b00001111) << 2);
		ans[j++] = alphabet[k1];
		ans[j++] = alphabet[k2];
		ans[j++] = alphabet[k3];
		ans[j++] = paddingSymbol;
	}

	return ans;
}

vector <BYTE> Decode(vector <BYTE> &inp)
{
	if (inp.size() % decodeGroupLength != 0)
		throw std::runtime_error("Incorrect number of symbols\n");

	vector <BYTE> ans((inp.size() / decodeGroupLength) * encodeGroupLength);

	int pads = 0;
	if (inp.size() > 0 && inp[inp.size() - 1] == paddingSymbol)
		pads++;
	if (inp.size() > 0 && inp[inp.size() - 2] == paddingSymbol)
		pads++;

	ui limit = inp.size() - decodeGroupLength * (pads != 0);

	ui j = 0, i = 0;  // i - счетчик в закодированном, j - в ans

	for (; i < limit; i += decodeGroupLength)
	{
		BYTE
			s1 = 0,
			s2 = 0,
			s3 = 0,
			i1 = FindIndex(inp[i]),
			i2 = FindIndex(inp[i + 1]),
			i3 = FindIndex(inp[i + 2]),
			i4 = FindIndex(inp[i + 3]);

		s1 = (i1 << 2) | (i2 >> 4);
		s2 = (i2 << 4) | (i3 >> 2);
		s3 = (i3 << 6) | i4;

		ans[j++] = s1;
		ans[j++] = s2;
		ans[j++] = s3;
	}

	if (pads > 0)
	{
		BYTE
			s1 = 0,
			s2 = 0,
			s3 = 0,
			i1 = FindIndex(inp[i]),
			i2 = FindIndex(inp[i + 1]),
			i3 = 0, i4 = 0;

		if (pads == 1)
			i3 = FindIndex(inp[i + 2]);

		s1 = (i1 << 2) | (i2 >> 4);
		s2 = (i2 << 4) | (i3 >> 2);
		s3 = (i3 << 6) | i4;

		ans[j++] = s1;
		ans[j++] = s2;
		ans[j++] = s3;
	}

	if (pads == 1)
		ans.resize(ans.size() - 1);
	if (pads == 2)
		ans.resize(ans.size() - 2);

	return ans;
}


void Go(Regime regime)
{
	char dataFileName[100], resFileName[100];

	cout << "Name of data file:\t";
	cin >> dataFileName;
	cout << "Name of result file:\t";
	cin >> resFileName;
	try
	{
		FILE *data, *res;

		data = fopen(dataFileName, "rb");
		res = fopen(resFileName, "wb");
		if (data == NULL)
			throw std::runtime_error("Incorrect name of file\n");

		vector <BYTE> input, output;
		BYTE c;

		while (true)
		{
			if (0 == fread(&c, 1, 1, data))
				break;
			input.push_back(c);
		}

		regime == Encoding ? output = Encode(input) : output = Decode(input);

		for (ui i = 0; i < output.size(); i++)
		{
			fwrite(&output[i], 1, 1, res);
		}

		fclose(data);
		fclose(res);
	}
	catch (const runtime_error &re)
	{
		cout << re.what() << endl;
	}
}

int main()
{
	char command;

	do
	{
		cout << "\n1 - Encode from data file to result file\n2 - Decode from data file to result file\n3 - Exit\nCommand = ";
		cin >> command;

		switch (command)
		{
		case '1':
			Go(Encoding);
			break;
		case '2':
			Go(Decoding);
			break;
		case '3':
			return 0;
			break;
		default:
			cout << "Incorrect command\nTry again\n";
			cin.clear();
			while (cin.get() != '\n');
			break;
		}

	} while (true);

	return 0;
}