/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/

#ifndef _JSTRING_H_
#define _JSTRING_H_

#include "standard.h"

#if !defined(_WIN32_WCE)
	#if !defined(CYGWIN)
		#include <sys/types.h>
		#include <sys/timeb.h>
	#endif
#endif
#ifdef WIN32
	#include <windows.h>
	#if !defined(CYGWIN)
		#include <tchar.h> 
	#endif
	#if !defined(_WIN32_WCE)
		#include <wchar.h> 
	#endif
#endif // WIN32
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Object.h"

namespace cmlabs {

class Collection;
class ObjectCollection;
class Dictionary;
class ObjectDictionary;
class DotString;
class JTime;
class Boolex;


class JString : public Object {
public:
	JString(const bool val);
	JString(const JString &value, unsigned int width, int justification);
	JString(unsigned int intValue, int padding = 0 );
	JString(int intValue, int padding = 0 );
	JString(long longValue, int padding = 0 );
	JString(double doubleValue , int afterdec = -1, int beforedec = -1);
	JString(const char *str = "");
	JString(char c);
	JString(const JString &str);
	JString(void* pointer);
	
	virtual ~JString();

	static JString format(char *formatstring, ...);

	Object* clone() const;
	JString print();
	unsigned long getHash() const;
	virtual bool unitTest();

	operator char*() const;
	operator const char*() const;
	operator void*();
	operator const void*();

	char* getCharCopy(int &len) const;
	DotString toDotString() const;
	JTime toTime();

	int operator ==(const char* chr) const;
	int operator !=(const char* chr) const;
	int operator ==(const JString &str) const;
	int operator !=(const JString &str) const;
	const JString& operator +=(const char*);
	const JString& operator +=(const char);
	const JString& operator +=(const unsigned char);
	const JString& operator =(const JString &str);
	const JString& operator +=(const JString &str);
	char operator [](unsigned int i) const;
	char& operator [](unsigned int i);
	char operator [](int i) const;
	char& operator [](int i);
	int operator <=(const JString &str) const;
	int operator >=(const JString &str) const;
	int operator < (const JString &str) const;
	int operator > (const JString &str) const;
	JString	operator +(const JString& str);
	JString	operator +(char* c);
	JString	operator +(const JString& str) const;
	JString	operator +(char* c) const;

	bool startsWith( const JString &prefix ) const;
	bool startsWith( const JString &prefix, unsigned int toffset ) const;
	bool startsWithIgnoreCase( const JString &prefix ) const;
	bool startsWithIgnoreCase( const JString &prefix, unsigned int toffset ) const;
	bool startsOrEndsWithWhiteSpace() const;
	bool startsWithWhiteSpace() const;
	bool endsWithWhiteSpace() const;
	char charAt(unsigned int i) const;
	char getCharAt( unsigned int i ) const;
	bool setCharAt( unsigned int i, char ch );
	int compare(const Object* o2) const;
	int compareTo( const JString &anotherString ) const;
	const char* cstr( ) const { return Buffer; }
	const char* c_str( ) const { return Buffer; }
	const char* charpoint() const;
	bool endsWith( const JString &suffix ) const;
	bool endsWithIgnoreCase( const JString &suffix ) const;
	virtual bool equals(const Object* o2) const;
	bool equals( const JString &anObject ) const;
	bool equalsIgnoreCase( const JString &anotherString ) const;
	int indexOf( char ch ) const;
	int indexOf( char ch, unsigned int fromIndex ) const;
	int indexOf( const JString &str ) const;
	int indexOf( const JString &str, unsigned int fromIndex ) const;
	int indexOfIgnoreCase( const JString &str ) const;
	int indexOfIgnoreCase( const JString &str, unsigned int fromIndex ) const;
	int lastIndexOf( char ch ) const;
	int lastIndexOf( char ch, unsigned int fromIndex ) const;
	int lastIndexOf( const JString &str ) const;
	int lastIndexOf( const JString &str, unsigned int fromIndex ) const;
	int lastIndexOfIgnoreCase( const JString &str ) const;
	int lastIndexOfIgnoreCase( const JString &str, unsigned int fromIndex ) const;
	JString invertString() const;
	const unsigned int length( ) const { return Length; }
	JString substring( unsigned int beginIndex ) const;
	JString substring( unsigned int beginIndex, unsigned int endIndex ) const;
	JString toLowerCase( ) const;
	JString toUpperCase( ) const;
	JString trim( ) const; 
	JString trimCTRL() const; 
	JString trimLines() const;
	JString trimLinesRemoveEmpty() const;
	JString concatLines() const;

	bool contains(const JString& str) const;
	bool containsIgnoreCase(const JString& str) const;
	JString indentXML(int numTabs = 1) const;
	JString indentHTML(int numTabs = 1) const;
	bool looksLikeXML() const;
	bool isUTF() const;
	JString addUTFHeader() const;
	JString toXML();
	bool fromXML(const JString& xml);
	JString getFirst(int len);
	JString getLast(int len);

