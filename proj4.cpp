#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cctype>
#include <vector>
using namespace std;

/*
name: Jeremy Goldman
program: Project 4
purpose: to search through a corpus, find the document most relevant to an inputted query, and to highlight words 
  in the document that were contained in the query.
*/
//returns vector of term frequency in documents
vector<int> GetTFs(string doc, vector<string> words) {
	vector<int> tfs(words.size());
	istringstream inSS(doc);
	while (inSS.good()) {
		string currentWord = "";
		inSS >> currentWord;
		for (int i = 0; i<words.size(); i++) {
			if (currentWord == words.at(i)) {
				tfs.at(i)++;
			}
		}
	}
	return tfs;
}
//returns lowercase form of a string
string MakeLower(string phrase) {
	string lowerPhrase;
	istringstream inSS(phrase);
	while (inSS.good()) {
		string tempWord = "";
		inSS >> tempWord;
		for (int i = 0; i<tempWord.size(); i++) {
			tempWord[i] = tolower(tempWord[i]);
		}
		lowerPhrase += tempWord;
		lowerPhrase += " ";
	}
	lowerPhrase.resize(lowerPhrase.size()-1);
	return lowerPhrase;
}
//returns IDF given df and number of documents in corpus
double GetIDF(int numDocs, int df) {
	double quotient = static_cast<double>(numDocs) / (1+static_cast<double>(df));
	double idf = log10(quotient);
	return idf;
}
//creates tf-idf vector for document
vector<double> CreateTF_IDF(string doc, vector<string> vocabWords, vector<int> dfs, int numDocs) {
	vector<double> tf_idf;
	vector<int> tfs(vocabWords.size());
	tfs = GetTFs(doc, vocabWords);
	double df;
	double idf;
	for (int i = 0; i<vocabWords.size(); i++) {
		df = dfs.at(i);
		idf = GetIDF(numDocs, df);
		tf_idf.push_back((static_cast<double>(tfs.at(i)))*idf);
	}
	return tf_idf;
}
//compares cosine similarities between 2 vectors
double CompareCosineSimilarity(vector<double> tf_idf1, vector<double> tf_idf2) {
	double numerator = 0;
	double product = 0;
	for (int i = 0; i<tf_idf1.size(); i++) {
		numerator += tf_idf1.at(i) * tf_idf2.at(i);
	}

	double firstMagnitude = 0;
	double inside = 0;
	for (int i = 0; i<tf_idf1.size(); i++) {
		inside += pow(tf_idf1.at(i), 2);
	}
	firstMagnitude = sqrt(inside);

	double secondMagnitude = 0;
	double inside2 = 0;
	for (int i = 0; i<tf_idf2.size(); i++) {
		inside2 += pow(tf_idf2.at(i), 2);
	}
	secondMagnitude = sqrt(inside2);

	double denominator = firstMagnitude * secondMagnitude;
	double answer = numerator/denominator;
	return answer;
}



int main() {
	string vocabName = "";
	string corpusName = "";
	string query = "";
	cin >> vocabName;
	cin >> corpusName;
	cin.ignore();
	getline(cin, query);
	string queryLower = MakeLower(query);

	ifstream vocabFS;
	ifstream corpusFS;
	vocabFS.open(vocabName.c_str());
	corpusFS.open(corpusName.c_str());

	vector<string> vocabWords;
	while (vocabFS.good()) {
		string currentVocabWord = "";
		vocabFS >> currentVocabWord;
		vocabWords.push_back(MakeLower(currentVocabWord));
	}
	vocabFS.close();
	vocabWords.resize(vocabWords.size()-1);

	//find df's and number of documents in corpus
	int numDocuments = 0;
	string currentDocument = "";
	vector<int> dfs(vocabWords.size());
	while (corpusFS.good()) {
		getline(corpusFS, currentDocument);
		currentDocument = MakeLower(currentDocument);
		for (int i = 0; i<vocabWords.size(); i++) {
			if (currentDocument.find(vocabWords.at(i)) != std::string::npos) {
				dfs.at(i)++;
			}
		}
		numDocuments++;
	}
	//create tf-idf vector for query
	vector<double> queryTF_IDF;
	queryTF_IDF = CreateTF_IDF(query, vocabWords, dfs, numDocuments);


	corpusFS.clear();
	corpusFS.seekg(0, ios::beg);
	
	vector<double> comparisons;
	//find tf's within every document, construct tf-idf vector for each document, compare with query tf-idf vector, creates 
	//  vector of cosine similarities between query and each document.
	while (corpusFS.good()) {
		string currentDoc = "";
		getline(corpusFS, currentDoc);
		currentDoc = MakeLower(currentDoc);
		vector<double> currentDocTF_IDF = CreateTF_IDF(currentDoc, vocabWords, dfs, numDocuments);
		comparisons.push_back(CompareCosineSimilarity(queryTF_IDF, currentDocTF_IDF));
	}
	//find index of most similar document
	double maxComparison = comparisons.at(0);
	int indexOfMax = 0;
	for (int i = 0; i<comparisons.size(); i++) {
		if (comparisons.at(i) > maxComparison) {
			maxComparison = comparisons.at(i);
			indexOfMax = i;
		}
	}

	corpusFS.clear() ;
	corpusFS.seekg(0, ios::beg);

	//find most relevant document from index in corpus
	string docMatch = "";
	int i = 0;
	while (i<indexOfMax) {
		string currentDocument = "";
		getline(corpusFS, currentDocument);
		i++;
	}
	getline(corpusFS, docMatch);
	istringstream matchISS(docMatch);
	istringstream queryISS(query);
	ostringstream outputOSS;
	vector<string> queryWords;
	while (queryISS.good()) {
		string currentQueryWord = "";
		queryISS >> currentQueryWord;
		queryWords.push_back(MakeLower(currentQueryWord));
	}
	corpusFS.close();


	//highlights words from query in most relevant document, puts into ostringstream
	bool wordHighlighted = false;
	string currentWord = "";
	int j = 0;
	while (matchISS.good()) {
		matchISS >> currentWord;
		string lowerCurrentWord = MakeLower(currentWord);
		wordHighlighted = false;
		while (!wordHighlighted && j<queryWords.size()) {
			if (lowerCurrentWord == queryWords.at(j)) {
				outputOSS << "*" << currentWord << "* ";
				wordHighlighted = true;
			}
			j++;
		}
		if (!wordHighlighted) {
			outputOSS << currentWord << " ";
		}
		j=0;
	}

	bool noMatches = true;
	for (int i = 0; i<queryTF_IDF.size(); i++) {
		if (queryTF_IDF.at(i) != 0) {
			noMatches = false;
		}
	}
	//code commented out below was for comparing to sample cases in project spec

	/*
	cout << "Query: " << query << endl;
	cout << "The tf-idf vector for this query is: ";
	for (int i = 0; i<queryTF_IDF.size()-1; i++) {
		cout << queryTF_IDF.at(i) << ", ";
	}
	cout << queryTF_IDF.at(queryTF_IDF.size()-1) << endl;
	cout << "The maximum angle this query achieves with any document is: " << maxComparison << endl;

	cout << "The returned result is: " << endl << endl;
	*/
	
	
	//if there are no matches in corpus/vocabular for query, output nothing. If there are matches, output 
	//  the most relevant document with words from query highlighted.
	if (!noMatches) {
		cout << outputOSS.str() << endl;
	}
	
	return 0;
}






