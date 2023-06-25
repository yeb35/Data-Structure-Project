#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

using namespace std;

#define CHAR_SIZE 128

struct WordItem
{
	string Word="";
	int Count = 0;
};

class Word
{
public:
	bool isWord;
	int count;
	Word* character[CHAR_SIZE];
	void insertWord(string);
	bool searchWord(string);

	
	Word() {
		count = 0;
		isWord = false;
		for (int i = 0; i < CHAR_SIZE; i++)
			this->character[i] = nullptr;
	}

};

void Word::insertWord(string word)
{
	Word* currentNode = this;
	for (int i = 0; i < word.length(); i++)
	{
		if (currentNode->character[word[i]] == nullptr)
			currentNode->character[word[i]] = new Word();

		currentNode = currentNode->character[word[i]];
	}

	currentNode->count++;
	currentNode->isWord = true;

}

bool Word::searchWord(string word)
{
	Word* currentNode = this;
	if (currentNode == nullptr)
		return false;

	for (int i = 0; i < word.length(); i++)
	{
		currentNode = currentNode->character[word[i]];
		if (currentNode == nullptr)
			return false;
	}
	return currentNode->isWord;
}





class ReutersWordCounter {
private:
	string bodyTag = "<BODY>"; //start tag to search in article
	string bodyCloseTag = "</BODY>"; //end tag to search in article
	string exportFolder = "Reuters-21578/bodyTexts";

	WordItem countedArray[11];
	Word* Stopwords = new Word();
	Word* CleanWords = new Word();

	time_t timer;
	double firsttime, lasttime;


	string pad_right(string const& str, size_t s, char fillWith)
	{
		if (str.size() < s)
			return str + string(s - str.size(), fillWith);
		else
			return str;
	}

	string pad_left(std::string const& str, size_t s, char fillWith)
	{
		if (str.size() < s)
			return std::string(s - str.size(), fillWith) + str;
		else
			return str;
	}


	void CreateReutersFileList() {
		for (int i = 0; i <= processFileCount - 1; i++) {
			reutersFiles[i] = "Reuters-21578/reut2-" + pad_left(to_string(i), 3, '0') + ".sgm";
		}
	}

	void LoadStopwords() {
		if (stopWordsFileName.empty())
			return;

		fstream stopWordsFile;
		stopWordsFile.open(stopWordsFileName);
		while (!stopWordsFile.eof()) {
			string stopword = "";
			getline(stopWordsFile, stopword);
			
			Stopwords->insertWord(stopword);
		}
		stopWordsFile.close();
	}

	
	void ProcessFile(string fileName) {
		string fileText = GetFileText(fileName);
		size_t currentPos = 0;
		bool eofBody = true;
		while (eofBody) {
			size_t bodyPos = fileText.find(bodyTag, currentPos);
			if (bodyPos == string::npos) {
				eofBody = false;
			}
			else {
				size_t bodyClosePos = fileText.find(bodyCloseTag, bodyPos);
				currentPos = bodyClosePos;
				string bodyText = fileText.substr(bodyPos + bodyTag.length(), bodyClosePos - bodyPos - bodyTag.length());
				ProcessText(bodyText);
			}
		}
	}

	
	string GetFileText(string fileName) {
		fstream file;
		file.open(fileName);
		if (file.fail())
			return "";
		string fileText = "";
		while (!file.eof()) {
			string tempText;
			getline(file, tempText);
			fileText += " " + tempText;
		}
		file.close();
		return fileText;
	}


	
	void ProcessText(string str) {
		string blankStr = "*";
		string keyWord = "";
		int numLower = 32;
		for (int i = 0; i < str.length(); i++) {
			string newLetter = "";
			if (str[i] >= 'A' && str[i] <= 'Z') {
				str[i] = str[i] + numLower;
				newLetter = str[i];
			}
			else if (str[i] >= 'a' && str[i] <= 'z') {
				newLetter = str[i];
			}
			else if (str[i] == (char)39) {
				newLetter = str[i];
			}
			else {
				newLetter = blankStr;
			}

			if (newLetter != blankStr)
				keyWord += newLetter;
			else {
				if (keyWord != "") {
					AddToCleanWords(keyWord);
				}
				keyWord = "";
			}
		}
	}

	
	void AddToCleanWords(string keyWord) {
		if (!IsStopWord(keyWord)) {
			CleanWords->insertWord(keyWord);
		}
	}

	bool IsStopWord(string w) {
		return Stopwords->searchWord(w);
	}

	
	void AddToCountArray(int i, WordItem word) {
		countedArray[10] = countedArray[i];
		countedArray[i] = word;
		SortCountedArray();

	}

	void SortCountedArray() {
		bool sorted = true;
		int i;
		while (sorted) {
			sorted = false;
			for (i = 0; i < 10; i++) {
				if (countedArray[i].Count < countedArray[i + 1].Count) {
					WordItem tempItem = countedArray[i + 1];
					countedArray[i + 1] = countedArray[i];
					countedArray[i] = tempItem;
					sorted = true;
				}
			}
		}
	}

	void CreateSortedList(Word* node, char str[], int step)
	{
		if (node->isWord)
		{
			str[step] = '\0';

			for (int i = 9; i >= 0; i--) {
				if (countedArray[i].Count < node->count) {
					WordItem w = { str,node->count };
					AddToCountArray(i, w);
					break;
				}
			}
		}

		for (int i = 0; i < CHAR_SIZE; i++)
		{
			if (node->character[i])
			{
				str[step] = i;
				CreateSortedList(node->character[i], str, step + 1);
			}
		}
	}



public:
	int processFileCount;
	string stopWordsFileName;
	string* reutersFiles;
	char printWordCountSeperator = ' ';

	
	ReutersWordCounter() {
		this->firsttime = time(&timer);
		this->processFileCount = 0;
		this->stopWordsFileName = "";

	}

	void Initialize() {
		this->reutersFiles = new (nothrow) string[processFileCount];

		this->CreateReutersFileList();

		this->LoadStopwords();
	}

	
	void ProcessFiles() {
		for (int i = 0; i <= processFileCount - 1; i++) {
			string currentFile = reutersFiles[i];
			if (!currentFile.empty()) {
				this->ProcessFile(currentFile);
			}
		}
	}


	void PrintResult() {
		char str[CHAR_SIZE];
		CreateSortedList(CleanWords, str, 0);
		lasttime = time(&timer);
		for (int i = 0; i < 10; i++) {
			cout << pad_right(countedArray[i].Word, 20, this->printWordCountSeperator) << countedArray[i].Count << "\n";
		}
		cout << "Total Elapsed Time: " << lasttime - firsttime << " seconds" << endl;
	}
};

int main()
{
	ReutersWordCounter reutersHelper;
	reutersHelper.printWordCountSeperator = ' ';
	reutersHelper.processFileCount = 22;
	reutersHelper.stopWordsFileName = "Reuters-21578/stopwords.txt";

	reutersHelper.Initialize();
	reutersHelper.ProcessFiles();
	reutersHelper.PrintResult();
}