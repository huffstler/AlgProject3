Patrick
G
A
F
P
Type
C++
Size
24 KB (24,971 bytes)
Storage used
0 bytesOwned by someone else
Location
CISC320 PROJ 3
Owner
Gary Sidoti
Modified
12:10 am by Gary Sidoti
Created
12:10 am
Add a description
Viewers can download

#define WIN32_LEAN_AND_MEAN
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>
#include <string>
#include <array>
#include <sstream>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <math.h>

using namespace std;

//ideone link:  http://ideone.com/hL9FUR

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BEGIN GRAPH CODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


class Node
{
public:
	string name;
	double *tfidf;
	int *docWords;//array of amount of documents, array at 0 will contain how many times the word appears in document 1

	Node(string id, int numDocs, int doc)
	{
		name = id;
		tfidf = new double[numDocs];
		docWords = new int[numDocs];
		fill_n(docWords, numDocs, 0);
		docWords[doc] = 1;
	}

	void incWordAmt(int docNum)
	{
		this->docWords[docNum]++;
	}

};
int amtDocs = 0;
//An object of class graph holds a directed graph
class Graph
{
public:
	vector<Node*> nodeList;//list of verticies

	void addNewNode(Node *nNode)
	{
		nodeList.push_back(nNode);
	}


	Node* findNodeByName(string name)
	{
		for (int i = 0; i < nodeList.size(); i++)
		{
			if (nodeList[i]->name == name)
				return nodeList[i];
		}
		return NULL;
	}

	~Graph()
	{
		//free mem allocated to verticies
		for (int i = 0; i < nodeList.size(); i++)
			delete nodeList[i];
		nodeList.clear();
	}


