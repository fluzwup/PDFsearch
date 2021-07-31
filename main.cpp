#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <map>

#include <cpp/poppler-document.h>
#include <cpp/poppler-page.h>
#include <cpp/poppler-toc.h>

// poppler example text extraction code:  https://github.com/jeroen/popplertest/blob/master/encoding.cpp

// create HTML index, with links to each first letter, and links for each word that opens PDFs to the right page
// For example, this HTML tag opens page 4 of a PDF file named myfile.pdf:
// <A HREF="http://www.example.com/myfile.pdf#page=4">

struct DocumentText
{
	// filename is qualified, with the path relative to the index file
	std::string filename;
	// key and value in the map
	std::map<std::string, std::string> metadata;

	unsigned int pageCount;

	// text[page][line][word]
	std::vector< std::vector< std::vector< std::string > > > text;
};

// open up the given PDF, and fill out the DocumentText structure with its contents
bool ExtractPDFText(const std::string &filename, DocumentText &doctext)
{
	try
	{
		poppler::document * doc = poppler::document::load_from_file(filename, "", "");
		if(!doc)
			throw "Failed to read PDF file";

		// get document metadata by key
		for (std::string& key : doc->info_keys())
			doctext.metadata[key] = doc->info_key(key).to_latin1();

		// get page count
		doctext.pageCount = doc->pages();
		doctext.text.resize(doctext.pageCount);

		// iterate through pages, pulling text from each
		for(unsigned int pageindex = 0; pageindex < doctext.pageCount; ++pageindex)
		{
			poppler::page *pg(doc->create_page(pageindex));
			if(!pg)
				throw "Failed to open page";

			std::string pagetext = pg->text().to_latin1();

			// split text into lines and words
			unsigned int lineindex = 0; 
			char *linetok;
			char *linetext = strtok_r((char *)pagetext.c_str(), "\n", &linetok);
			while(linetext != NULL)
			{
				doctext.text[pageindex].resize(lineindex + 1);

				unsigned int wordindex = 0;
				char *wordtok;
				char *wordtext = strtok_r((char *)linetext, " ", &wordtok);
				while(wordtext != NULL)
				{
					doctext.text[pageindex][lineindex].push_back(wordtext);

					wordtext = strtok_r(NULL, " ", &wordtok);
					++wordindex;
				} 

				linetext = strtok_r(NULL, "\n", &linetok); 
				++lineindex;
			} 
		}
	}
	catch(const char *e)
	{
		printf("Caught exception:  %s\n", e);
		return false;
	}

	doctext.filename = filename;
	return true;
}

class WordLocations
{
public:
	WordLocations(const char *f, unsigned int p, unsigned int l)
	{
		filename = f;
		page = p;
		line = l;
	};
	std::string filename;
	unsigned int page;
	unsigned int line;
};

class Dictionary
{
public:
	// identifier is the word in question, cleaned
	std::map<std::string, std::vector<WordLocations> > wordLocations;
	std::string filename;
	bool dirty;
	bool writeIfDirty;

public:
	Dictionary()
	{
		dirty = false;
		writeIfDirty = true;
	};
	~Dictionary()
	{
		if(writeIfDirty && dirty && !filename.empty())
			WriteToFile(filename.c_str());
	};

	bool LoadFromFile(const char *filename)
	{
		return false;
	};
	bool WriteToFile(const char *filename)
	{
		return false;
	};

	bool AutoWrite()
	{
		return writeIfDirty;
	};
	void AutoWrite(bool newstate)
	{
		writeIfDirty = newstate;
	};
	
	// discard really short words and numbers, convert to upper case, filter out anything
	// but alphanumerics
	std::string Clean(const char *word)
	{
		std::string clean;
		size_t len = strlen(word);
		if(len <= 2) return "";

		clean.reserve(len);
		for(int i = 0; i < len; ++i)
		{
			if(word[i] >= 'a' && word[i] <= 'z') clean += word[i] + ('A' - 'a');
			else if(word[i] >= 'A' && word[i] <= 'Z') clean += word[i];
			else if(word[i] >= '0' && word[i] <= '9') clean += word[i];
		}
		if(clean.length() <= 2) return "";
		return clean;
	}

	bool AddOcurrence(const char *word, const char *filename, unsigned int page, unsigned int line)
	{
		// clean word
		std::string clean = Clean(word);

		if(clean.empty()) return false;

		// add new file/page/line to list
		wordLocations[clean].push_back(WordLocations(filename, page, line));
		return true;
	};

};

bool AddToDictionary(DocumentText &doctext, Dictionary &dict)
{
	unsigned int pageindex = 0;
	unsigned int lineindex = 0;
	for(std::vector< std::vector< std::string > > &pgtxt : doctext.text)
	{
		++pageindex;
		for(std::vector<std::string> &linetxt : pgtxt)
		{
			++lineindex;
			for(std::string &word : linetxt)
			{
				//printf("%s ", word.c_str());
				dict.AddOcurrence(word.c_str(), doctext.filename.c_str(), pageindex, lineindex);
			}
		}
	}
	return true;
}

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("Usage:  %s PDF [PDF [PDF ...]]\n\n", argv[0]);
		return 0;
	}

	Dictionary dict;
	
	for(int i = 1; i < argc; ++i)
	{
		std::string filename = argv[i];

		DocumentText doctext;
		bool bret = ExtractPDFText(filename, doctext);

		bret = AddToDictionary(doctext, dict);
	}

	char lastchar = 0;
	printf("<html><body>\n");
	for(int i = 0; i < 10; ++i)
		printf("<A HREF=\"#%i\">%i</A> ", i, i);
	printf("\n");
	for(int i = 'A'; i <= 'Z'; ++i)
		printf("<A HREF=\"#%c\">%c</A> ", i, i);
	printf("\n");
	for(std::pair<std::string, std::vector<WordLocations> > wll : dict.wordLocations)
	{
		// new first character
		if(wll.first[0] != lastchar)
		{
			lastchar = wll.first[0];
			printf("<p id=\"%c\">%s, %i locations</p>", lastchar, wll.first.c_str(), wll.second.size());
		}
		else
			printf("<p>%s, %i locations</p>", wll.first.c_str(), wll.second.size());
		size_t lastpage = 0;
		std::string lastfile;
		for(WordLocations wl : wll.second)
		{
			if(wl.filename != lastfile)
			{
				lastfile = wl.filename;
				lastpage = 0;
				printf("<br>\n  <A HREF=\"%s\">%s</A> : ", wl.filename.c_str(), wl.filename.c_str());
			}
			if(wl.page != lastpage)
			{
				lastpage = wl.page;
				printf("  <A HREF=\"%s#page=%i\">%i</A> ", wl.filename.c_str(), wl.page,
							wl.page);
			}
		}
		printf("<br><A HREF=\"#top\">Top</A><br>\n");
	}
	printf("</html></body>\n");

	return 0;
}
