# JetBrains_Internship_Entry_task

This is a small UI app that searches a dictionary and finds matches. It is written in c++ using wxWidgets library.

### Implementation details.

The app uses `HelperThread` to search for the matches, so that searching doesn't affect UI thread and the application doesn't freeze.

The `HelperThread` sends results to main thread via `wxMessageQueue`.

The main thread displays the results in a `wxRichTextCtrl` (to support making parts of the text bold).

It adds the results to the screen periodically rather than as soon as a result is received, because adding the results to the screen is slow and would freeze the application due to many messages (and quite fast search).

To quickly search for the word the app uses a suffix array stored in file `words.suffixarray` (in binary format).



The app reads files as binary as it's faster.

The app's code supports both loading the files into memory and streaming them. Both ways seem to be quite fast. For that the following classes are used: 

`SuffixArray` and its derived classes - `InMemorySuffixArray` and `StreamingSuffixArray`

`WordDictionary` and its derived classes - `InMemoryWordDictionary` and `StreamingWordDictionary`


The suffix array was generated using a very simple but slow algorithm (a bit less than 3 minutes on my PC). This is the code as it's not in the repository:

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct Elem {
    string str;
    uint32_t offset = 0;
    bool operator<(const Elem& e){return this->str < e.str;}
};

uint32_t offsets[4396442];
int main()
{    
    ifstream f("words.txt");

    vector<Elem> elems;

    // read the elements
    size_t offset = 0;
    while (!f.eof()) {
        Elem e;
        getline(f, e.str);
        if (e.str.empty())
            break;
        e.offset = offset;
        offset += e.str.length() + 1;
        elems.push_back(e);
    }
   
    // add suffixes for each element
    size_t count = elems.size();
    for (size_t i = 0; i < count; i++) {
        Elem* e = &elems[i];
        while (e->str.length() > 1) {
            Elem newE;
            newE.str = e->str.substr(1);
            newE.offset = e->offset + 1;
            elems.push_back(newE);
            e = &elems[elems.size()-1];
        }
    }
    sort(elems.begin(), elems.end());

    FILE* out;
    fopen_s(&out, "words.suffixarray", "wb");
    // wirte the number of elements
    uint32_t totalSize = (uint32_t)elems.size();
    fwrite(&totalSize, sizeof(totalSize), 1, out);
    // write the elements
    for (const Elem& e : elems)
    {
        fwrite(&e.offset, sizeof(e.offset), 1, out);
    }   
    if (out != nullptr)
        fclose(out);
}
```
