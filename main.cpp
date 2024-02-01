//for YandexPracticum from obegrand aka Andrey Melnikov
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        for (auto& word : words) {
            word_to_document_[word][document_id] += 1.0 / words.size();
        }
        ++document_counter_;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        vector<Document> matched_documents = FindAllDocuments(query_words);
        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    map<string, map<int, double>> word_to_document_;

    set<string> stop_words_;

    int document_counter_ = 0;

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (stop_words_.count(word) == 0) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query;
        for (string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-') {
                word = word.substr(1);
                if (stop_words_.count(word) == 0) {
                    query.minus_words.insert(word);
                }
            }
            query.plus_words.insert(word);
        }
        return query;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        map<int, double> document_to_relevance;
        for (const string& word : query_words.plus_words) {
            if (word_to_document_.count(word) == 0) { 
                continue;
            }
            for (const auto& [document_id, TF] : word_to_document_.at(word)) {
                double IDF = log(static_cast<double>(document_counter_) / word_to_document_.at(word).size());
                document_to_relevance[document_id] += IDF * TF;
            }
        }
        for (const string& word : query_words.minus_words) {
            if (word_to_document_.count(word) == 0) {
                continue;
            }
            for (const auto& [document_id, TF] : word_to_document_.at(word)) { 
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (auto& [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({ document_id, relevance });
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
    //system("pause");
}