// POP 2021-01-14 projekt 2 GRA¯EWICZ GRZEGORZ EIT 3 183208 VISUAL STUDIO 2019
#include <iostream>
#include <string.h>
#include <fstream>
#include <math.h>

//Rozmiar porcji wczytywanych plików; musi byæ wiêkszy ni¿ 1
#define BATCH_SIZE 4

using namespace std;

//Funkcja sprawdzaj¹ca ile znaków zczytano
int howMany(int h)
{
	if (h / BATCH_SIZE >= 1)
	{
		return BATCH_SIZE;
	}
	else
	{
		return h;
	}
}

void cipher(ifstream* input, ofstream* output)
{
	int counter;
	char initVec = 0xAA;
	char batch[BATCH_SIZE], cipheredBatch[BATCH_SIZE];

	//Obliczanie d³ugosci tekstu
	input->seekg(0, input->end);
	int textLength = input->tellg();
	input->seekg(0, input->beg);

	//Obliczanie ilosci przejsc pêtli
	float rounds = textLength / (float)BATCH_SIZE;
	rounds = ceil(rounds);

	cout << "Podaj wartosc poczatkowa licznika" << endl;
	cin >> counter;

	for (int i = 0; i < rounds; i++)
	{
		input->read(batch, BATCH_SIZE);
		for (int j = 0; j < howMany(textLength); j++)
		{
			//Szyfrowanie liter
			cipheredBatch[j] = batch[j] ^ initVec ^ counter;
			initVec = cipheredBatch[j];
			counter = (counter + 1) % 256;
		}
		output->write(cipheredBatch, howMany(textLength));
		textLength -= BATCH_SIZE;
	}
	cout << "Zakodowane" << endl;
}

void decipher(ifstream* input, ofstream* output, fstream* temp)
{
	int counter;
	int decOffset = 0;
	bool decipher = true;
	char batch[BATCH_SIZE], decipherBatch[BATCH_SIZE];
	char initVec;
	//Obliczanie d³ugosci tekstu
	input->seekg(0, input->end);
	int textLength = input->tellg();
	input->seekg(0, input->beg);

	//Podawanie wartoœci pocz¹tkowej licznika
	cout << "Podaj wartosc poczatkowa licznika" << endl;
	cin >> counter;

	counter += textLength;
	counter = counter % 256;
	int toMove = textLength;

	while (decipher == true)
	{
		if (toMove - BATCH_SIZE + decOffset > 0)
		{
			toMove = toMove - BATCH_SIZE + decOffset;
			input->seekg(toMove);
			if (decOffset == 0)
				decOffset = 1;
			input->read(batch, BATCH_SIZE);
			for (int j = BATCH_SIZE - 1; j > 0; j--)
			{
				counter = (counter - 1) % 256;
				initVec = batch[j - 1];
				decipherBatch[j] = batch[j] ^ counter ^ initVec;
			}
			temp->write(decipherBatch+1, BATCH_SIZE - 1);
		}
		else
		{
			input->seekg(0);
			input->read(batch, toMove + decOffset);
			for (int j = toMove - 1 + decOffset; j > 0; j--)
			{
				counter = (counter - 1) % 256;
				initVec = batch[j - 1];
				decipherBatch[j] = batch[j] ^ counter ^ initVec;
			}
			counter = (counter - 1) % 256;
			initVec = 0xAA;
			decipherBatch[0] = batch[0] ^ counter ^ initVec;
			temp->write(decipherBatch, toMove + decOffset);
			decipher = false;
		}
	}
	if (decOffset == 0)
	{
		//Przepisywanie w przypadku gdy tekst ma mnie lub równ¹ iloœæ znaków co BATCH_SIZE
		temp->seekg(0);
		temp->read(batch, textLength);
		output->write(batch, textLength);
		return;
	}
	else
	{
		//Przepisywanie w przypadku gdy tekst ma wiêcej znaków ni¿ BATCH_SIZE
		int len = textLength;
		len = len - toMove - 1;
		temp->seekg(len);
		temp->read(batch, toMove + decOffset);
		output->write(batch, toMove + decOffset);
		int rounds = len / (BATCH_SIZE - 1);
		for (int i = 0; i < rounds; i++)
		{
			len = len - (BATCH_SIZE - 1);
			temp->seekg(len);
			temp->read(batch, (BATCH_SIZE - 1));
			output->write(batch, (BATCH_SIZE - 1));
		}
	}
}

int main()
{
	int option = 0;
	//Otwieranie plików
	ifstream myInputFile("Zrodla/out.txt", ios::in);			//plik input
	ofstream myOutputFile("Zrodla/in.txt", ios::trunc);			//plik output

	//Plik do przechowywania obecnych wyników deszyfracji
	fstream tempFile("Zrodla/temp.txt", ios::in | ios::out | ios::trunc);

	if (!myInputFile.is_open() || !myOutputFile.is_open())
	{
		cout << "Nie udalo siê wczytac pliku" << endl;
		return 0;
	}

	cout << "Podaj operacje" << endl;
	cout << "1. Szyfrowanie" << endl;
	cout << "2. Deszyfrowanie" << endl;
	cin >> option;
	if (option == 1)
	{
		cipher(&myInputFile, &myOutputFile);
	}
	else if (option == 2)
	{
		decipher(&myInputFile, &myOutputFile, &tempFile);
	}
	else
	{
		cout << "Niepoprawna opcja" << endl;
		return 0;
	}

	tempFile.close();
	myOutputFile.close();
	myInputFile.close();
	return 0;
}