	void displayGraph(){
		for (int i = 0; i < nodeList.size(); i++){
			cout << nodeList[i]->name << ": "<<endl;
			for (int j = 0; j < amtDocs; j++){
				cout << "(Doc#" << j;
				cout << " TFIDF:"<<nodeList[i]->tfidf[j];
				cout << " Appears:" << nodeList[i]->docWords[j];
				cout << ")";
				cout << endl;
			}
			cout << endl;
		}
	}
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~END GRAPH CODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BEGIN STEM CODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void stem(string& word);
void trim(string& word);
size_t firstNonVowelAfterVowel(const string& word, size_t start);
size_t getStartR1(const string& word);
size_t getStartR2(const string& word, size_t startR1);
void changeY(string& word);
void step0(string& word);
bool step1A(string& word);
void step1B(string& word, size_t startR1);
void step1C(string& word);
void step2(string& word, size_t startR1);
void step3(string& word, size_t startR1, size_t startR2);
void step4(string& word, size_t startR2);
void step5(string& word, size_t startR1, size_t startR2);
inline bool isShort(const string& word);
bool isVowel(char c);
bool isVowelY(char c);
bool endsWith(const string& word, const string& str);
bool endsInDouble(const string& word);
bool replaceIfExists(string& word, const string& suffix,
	const string& replacement, size_t start);
bool ValidEndsWithLI(char c);
bool containsVowel(const string& word, size_t start, size_t end);



void stem(string& word) {
	// doesn't care about words smaller than 3 characters
	if (word.size() <= 2)
		return;

	if (word[0] == '\'')
		word = word.substr(1, word.size() - 1);

	changeY(word);
	size_t startR1 = getStartR1(word);
	size_t startR2 = getStartR2(word, startR1);

	step0(word);

	if (step1A(word)) {
		replace(word.begin(), word.end(), 'Y', 'y');
		return;
	}

	step1B(word, startR1);
	step1C(word);
	step2(word, startR1);
	step3(word, startR1, startR2);
	step4(word, startR2);
	step5(word, startR1, startR2);

	replace(word.begin(), word.end(), 'Y', 'y');
	return;
}

// Trims the word given to it. Trimming entails turning it to all lowercase, and removing non-alphanumeric characters
void trim(string& word) {
	transform(word.begin(), word.end(), word.begin(), ::tolower);
	remove_if(word.begin(), word.end(), [](char c){
		return !((c >= 'a' && c <= 'z') || c == '\'');
	});
}

// Checks for special cases of words ["gener", "commun", "arsen"].
size_t getStartR1(const string& word) {
	// defined special cases
	if (word.size() >= 5 && word[0] == 'g' && word[1] == 'e' && word[2] == 'n'
		&& word[3] == 'e' && word[4] == 'r')
		return 5;
	if (word.size() >= 6 && word[0] == 'c' && word[1] == 'o' && word[2] == 'm'
		&& word[3] == 'm' && word[4] == 'u' && word[5] == 'n')
		return 6;
	if (word.size() >= 5 && word[0] == 'a' && word[1] == 'r' && word[2] == 's'
		&& word[3] == 'e' && word[4] == 'n')
		return 5;
	// If no special cases found, use standard function
	return firstNonVowelAfterVowel(word, 1);
}

// checks size of word and returns it if the same
size_t getStartR2(const string& word, size_t startR1) {
	if (startR1 == word.size())
		return startR1;

	return firstNonVowelAfterVowel(word, startR1 + 1);
}

// return the first consonant character that appears after a vowel, beginning from start
size_t firstNonVowelAfterVowel(const string& word, size_t start) {
	for (size_t i = start; i != 0 && i < word.size(); ++i) {
		if (!isVowelY(word[i]) && isVowelY(word[i - 1]))
			return i + 1;
	}
	return word.size();
}

// changes case of the char y depending on what appears before it, and when it appears in the string
void changeY(string& word) {
	if (word[0] == 'y')
		word[0] = 'Y';

	for (size_t i = 1; i < word.size(); ++i) {
		if (word[i] == 'y' && isVowel(word[i - 1]))
			word[i++] = 'Y'; // skip next iteration
	}
}

/** 0
* catch words that are possesive || plural
*/
void step0(string& word) {
	replaceIfExists(word, "'s'", "", 0) || replaceIfExists(word, "'s", "", 0) || replaceIfExists(word, "'", "", 0);
}

/** 1A
* Looks for past tense words. Checks for edge cases as well
* Focus on suffix of: "Cs", "sses", "s", "ieC" (Where C is a variable character)
*/
bool step1A(string& word) {
	if (!replaceIfExists(word, "sses", "ss", 0)) {
		if (endsWith(word, "ied") || endsWith(word, "ies")) {
			// if preceded by only one letter
			if (word.size() <= 4)
				word.pop_back();
			else {
				word.pop_back();
				word.pop_back();
			}
		}
		else if (endsWith(word, "s") && !endsWith(word, "us") && !endsWith(word, "ss")) {
			if (word.size() > 2 && containsVowel(word, 0, word.size() - 2))
				word.pop_back();
		}
	}

	// special case after step 1a
	return word == "inning" || word == "outing" || word == "canning"
		|| word == "herring" || word == "earring" || word == "proceed"
		|| word == "exceed" || word == "succeed";
}

/** 1B
* Looks for (paste-tense)adverbs.
* Focus on suffix of: "eC", "ingCC","eed","edly" (Where C is a variable character)
*/
void step1B(string& word, size_t startR1) {
	bool exists = endsWith(word, "eedly") || endsWith(word, "eed");

	if (exists) // look only in startR1 now
		replaceIfExists(word, "eedly", "ee", startR1) || replaceIfExists(word, "eed", "ee", startR1);
	else {
		size_t size = word.size();
		bool deleted = (containsVowel(word, 0, size - 2) && replaceIfExists(word, "ed", "", 0))
			|| (containsVowel(word, 0, size - 4) && replaceIfExists(word, "edly", "", 0))
			|| (containsVowel(word, 0, size - 3) && replaceIfExists(word, "ing", "", 0))
			|| (containsVowel(word, 0, size - 5) && replaceIfExists(word, "ingly", "", 0));

		if (deleted && (endsWith(word, "at") || endsWith(word, "bl") || endsWith(word, "iz")))
			word.push_back('e');
		else if (deleted && endsInDouble(word))
			word.pop_back();
		else if (deleted && startR1 == word.size() && isShort(word))
			word.push_back('e');
	}
}

/** 1C
* Looks for words ending in y
* Swaps 'y' with 'i'
*/
void step1C(string& word) {
	size_t size = word.size();
	if (size > 2 && (word[size - 1] == 'y' || word[size - 1] == 'Y'))
		if (!isVowel(word[size - 2]))
			word[size - 1] = 'i';
}

/** 2
* No focus on anything specific.
* Look at the list of suffixes compared. to find out more
*/
void step2(string& word, size_t startR1) {

	static const vector<pair<string, string>> subs
		= { { "ational", "ate" },
		{ "tional", "tion" },
		{ "enci", "ence" },
		{ "anci", "ance" },
		{ "abli", "able" },
		{ "entli", "ent" },
		{ "izer", "ize" },
		{ "ization", "ize" },
		{ "ation", "ate" },
		{ "ator", "ate" },
		{ "alism", "al" },
		{ "aliti", "al" },
		{ "alli", "al" },
		{ "fulness", "ful" },
		{ "ousli", "ous" },
		{ "ousness", "ous" },
		{ "iveness", "ive" },
		{ "iviti", "ive" },
		{ "biliti", "ble" },
		{ "bli", "ble" },
		{ "fulli", "ful" },
		{ "lessli", "less" } };

	for (auto& sub : subs)
		if (replaceIfExists(word, sub.first, sub.second, startR1))
			return;

	if (!replaceIfExists(word, "logi", "log", startR1 - 1)) {
		// make sure we choose the longest suffix
		if (endsWith(word, "li") && !endsWith(word, "abli")
			&& !endsWith(word, "entli") && !endsWith(word, "aliti")
			&& !endsWith(word, "alli") && !endsWith(word, "ousli")
			&& !endsWith(word, "bli") && !endsWith(word, "fulli")
			&& !endsWith(word, "lessli"))
			if (word.size() > 3 && word.size() - 2 >= startR1 && ValidEndsWithLI(word[word.size() - 3])) {
				word.pop_back();
				word.pop_back();
			}
	}
}

/** 3
* Same as Step2
* No specific focus on certain suffixes
*/
void step3(string& word, size_t startR1, size_t startR2) {
	static const vector<pair<string, string>> subs
		= { { "ational", "ate" },
		{ "tional", "tion" },
		{ "alize", "al" },
		{ "icate", "ic" },
		{ "iciti", "ic" },
		{ "ical", "ic" },
		{ "ful", "" },
		{ "ness", "" } };

	for (auto& sub : subs)
		if (replaceIfExists(word, sub.first, sub.second, startR1))
			return;

	replaceIfExists(word, "ative", "", startR2);
}

/** 4
* Same as step 3 for the most part.
* one deviation is that this makes sure that the word with the longest suffix is used,
* instead of using a shorter suffix, which would be wrong is certain cases
*/
void step4(string& word, size_t startR2) {
	static const vector<pair<string, string>> subs
		= { { "al", "" },
		{ "ance", "" },
		{ "ence", "" },
		{ "er", "" },
		{ "ic", "" },
		{ "able", "" },
		{ "ible", "" },
		{ "ant", "" },
		{ "ement", "" },
		{ "ment", "" },
		{ "ism", "" },
		{ "ate", "" },
		{ "iti", "" },
		{ "ous", "" },
		{ "ive", "" },
		{ "ize", "" } };

	for (auto& sub : subs)
		if (replaceIfExists(word, sub.first, sub.second, startR2))
			return;

	// ensures word with longest tail is used
	if (!endsWith(word, "ement") && !endsWith(word, "ment"))
		if (replaceIfExists(word, "ent", "", startR2))
			return;

	// shortcut
	replaceIfExists(word, "sion", "s", startR2 - 1) || replaceIfExists(word, "tion", "t", startR2 - 1);
}

/** 5
* Checks the end of word for the char 'e'
* pops 'e' off if its found
*/
void step5(string& word, size_t startR1, size_t startR2) {
	size_t size = word.size();

	if (word[size - 1] == 'e') {

		if (size - 1 >= startR2)
			word.pop_back();

		else if (size - 1 >= startR1 && !isShort(word.substr(0, size - 1)))
			word.pop_back();

	}
	else if (word[word.size() - 1] == 'l') {

		if (word.size() - 1 >= startR2 && word[word.size() - 2] == 'l')
			word.pop_back();
	}
}

//Checks word for low amount of syllables
bool isShort(const string& word) {
	size_t size = word.size();

	if (size >= 3) {
		if (!isVowelY(word[size - 3]) && isVowelY(word[size - 2])
			&& !isVowelY(word[size - 1]) && word[size - 1] != 'w'
			&& word[size - 1] != 'x' && word[size - 1] != 'Y')
			return true;
	}
	return size == 2 && isVowelY(word[0]) && !isVowelY(word[1]);
}

//checks if the character is the vowel version of y
bool isVowelY(char c) {
	return c == 'e' || c == 'a' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}

//checks if given character is a vowel
bool isVowel(char c) {
	return c == 'e' || c == 'a' || c == 'i' || c == 'o' || c == 'u';
}

//checks if word ends with the given string, str
bool endsWith(const string& word, const string& str) {
	return word.size() >= str.size() && equal(word.begin() + (word.size() - str.size()), word.end(), str.begin());
}

//checks if given word ends with double of the chars listed [b,d,f,g,m,n,p,r,t]
bool endsInDouble(const string& word) {
	if (word.size() >= 2) {
		char a = word[word.size() - 1];
		char b = word[word.size() - 2];

		if (a == b)
			return a == 'b' || a == 'd' || a == 'f' || a == 'g' || a == 'm'
			|| a == 'n' || a == 'p' || a == 'r' || a == 't';
	}
	return false;
}

//replaces word with replacement beginning from start if word exists
bool replaceIfExists(string& word, const string& suffix, const string& replacement, size_t start) {
	size_t idx = word.size() - suffix.size();
	if (idx < start)
		return false;

	if (equal(word.begin() + idx, word.end(), suffix.begin())) {
		word = word.substr(0, word.size() - suffix.size()) + replacement;
		return true;
	}
	return false;
}

//checks some fringe cases. Words ending in 'li'
bool ValidEndsWithLI(char c) {
	return c == 'c' || c == 'd' || c == 'e' || c == 'g' || c == 'h'
		|| c == 'k' || c == 'm' || c == 'n' || c == 'r' || c == 't';
}

//returns true if word contains a vowel. False otherwise
bool containsVowel(const string& word, size_t start, size_t end) {
	if (end <= word.size()) {
		for (size_t i = start; i < end; ++i)
			if (isVowelY(word[i]))
				return true;
	}
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~END STEM CODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

unordered_map<int, string> mymap;
list<string> importedData;
hash<string> strhash;
Graph G;
//list of all stopwords
vector<string> stopWords = { "a", "about", "above", "across", "after", "afterwards", "again", "against", "all", "almost", "alone", "along", "already", "also", "although", "always", "am", "among", "amongst", "amoungst", "amount", "an", "and", "another", "any", "anyhow", "anyone", "anything", "anyway", "anywhere", "are", "around", "as", "at", "back", "be", "became", "because", "become", "becomes", "becoming", "been", "before", "beforehand", "behind", "being", "below", "beside", "besides", "between", "beyond", "bill", "both", "bottom", "but", "by", "call", "can", "cannot", "cant", "co", "computer", "con", "could", "couldnt", "cry", "de", "describe", "detail", "do", "done", "down", "due", "during", "each", "eg", "eight", "either", "eleven", "else", "elsewhere", "empty", "enough", "etc", "even", "ever", "every", "everyone", "everything", "everywhere", "except", "few", "fifteen", "fify", "fill", "find", "fire", "first", "five", "for", "former", "formerly", "forty", "found", "four", "from", "front", "full", "further", "get", "give", "go", "had", "has", "hasnt", "have", "he", "hence", "her", "here", "hereafter", "hereby", "herein", "hereupon", "hers", "him", "his", "how", "however", "hundred", "i", "ie", "if", "in", "inc", "indeed", "interest", "into", "is", "it", "its", "keep", "last", "latter", "latterly", "least", "less", "ltd", "made", "many", "may", "me", "meanwhile", "might", "mill", "mine", "more", "moreover", "most", "mostly", "move", "much", "must", "my", "name", "namely", "neither", "never", "nevertheless", "next", "nine", "no", "nobody", "none", "noone", "nor", "not", "nothing", "now", "nowhere", "of", "off", "often", "on", "once", "one", "only", "onto", "or", "other", "others", "otherwise", "our", "ours", "ourselves", "out", "over", "own", "part", "per", "perhaps", "please", "put", "rather", "re", "same", "see", "seem", "seemed", "seeming", "seems", "serious", "several", "she", "should", "show", "side", "since", "sincere", "six", "sixty", "so", "some", "somehow", "someone", "something", "sometime", "sometimes", "somewhere", "still", "such", "system", "take", "ten", "than", "that", "the", "their", "them", "themselves", "then", "thence", "there", "thereafter", "thereby", "therefore", "therein", "thereupon", "these", "they", "thick", "thin", "third", "this", "those", "though", "three", "through", "throughout", "thru", "thus", "to", "together", "too", "top", "toward", "towards", "twelve", "twenty", "two", "un", "under", "until", "up", "upon", "us", "very", "via", "was", "we", "well", "were", "what", "whatever", "when", "whence", "whenever", "where", "whereafter", "whereas", "whereby", "wherein", "whereupon", "wherever", "whether", "which", "while", "whither", "who", "whoever", "whole", "whom", "whose", "why", "will", "with", "within", "without", "would", "yet", "you", "your", "yours", "yourself", "yourselves" };

void populateMap() { //Populate a hashmap with stopwords
	for (int i = 0; i < stopWords.size(); i++){
		mymap.insert(make_pair(strhash(stopWords[i]), stopWords[i]));
	}
}

bool isStop(string word){//check if the word is a stopword
	if (mymap.find(strhash(word)) != mymap.end()) {
		return true;
	}
	else {
		return false;
	}
}

void throwError(string s){
	cout << "ERROR: " << s << endl;
}
struct docList{ //adjacency list for documents
	int docNumber;
	int queryDist;
	vector<Node*> wordsList;
	int wordsInDoc;

	docList(int n){
		docNumber = n;
	}
};
struct queryWords{ //creates object querywords for the queryList
	string Word;
	double *tfidf;
	queryWords(string s){
		Word = s;
		tfidf = new double[amtDocs];
	}
};

struct queryCheck{//used in checking score
	string Word;
	float score = 0.0;
};

vector<docList*> documentList;
vector<queryWords*> queryList;


float lenScore(double large, double small){
	if (large == small){
		return .25;
	}
	else {
		return ((small / large)*.25);
	}
}

float charScore(string large, string small){
	float p1 = (75 / large.size());
	float pointScale = p1 / 100;
	int window = small.size();
	float score = 0;
	float tempScore;

	if (large.size() == small.size()){ // if both words are the same size, do a regular comparison
		for (int id = 0; id < large.size(); id++){
			if (large[id] == small[id]) {
				score += pointScale;
			}
		}
		return score;
	}
	else { //scoring alg goes here
		for (int idx = 0; idx < large.size(); idx++) { // idx moves the slider one place
			string tmpStr1 = large.substr(idx, window);
			for (int index = 0; index < tmpStr1.size(); index++){
				if (tmpStr1[index] == small[index]){
					tempScore += pointScale;
				}
			}
			if (tempScore > score){
				score = tempScore;
				tempScore = 0;
			}
			else {
				tempScore = 0;
			}
		}
		return score;
	}
}

float calcSimilar(string first, string second){
	double LengthScore;  //makes up 25% of total score
	double CharacterScore; // makes up 75% of total score

	double firstIn = first.size();
	double secondIn = second.size();

	if (firstIn <= 2 || secondIn <= 2){
		//cout << "Don't bother with strings less than 2 characters please." << endl;
		return -1.00;
	}
	else if (firstIn < secondIn){
		LengthScore = lenScore(secondIn, firstIn);
		CharacterScore = charScore(second, first);
	//	cout << "Second word larger than first... LScore: " << LengthScore << " CScore: " << CharacterScore << endl;
		return LengthScore + CharacterScore;
	}
	else if (firstIn > secondIn){
		LengthScore = lenScore(firstIn, secondIn);
		CharacterScore = charScore(first, second);
	//	cout << "First word larger than second... LScore: " << LengthScore << " CScore: " << CharacterScore << endl;
		return LengthScore + CharacterScore;
	}
	else {
		LengthScore = lenScore(firstIn, secondIn);
		CharacterScore = charScore(first, second);
	//	cout << "Both words are the same size... LScore: " << LengthScore << " CScore: " << CharacterScore << endl;
		return LengthScore + CharacterScore;
	}
}
//accept a query at the top of the input file
//lowercases and stems to make searching easier
void inputQuery(string tempStringP){
	string s = tempStringP;
	unsigned int len = s.size();
	for (int i = 0; i < len; i++) {//lowercase everything
		s[i] = tolower(s[i]);
		if (isdigit(s[i]) || ispunct(s[i])) {
			s.erase(i--, 1);
			len = s.size();
		}
	}
	stringstream iss(s);
	cout << "Your Query is: " << tempStringP << endl;
	while (getline(iss, s, ' ')){ //get string up to a space
		if (!isStop(s) && s != "") {
			stem(s);
			queryWords *c = new queryWords(s);	
			queryList.push_back(c);
		}
	}
}
//check if the words in the query exist
//if they dont exist find three words that are similar to it
void checkQuery(){
	queryCheck highestValue[3];
	float temp;
	for (int i = 0; i < queryList.size(); i++){
		if (!G.findNodeByName(queryList[i]->Word)){
			cout << "Changed bad word: " << queryList[i]->Word;
			for (int x = 0; x < G.nodeList.size(); x++){
				if ((temp = calcSimilar(G.nodeList[x]->name, queryList[i]->Word)) > highestValue[0].score){
					highestValue[2] = highestValue[1];
					highestValue[1] = highestValue[0];
					highestValue[0].score = temp;
					highestValue[0].Word = G.nodeList[x]->name;
				}
				else if ((temp = calcSimilar(G.nodeList[x]->name, queryList[i]->Word)) > highestValue[1].score){
					highestValue[2] = highestValue[1];
					highestValue[1].score = temp;
					highestValue[1].Word = G.nodeList[x]->name;
				}
				else if ((temp = calcSimilar(G.nodeList[x]->name, queryList[i]->Word)) > highestValue[2].score){
					highestValue[2].score = temp;
					highestValue[2].Word = G.nodeList[x]->name;
				}
			}
			cout << " -> " << highestValue[0].Word << ", " << highestValue[1].Word << ", " << highestValue[2].Word << endl;
			queryList[i]->Word = highestValue[0].Word;
			queryWords *o = new queryWords(highestValue[1].Word);
			queryList.push_back(o);
			queryWords *p = new queryWords(highestValue[1].Word);
			queryList.push_back(p);
		}
	}
}



bool getFile(){//inports lines into ideone
	string tempStringP;
	int docNum = 0;
	int amtInDoc = 0;
	Node* temp;

	getline(cin, tempStringP);
	amtDocs = atoi(tempStringP.c_str());
	if (amtDocs == 0){
		throwError("Input the amount of Documents");
		return false;
	}
	cout << "You said you imported " << amtDocs << " documents" << endl;
	getline(cin, tempStringP);
	if (tempStringP != "-"){
		throwError("Seperate inputs with a -");
		return false;
	}
	getline(cin, tempStringP);
	inputQuery(tempStringP);
	getline(cin, tempStringP);
	docList *d = new docList(docNum);
	documentList.push_back(d);
	
	while (getline(cin, tempStringP)){ //reads in a line from ideone
		if (tempStringP == "-"){
			//cout << "getting to first -" << endl;
			documentList[docNum]->wordsInDoc = amtInDoc;
			docNum++;
			docList *x = new docList(docNum);
			documentList.push_back(x);
		}
		unsigned int len = tempStringP.size();
		for (int i = 0; i < len; i++) {//lowercase everything
			tempStringP[i] = tolower(tempStringP[i]);
			if (isdigit(tempStringP[i]) || ispunct(tempStringP[i])) {
				tempStringP.erase(i--, 1);
				len = tempStringP.size();
			}
		}
		stringstream iss(tempStringP);
		//cout << "splitting at spaces" << endl;
		while (getline(iss, tempStringP, ' ')){ //get string up to a space
			//push string with removed punctuation onto the imported data list
			if (!isStop(tempStringP) && tempStringP != "") {
				stem(tempStringP);
				if (temp = G.findNodeByName(tempStringP)){
					temp->incWordAmt(docNum);
					amtInDoc++;
				}
				else{
					Node *newWord = new Node(tempStringP, amtDocs, docNum);
					G.addNewNode(newWord);
					documentList[docNum]->wordsList.push_back(newWord);
					amtInDoc++;
				}
			}
		}
	}
	checkQuery();
	if (docNum != amtDocs){
		throwError("Incorrect number of documents entered");
		return false;
	}
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BEGIN TF-IDF~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


double tf(string s, int docNum){//calculates the tf
	double d = G.findNodeByName(s)->docWords[docNum];
	double c = documentList[docNum]->wordsInDoc;
	return d / c;
}

double idf(string s){//caclulates idf
	double wordAppearance = 0;
	double d = amtDocs;
	for (int i = 0; i < amtDocs; i++){
		if (G.findNodeByName(s)->docWords[i] != 0){
			wordAppearance++;
		}
	}
	return log2(d / wordAppearance);
}

void calcTFIDF(){//fills in tfidf value for every word in graph
	for (int i = 0; i < G.nodeList.size(); i++){
		for (int j = 0; j < amtDocs; j++){
			G.nodeList[i]->tfidf[j] = (idf(G.nodeList[i]->name)  *  tf(G.nodeList[i]->name, j));
		}
	}
	for (int i = 0; i < queryList.size(); i++){
		for (int j = 0; j < amtDocs; j++){
			double c = tf(queryList[i]->Word, queryList[i]->tfidf[j]);
			double d = idf(queryList[i]->Word);
			queryList[i]->tfidf[j] = c*d;
		}
	}
}

void search(){
	double d = 0;
	for (int i = 0; i < documentList.size()-1; i++){
		for (int j = 0; j < documentList[i]->wordsList.size(); j++){
			for (int m = 0; m < queryList.size(); m++){
				d += pow((queryList[m]->tfidf[i] - documentList[i]->wordsList[j]->tfidf[i]), 2);
			}
		}
		cout << "Query Distance: " << sqrt(d) << " for document " << i << endl;
		documentList[i]->queryDist = sqrt(d);
	}
}


int main(){
	populateMap();
	if (getFile()){
		calcTFIDF();
		search();
		G.displayGraph();
	}
	return 0;
}