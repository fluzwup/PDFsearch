#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <map>
#include <set>

#include <cpp/poppler-document.h>
#include <cpp/poppler-page.h>
#include <cpp/poppler-toc.h>

// poppler example text extraction code:  https://github.com/jeroen/popplertest/blob/master/encoding.cpp

// an initial set of common words that we will not index
std::set<std::string> commonWordsInitialSet = 
{
	"A", "ABLE", "ABOUT", "ABOVE", "ACCORDING", "ACCORDINGLY", "ACROSS", "ACTUALLY", "AFTER", "AFTERWARDS", 
	"AGAIN", "AGAINST", "ALL", "ALLOW", "ALLOWS", "ALMOST", "ALONE", "ALONG", "ALREADY", "ALSO", 
	"ALTHOUGH", "ALWAYS", "AM", "AMONG", "AMONGST", "AN", "AND", "ANOTHER", "ANY", "ANYBODY", 
	"ANYHOW", "ANYONE", "ANYTHING", "ANYWAY", "ANYWAYS", "ANYWHERE", "APART", "APPEAR", "APPRECIATE", "APPROPRIATE", 
	"ARE", "ARENT", "AROUND", "AS", "ASIDE", "ASK", "ASKING", "ASSOCIATED", "AT", "AVAILABLE", 
	"AWAY", "AWFULLY", "BE", "BECAME", "BECAUSE", "BECOME", "BECOMES", "BECOMING", "BEEN", "BEFORE", 
	"BEFOREHAND", "BEHIND", "BEING", "BELIEVE", "BELOW", "BESIDE", "BESIDES", "BEST", "BETTER", "BETWEEN", 
	"BEYOND", "BOTH", "BRIEF", "BUT", "BY", "CAME", "CAN", "CANT", "CANNOT", "CANT", 
	"CAUSE", "CAUSES", "CERTAIN", "CERTAINLY", "CHANGES", "CLEARLY", "CO", "COM", "COME", "COMES", 
	"CONCERNING", "CONSEQUENTLY", "CONSIDER", "CONSIDERING", "CONTAIN", "CONTAINING", "CONTAINS", "CORRESPONDING", "COULD", "COULDNT", 
	"COURSE", "CURRENTLY", "DEFINITELY", "DESCRIBED", "DESPITE", "DID", "DIDNT", "DIFFERENT", "DO", "DOES", 
	"DOESNT", "DOING", "DONT", "DONE", "DOWN", "DOWNWARDS", "DURING", "EACH", "EDU", "EG", 
	"EIGHT", "EITHER", "ELSE", "ELSEWHERE", "ENOUGH", "ENTIRELY", "ESPECIALLY", "ET", "ETC", "EVEN", 
	"EVER", "EVERY", "EVERYBODY", "EVERYONE", "EVERYTHING", "EVERYWHERE", "EX", "EXACTLY", "EXAMPLE", "EXCEPT", 
	"FAR", "FEW", "FIFTH", "FIRST", "FIVE", "FOLLOWED", "FOLLOWING", "FOLLOWS", "FOR", "FORMER", 
	"FORMERLY", "FORTH", "FOUR", "FROM", "FURTHER", "FURTHERMORE", "GET", "GETS", "GETTING", "GIVEN", 
	"GIVES", "GO", "GOES", "GOING", "GONE", "GOT", "GOTTEN", "GREETINGS", "HAD", "HADNT", 
	"HAPPENS", "HARDLY", "HAS", "HASNT", "HAVE", "HAVENT", "HAVING", "HE", "HES", "HELLO", 
	"HELP", "HENCE", "HER", "HERE", "HERES", "HEREAFTER", "HEREBY", "HEREIN", "HEREUPON", "HERS", 
	"HERSELF", "HI", "HIM", "HIMSELF", "HIS", "HITHER", "HOPEFULLY", "HOW", "HOWBEIT", "HOWEVER", 
	"ID", "ILL", "IM", "IVE", "IE", "IF", "IGNORED", "IMMEDIATE", "IN", "INASMUCH", 
	"INC", "INDEED", "INDICATE", "INDICATED", "INDICATES", "INNER", "INSOFAR", "INSTEAD", "INTO", "INWARD", 
	"IS", "ISNT", "IT", "ITD", "ITLL", "ITS", "ITS", "ITSELF", "JUST", "KEEP", 
	"KEEPS", "KEPT", "KNOW", "KNOWS", "KNOWN", "LAST", "LATELY", "LATER", "LATTER", "LATTERLY", 
	"LEAST", "LESS", "LEST", "LET", "LETS", "LIKE", "LIKED", "LIKELY", "LITTLE", "LOOK", 
	"LOOKING", "LOOKS", "LTD", "MAINLY", "MANY", "MAY", "MAYBE", "ME", "MEAN", "MEANWHILE", 
	"MERELY", "MIGHT", "MORE", "MOREOVER", "MOST", "MOSTLY", "MUCH", "MUST", "MY", "MYSELF", 
	"NAME", "NAMELY", "ND", "NEAR", "NEARLY", "NECESSARY", "NEED", "NEEDS", "NEITHER", "NEVER", 
	"NEVERTHELESS", "NEW", "NEXT", "NINE", "NO", "NOBODY", "NON", "NONE", "NOONE", "NOR", 
	"NORMALLY", "NOT", "NOTHING", "NOVEL", "NOW", "NOWHERE", "OBVIOUSLY", "OF", "OFF", "OFTEN", 
	"OH", "OK", "OKAY", "OLD", "ON", "ONCE", "ONE", "ONES", "ONLY", "ONTO", 
	"OR", "OTHER", "OTHERS", "OTHERWISE", "OUGHT", "OUR", "OURS", "OURSELVES", "OUT", "OUTSIDE", 
	"OVER", "OVERALL", "OWN", "PARTICULAR", "PARTICULARLY", "PER", "PERHAPS", "PLACED", "PLEASE", "PLUS", 
	"POSSIBLE", "PRESUMABLY", "PROBABLY", "PROVIDES", "QUE", "QUITE", "QV", "RATHER", "RD", "RE", 
	"REALLY", "REASONABLY", "REGARDING", "REGARDLESS", "REGARDS", "RELATIVELY", "RESPECTIVELY", "RIGHT", "SAID", "SAME", 
	"SAW", "SAY", "SAYING", "SAYS", "SECOND", "SECONDLY", "SEE", "SEEING", "SEEM", "SEEMED", 
	"SEEMING", "SEEMS", "SEEN", "SELF", "SELVES", "SENSIBLE", "SENT", "SERIOUS", "SERIOUSLY", "SEVEN", 
	"SEVERAL", "SHALL", "SHE", "SHOULD", "SHOULDNT", "SINCE", "SIX", "SO", "SOME", "SOMEBODY", 
	"SOMEHOW", "SOMEONE", "SOMETHING", "SOMETIME", "SOMETIMES", "SOMEWHAT", "SOMEWHERE", "SOON", "SORRY", "SPECIFIED", 
	"SPECIFY", "SPECIFYING", "STILL", "SUB", "SUCH", "SUP", "SURE", "TAKE", "TAKEN", "TELL", 
	"TENDS", "TH", "THAN", "THANK", "THANKS", "THANX", "THAT", "THATS", "THATS", "THE", 
	"THEIR", "THEIRS", "THEM", "THEMSELVES", "THEN", "THENCE", "THERE", "THERES", "THEREAFTER", "THEREBY", 
	"THEREFORE", "THEREIN", "THERES", "THEREUPON", "THESE", "THEY", "THEYD", "THEYLL", "THEYRE", "THEYVE", 
	"THINK", "THIRD", "THIS", "THOROUGH", "THOROUGHLY", "THOSE", "THOUGH", "THREE", "THROUGH", "THROUGHOUT", 
	"THRU", "THUS", "TO", "TOGETHER", "TOO", "TOOK", "TOWARD", "TOWARDS", "TRIED", "TRIES", 
	"TRULY", "TRY", "TRYING", "TWICE", "TWO", "UN", "UNDER", "UNFORTUNATELY", "UNLESS", "UNLIKELY", 
	"UNTIL", "UNTO", "UP", "UPON", "US", "USE", "USED", "USEFUL", "USES", "USING", 
	"USUALLY", "VALUE", "VARIOUS", "VERY", "VIA", "VIZ", "VS", "WANT", "WANTS", "WAS", 
	"WASNT", "WAY", "WE", "WED", "WELL", "WERE", "WEVE", "WELCOME", "WELL", "WENT", 
	"WERE", "WERENT", "WHAT", "WHATS", "WHATEVER", "WHEN", "WHENCE", "WHENEVER", "WHERE", "WHERES", 
	"WHEREAFTER", "WHEREAS", "WHEREBY", "WHEREIN", "WHEREUPON", "WHEREVER", "WHETHER", "WHICH", "WHILE", "WHITHER", 
	"WHO", "WHOS", "WHOEVER", "WHOLE", "WHOM", "WHOSE", "WHY", "WILL", "WILLING", "WISH", 
	"WITH", "WITHIN", "WITHOUT", "WONT", "WONDER", "WOULD", "WOULDNT", "YES", "YET", "YOU", 
	"YOUD", "YOULL", "YOURE", "YOUVE", "YOUR", "YOURS", "YOURSELF", "YOURSELVES", "ZERO"
};

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