	//! Splits a string on another string into an Collection
	Collection split(const JString& splitstr) const;

	//! Splits a string on another string, then split each into two, into a Dictionary
//	Dictionary JString::split(char* splitstr, char* equalstr);
	//! Splits a string on another string, then split each into two, into a Dictionary
	Dictionary split(const JString& splitstr, const JString& equalstr) const;

	//! Splits a string on whitespaces into an Collection
	Collection splitOnWhiteSpaces() const;

	//! Splits a string into lines of max width
	Collection splitLineOnWidth(int maxWidth) const;

	//! Parses a string like a command line into a Dictionary
	Dictionary splitCommandLine() const;

	//! Splits a string into an Collection of lines
	Collection splitIntoLines() const;

	//! Do not use, only for linux compatibility
//	Collection split2(JString splitstr);

	bool insert(JString str, int pos);
	bool removeChars(int count, int pos);

	bool matchesWithRegExp(const JString& regExp) const;
	bool matchMyRegExpWith(const JString& text) const;

	bool matches(const JString& match) const;
	bool matchesIgnoreCase(const JString& match) const;

	// Windows Specific Methods
	#ifdef WIN32
		LPWSTR lpwBuffer;
	//	#ifdef UNICODE
			JString(wchar_t wch);
			JString(LPWSTR lstr);
			// JString fromUnicode(LPTSTR lstr);
			operator LPWSTR();
			bool toUnicode();
			JString operator+(LPWSTR wstr);
			const JString& operator+=(LPWSTR wstr);
			JString operator+(wchar_t wch);
			const JString& operator+=(wchar_t wch);
	//	#endif // UNICODE
		operator const unsigned short*();
		bool utf2unicode();
		bool unicode2utf();
	#endif // WIN32
	

	void* toPointer() const;
	int toInt() const;
	long toLong() const;
	double toDouble() const;
	bool toBool() const;

	const JString& concat( const JString &str );
	void replace( char oldChar, char newChar);
	// int replace( char* match, char* replacement, bool all = true, bool recursive = false) {return replace(JString(match), JString(replacement), all, recursive); }
	int replace(const JString& match, const JString& replacement, bool all = true, bool recursive = false);
	void replace( int fromPos, int toPos, const JString& replacement);
	// int replaceIgnoreCase( char* match, char* replacement, bool all = true, bool recursive = false) {return replace(JString(match), JString(replacement), all, recursive); }
	int replaceIgnoreCase( const JString& match, const JString& replacement, bool all = true, bool recursive = false);
	//! Script loop replace using numbers %1, %2, ...
	int scriptReplace(const JString& tagname, ObjectCollection* replacements);
	//! Script loop replace using keys %key1, %key2, ...
	int scriptReplaceDictionary(const JString& tagname, ObjectCollection* replacements);
	//! Get scripted section from text
	JString scriptGetSection(const JString& tagname);
	//! Get scripted section parameters from text
	Dictionary scriptGetSectionParameters(const JString& tagname);
	//! Replace scripted section with text
	bool scriptReplaceSection(const JString& tagname, const JString& text);

	int indexOfCount(const JString& match) const;
	int indexOfCountIgnoreCase(const JString& match) const;

	JString removeCRs();
	JString replaceCRs();
	JString restoreCRs();
	JString toHTML() ;
	JString fromHTML();
	JString xmlStringEncode() const;
	JString xmlStringDecode() const;
	JString xml2html() const;

	bool isCRTerminated;
	bool addCRLF();

	// Utility functions...
	static JString bytifySize(long val);
	static JString bytifySize(double val);
	static JString bytifySizes(long val1, long val2);
	static JString bytifySizes(double val1, double val2);
	static JString bytifyRate(long val);
	static JString bytifyRate(double val);
	static JString bytifyRates(long val1, long val2);
	static JString bytifyRates(double val1, double val2);

protected:
	char *Buffer;
	unsigned int BufferLen;
	unsigned int Length;

	void ResizeBuffer(unsigned int MaxStrLen);
	void adjustBuffer(unsigned int len);
	void doubleBuffer();
};


class Boolex : public Object {
public:
	Boolex();
	Boolex(const JString& str);
	Boolex(XMLNode* node);
	virtual ~Boolex();

	Object* clone() const;
	JString print();
	bool equals(const Object* o2) const;
	JString toText() const;
	bool reset();

	JString toXML();
	bool fromXML(XMLNode* xml);

	bool setExpression(const JString& str);
	bool parseExpression(const JString& str);

	JString text;
	bool isNegated;
	Boolex* boolex;
	Collection* operators;
	ObjectCollection* boolexes;

