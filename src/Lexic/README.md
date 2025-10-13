# Lexic
## A Lexical Analyzer (Tokenizer)

---
### Using Lexic as a Library

[A Video Example](https://youtu.be/9clwgveTgrs)

#### Linking the Library
1. Use the Makefile at the root: `make`
2. All Libraries may be found within `_build/lib`

#### Using the code
Lexic first expects you to make a vocabulary which defines your language. However, construction is up to the developer/user:
```c
#include "lexic.h"

int main(void) {
    //Manual Build (hard coded to prevent alteration through external files)
    LexicVocabulary *manual_dict = LexicVocabularyAllocate();
    LexicVocabularyDefinitionAdd(manual_dict, "SingleDigit", "[0-9]");
    // . . .
    LexicVocabularyDefinitionAdd(manual_dict, "SingleAlpha", "\\S");
    
    //String Construction (easier hard coding, faster loading compared to manual, but harder to manage)
    LexicVocabulary *str_dict = LexicVocabularyFromString("Ident: [a-zA-Z_][a-zA-Z0-9_]*\nType: char|int");
    
    //printf("Successful String Load: %d\n", str_dict->def_count == 2); 
    //note this is illegal without including private/lexic_internal.h
    //however, I do not expect developers to need to include it

    //File Construction, uses String Construction inside (easier vocabulary debugging and editting)
    LexicVocabulary *file_dict = LexicVocabularyFromFile("example.vocab");
    
    //Finally Memory Management Freeing:
    LexicVocabularyFree(manual_dict); manual_dict = NULL;
    LexicVocabularyFree(str_dict);       str_dict = NULL;
    LexicVocabularyFree(file_dict);     file_dict = NULL;
}
```

And here is the format expected by the auto generation functions ``FromFile && FromString``:
```
one token per line
any line without a colon will be ignored
like so (consider these comments)

any lines with colons (:) will be evaluated as 
TOKEN_NAME: TOKEN_REGEX

note that the regex will be validated with a simple check to ensure that it is proper

this passes ->
IDENTIFIER: [a-zA-Z_][a-zA-Z_0-9]*

this does not ->
NULL: \

nor does this ->
BRCK: [

please make sure that you have a proper regex
```

Once you've defined your vocabulary, you can ask lexic to produce you some tokens using the following functions:
```c
#include "lexic.h"

int main(void) {
    LexicVocabulary *vocab = LexicVocabularyFromString("ALPHA: \\S+\nWSPACE: \\s+");
    
    //end of array delineated with stream[?].definition_name == NULL
    LexicToken *str_strm = LexicTokensFromString("\nword and whitespace", vocab);
    //str_strm[1].definition_name == "ALPHA"
    //str_strm[1].matching_input == "word"
    //str_strm[1].line == 2
    //str_strm[1].col == 1

    LexicToken *file_stream = LexicTokensFromFile("main.aws", vocab);
    //etc
 
    //Or you can use the stripped version which only returns token names
    //end of array delineated with stream[?] == NULL
    char ** str_nstrm = LexicTokenNamesFromString("\nword and whitespace", vocab);
    //str_nstrm[1] == str_strm[1].definition_name
    //etc

    char ** file_nstrm = LexicTokenNamesFromFile("main.aws", vocab);
    //etc

    //And finally memory management techniques

    LexicTokensFree(str_strm);   str_strm = NULL;
    LexicTokensFree(file_strm); file_strm = NULL;

    LexicTokenNamesFree(str_nstrm);   str_nstrm = NULL;
    LexicTokenNamesFree(file_nstrm); file_nstrm = NULL;
}
```

#### Single Lookahead
Lexic only has a single lookahead, which should be sufficient for most languages. But it's important to consider your regexes.

For example, `-?[0-9]+(\.[0-9]+)?` may seem like a reasonable regex to hand Lexic for any and all numbers. However, because of the lookahead a number such as '-5.6' will be parsed as two separate numbers: '-5' and '6', the '.' will be dropped.

Why is that? Well, as stated before, Lexic only has a single lookahead. As it's considering what Token to produce, it will consume up to "-5" and say, ok this matches a number. It then looks ahead to see '-5.', according to the last half of the regex we provided, `(\.[0-9]+)?`, the regex will not consume the period because it's missing a number after the period. Therefore it will consume '-5' as a singular number and move on to produce more tokens. But it didn't look any more ahead to see the 6 which would've matched.

The regex only needs to be simplified: `-?[0-9]+\.?[0-9]*`, now "-5.6" will be completely consumed because '-5.' is a valid token.

If you'd like to prevent '-5.' to be tokenized, you could attempt to break up the possible numbers into integers and floats. Or you can always do a second token pass to clear illegal tokens. Up to your creativity really.

---
### Using the CLI Tool
Navigate to the examples folder in the root, and then build it. You may either `./lexic_cli` or `./lexic_cli file.cpp` to launch. Afterwards, it will allow you to create your own vocabulary if you didn't supply it with a file, and then it will tokenize strings you give it.

If you'd like to see how it was made you can look at [this](https://youtu.be/9clwgveTgrs).