// assume all caps by this point, include hex numbers
bool isNumeric(const std::string &s)
{
	bool allNumeric = true;
	for(char c : s)
	{
		if(!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
		{
			allNumeric = false;
			break;
		}
	}
	return allNumeric;
}

class Index
{
public:
	// identifier is the word in question, cleaned
	std::map<std::string, std::vector<WordLocations> > wordLocations;
	std::string filename;
	bool dirty;
	bool writeIfDirty;
	size_t documentsIndexed;
	size_t pagesIndexed;
	std::set<std::string> commonWords;

public:
	Index()
	{
		dirty = false;
		writeIfDirty = true;
		documentsIndexed = 0;
		pagesIndexed = 0;
		commonWords = commonWordsInitialSet;
	};
	~Index()
	{
		if(writeIfDirty && dirty && !filename.empty())
			WriteToFile(filename.c_str());
	};

	// TODO:  implememnt these so we can have a persistent, updateable index
	bool LoadFromFile(const char *filename)
	{
		return false;
		FILE *fp = fopen(filename, "r");
		if(fp == NULL) return false;


		fclose(fp);
		return true;
	};
	bool WriteToFile(const char *filename)
	{
		return false;
		FILE *fp = fopen(filename, "w");
		if(fp == NULL) return false;

		// print common word list, comma separated on a single line
		// IndexTerm1: "document 1 name.pdf":p1,p2,p3;"document 2 name.pdf":p1,p2,p3
		// IndexTerm2: "document 1 name.pdf":p1,p2,p3;"document 2 name.pdf":p1,p2,p3

		fclose(fp);
		return true;
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
	std::string CleanWord(const char *word)
	{
		std::string clean;
		size_t len = strlen(word);
		
		clean.reserve(len);
		for(size_t i = 0; i < len; ++i)
		{
			if(word[i] >= 'a' && word[i] <= 'z') clean += word[i] + ('A' - 'a');
			else if(word[i] >= 'A' && word[i] <= 'Z') clean += word[i];
			else if(word[i] >= '0' && word[i] <= '9') clean += word[i];
		}
		if(clean.length() < 2) return "";

		// filter out numeric values under 1000 and over 999,999,999
		if(isNumeric(clean) && (clean.size() < 4 || clean.size() >= 10))
			return "";
		
		if(commonWords.end() != commonWords.find(clean)) return "";

		return clean;
	}

	// go through and move words that appear very frequently out of the index and into the common words list
	int PurgeWords()
	{
		printf("Index size before purge:  %li\n", wordLocations.size());
		int count = 0;
		for(std::map<std::string, std::vector<WordLocations> >::iterator it = wordLocations.begin(); 
				it != wordLocations.end(); )
		{
			++count;
			if(it->second.size() > pagesIndexed)
			{
				printf("Adding %s to common words list\n", it->first.c_str());
				commonWords.insert(it->first);
				it = wordLocations.erase(it);
			}
			else if((isNumeric(it->first) && it->second.size() < documentsIndexed * 2)) 
			{ 	// a number found in too few locations
				it = wordLocations.erase(it);
			}
			else if(!isNumeric(it->first) && it->second.size() < 5)
			{ 	// a word that appears too few times
				it = wordLocations.erase(it);
			}
			else
				++it;
		}
		printf("Index size after purge:  %li count %i\n", wordLocations.size(), count);
		return wordLocations.size();
	};

	bool AddOccurrence(const char *word, const char *filename, unsigned int page, unsigned int line)
	{
		// clean word
		std::string clean = CleanWord(word);

		if(clean.empty()) return false;

		// add new file/page/line to list
		wordLocations[clean].push_back(WordLocations(filename, page, line));
		return true;
	};

};

bool AddToIndex(DocumentText &doctext, Index &index)
{
	unsigned int pageindex = 0;
	unsigned int lineindex = 0;
	++index.documentsIndexed;
	for(std::vector< std::vector< std::string > > &pgtxt : doctext.text)
	{
		++index.pagesIndexed;
		++pageindex;
		for(std::vector<std::string> &linetxt : pgtxt)
		{
			++lineindex;
			for(std::string &word : linetxt)
			{
				//printf("%s ", word.c_str());
				index.AddOccurrence(word.c_str(), doctext.filename.c_str(), pageindex, lineindex);
			}
		}
	}
	return true;
}

// if file pointer is not null, close out page first
// if filename is not null, open up new page, return open file pointer in fp
void NewFirstCharacter(FILE **fp, const char *filename, char character)
{
	if(*fp != NULL)
	{
		fprintf(*fp, "\n</html></body>\n");
		fclose(*fp);
		*fp = NULL;
	}

	if(filename != NULL)
	{
		*fp = fopen(filename, "w");
		fprintf(*fp, "<html><body>\n<title>%c</title><h1>%c</h1>\n", character, character);
	}
}

void PrintIndexEntry(FILE *fp, const char *word, std::vector<WordLocations> &vwl)
{
	fprintf(fp, "<p id=\"%s\"><h2>%s</h2></p>\n", word, word); 

	size_t lastpage = 0;
	std::string lastfile;
	for(WordLocations wl : vwl)
	{
		if(wl.filename != lastfile)
		{
			lastfile = wl.filename;
			lastpage = 0;
			fprintf(fp, "<br>\n  <B><A HREF=\"%s\">%s</A></B> : ", wl.filename.c_str(), wl.filename.c_str());
		}
		if(wl.page != lastpage)
		{
			lastpage = wl.page;
			fprintf(fp, "  <A HREF=\"%s#page=%i\">%i</A>, ", wl.filename.c_str(), wl.page, wl.page);
		}
	}

}

void PrintIndex(const char *filename, Index &index)
{
	FILE *fpLeaf = NULL;
	FILE *fpTop = fopen(filename, "w");

	char lastchar = 0;
	fprintf(fpTop, "<html><body>\n");
	for(int i = 0; i < 10; ++i)
		fprintf(fpTop, "<A HREF=\"#%i\">%i</A> ", i, i);
	fprintf(fpTop, "\n");
	for(int i = 'A'; i <= 'Z'; ++i)
		fprintf(fpTop, "<A HREF=\"#%c\">%c</A> ", i, i);
	fprintf(fpTop, "\n");

	bool firstLine = true;
	for(std::pair<std::string, std::vector<WordLocations> > wll : index.wordLocations)
	{
		// print keyword and link to page of links to that keyword in the PDFs
		if(wll.first[0] != lastchar)
		{
			// new first character, set up link from top index
			lastchar = wll.first[0];
			if(!firstLine) fprintf(fpTop, "<A HREF=\"#top\">Top</A><br>\n");

			fprintf(fpTop, "<p id=\"%c\">&bull;<A HREF=\"%s#%s\">%s</A>: %li locations</p>\n", 
					lastchar, 
					(wll.first.substr(0, 1) + ".html").c_str(), wll.first.c_str(), 
					wll.first.c_str(), wll.second.size());

			// open up new page for individual links
			std::string filename = wll.first.substr(0, 1) + ".html";
			NewFirstCharacter(&fpLeaf, filename.c_str(), wll.first[0]);
		}
		else
		{
			fprintf(fpTop, "<p>&bull;<A HREF=\"%s#%s\">%s</A>: %li locations</p>\n", 
					(wll.first.substr(0, 1) + ".html").c_str(), wll.first.c_str(),
					wll.first.c_str(), wll.second.size());
		}

		// create page with all the PDF links
		PrintIndexEntry(fpLeaf, wll.first.c_str(), wll.second);

		firstLine = false;
	}
	// close out leaf page
	NewFirstCharacter(&fpLeaf, NULL, ' ');

	fprintf(fpTop, "</html></body>\n");
	fclose(fpTop);
}

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		printf("Usage:  %s PDF [PDF [PDF ...]]\n\n", argv[0]);
		return 0;
	}

	Index index;
	
	int documentCount = 0;
	for(int i = 1; i < argc; ++i)
	{
		std::string filename = argv[i];

		DocumentText doctext;
		bool bret = ExtractPDFText(filename, doctext);

		if(bret) bret = AddToIndex(doctext, index);

		// if we successfully added a document, then increment the counter
		if(bret) ++documentCount;
	}
	index.PurgeWords();

	PrintIndex("Index.html", index);

	return 0;
}