	JString strAND;
	JString strOR;
	JString strXOR;
	JString strNOT;

protected:
	JString enforceParentheses(JString str);
	bool parseParentheses(const JString& str);

	bool isReservedWord(const JString& word);
	bool containsReservedWords(const JString& text);
};























#define BIT_CAN_MATCH_NOTHING          0x00
#define BIT_MORE_THAN_NULL             0x01
#define BIT_OPENS_ON_STAR_OR_PLUS      0x04
#define BIT_STAR_OR_PLUS_OK            0x02
//  Character values
#define CHAR_CLOSE_BRACKET    ']'
#define CHAR_CLOSE_PAREN      ')'
#define CHAR_DOLLAR           '$'
#define CHAR_DOUBLE_BSLASH    '\\'
#define CHAR_HOCH             '^'
#define CHAR_MINUS            '-'
#define CHAR_OPEN_BRACKET     '['
#define CHAR_OPEN_PAREN       '('
#define CHAR_OR               '|'
#define CHAR_PERIOD           '.'
#define CHAR_PLUS             '+'
#define CHAR_QUESTION         '?'
#define CHAR_STAR             '*'
#define CHAR_ZERO             '\0'
//  Pleasantly obscure macros
#define MAK_AS_UNSIGNED(p)          ((int)*(unsigned char *)(p))
#define MAK_CHAR_2_OPCODE(p)        (*(p))
#define MAK_CONTROLLING_OPERAND(p)  ((p)+3)
#define MAK_GET_NEXT(p)             (((*((p)+1)&0xFF)<<8)+(*((p)+2)&0xFF))
#define MAK_IS_COMPOUND(c)          ((c)==CHAR_STAR||(c)==CHAR_PLUS||(c)==CHAR_QUESTION)
//  Limitations
#define MAX_STORE             1024
#define MAX_SUB_EXPRESSIONS     10
// Opcodes used internally
#define OP_ANY                3
#define OP_ANYBUT             5
#define OP_ANYOF              4
#define OP_BACK               7
#define OP_BOL                1
#define OP_BRANCH             6
#define OP_CLOSE             30
#define OP_END                0
#define OP_EOL                2
#define OP_EXACT              8
#define OP_NMEMPTY            9
#define OP_OPEN              20
#define OP_PLUS              11
#define OP_STAR              10
#define SIGNATURE_VALUE    0x9C


class RegExpParser {
public:
    // query this element to learn if your regular expression
    // was compiled ok.
    int an_error_has_occured;
    // if the cGREP instance did not like something,
    // you can read all about it here.
    char error_string[  255 ];
public:
    // The single constructor requires a UNIX regular
    // expression.  It is ok to pass a constant.
    RegExpParser( char *exp );
    //  This method returns 1 if the regular expression
    //  was matched.  Otherwise it returns 0.
    int is_reg_exp_present( char *string );

private:
    //----------------------------------------------------------
    //------------  THE PRIVATE INTERFACE BEGINS HERE  ---------
    //----------------------------------------------------------
    char *reg_exp_match_begins_here[ MAX_SUB_EXPRESSIONS ];
    char *reg_exp_match_ends_here[ MAX_SUB_EXPRESSIONS ];
    char first_char_of_regexp;
    char is_anchored;
    char *required_substring;
    int length_required_substring;
    char symbol_string[ MAX_STORE ];
    char *Input_scan_pointer;
    int parentheses_count;
    char reg_exp_shadow;
    char *code_emit_pointer;
    long symbol_string_size;
    char *input_string_pointer;
    char *beginning_of_input;
    char **pointer_to_rembh;
    char **pointer_to_remeh;
    char *Special_chars;
    //----------------------------------------------------------
    //----------------------------------------------------------
    void record_error( int n );
    //----------------------------------------------------------
    char *parse_parens( int paren, int *flagp );
    //----------------------------------------------------------
    char *one_side_of_or_operator( int *flagp );
    //----------------------------------------------------------
    char *trailing_wild( int *flagp );
    //----------------------------------------------------------
    char *compress_ordinary_characters( int *flagp );
    //----------------------------------------------------------
    char *emit_node( char op );
    //----------------------------------------------------------
    void emit_next_byte( char b );
    //----------------------------------------------------------
    void insert_operator( char op, char *opnd );
    //----------------------------------------------------------
    void go_to_end_of_chain( char *p, char *val );
    //----------------------------------------------------------
    void go_to_end_of_chain_on_operand( char *p, char *val );
    //----------------------------------------------------------
    int lookup_engine( char *prog );
    //----------------------------------------------------------
    int wildcard_lookup( char *p );
    //----------------------------------------------------------
    char *get_next_pointer( char *p );
    //----------------------------------------------------------
    int evaluate_next( char *string );
};

} // namespace cmlabs

#endif // _JSTRING_H_





