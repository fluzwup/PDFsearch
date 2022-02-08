# PDFsearch
A utility for extracting text from a set of PDF files and generating a keyword index as an HTML page with links to the PDF pages containing the words.

This project uses the poppler library (https://gitlab.freedesktop.org/poppler/poppler) for extracting the text from PDF files.  The program then uses the extracted text to generate an index in the form of an HTML document with links from the current directory to all PDF files index, with links to each page containing each word indexed.

Build the project, then generate the index file like this:

$ ./IndexPDF PDFs/*.pdf

Then you can open the Index.html file in a browser, jump to each inital letter or number, and search for terms found in the PDFs.

Todo:  
  Work on reducing the number of index terms; still too much noise.
