#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

//
// fntdump - Edit a .FNT file's bitmap glyphs in a text editor
//
// This should build for both Unix and MS-DOS. 
// This code is GPL2 licensed (see COPYING that comes with the source).
//                                                       - erco@seriss.com
//

// A single character and it's 16 byte scanlines
typedef struct {
    char scan[16];
} Glyph;

char *G_self = 0;		// argv[0]

// Show the program's name followed by an error message and exit
void Error(char *msg)
{
    fprintf(stderr, "%s: %s", G_self, msg);
    exit(1);
}

// Show help and exit with optional 'unknown argument' message if 'badarg' is not NULL
void HelpAndExit(char *badarg)
{
    if ( badarg )
        fprintf(stderr, "%s: unknown argument '%s'\n\n", G_self, badarg);

    // 80 //////////////////////////////////////////////////////////////////////////
    fprintf(stderr,
    "fntdump - Edit a .FNT file's bitmap glyphs in a text editor\n"
    "\n"
    "    Use this tool to convert a .FNT file to/from an 'ASCII art' text file,\n"
    "    allowing you to edit font glyphs in any text editor (similar to XPMs).\n"
    "    .FNT files are used by MS-DOS and Linux as the console font bitmaps.\n"
    "\n"
    "USAGE\n"
    "    fntdump infile.fnt outfile.txt    # convert .fnt -> 'ASCII art' .txt\n"
    "    fntdump infile.txt outfile.fnt    # convert 'ASCII art' .txt -> .fnt\n"
    "    fntdump infile.fnt -              # dump .fnt to stdout as 'ASCII art'\n"
    "\n"
    "    NOTE: Filename extension on infile determines which conversion is done.\n"
    "\n"
    "OPTIONS\n"
    "    -h    -- help\n"
    "\n"
    "EXAMPLES\n"
    "    fntdump old-8x16.fnt old-8x16.txt\n"
    "    fntdump old-8x16.txt new-8x16.fnt\n"
    "    fntdump new-8x16.fnt - | more     # view ASCII art dump of font in 'more'\n"
    "\n"
    "AUTHOR/LICENSING\n"
    "    Greg Ercolano, Aug 16 2020 - Los Angeles, California\n"
    "    License: GPL2\n"
    "");
    exit(1);
}

// Strip off trailing \r or \n from string 's'
void strip_crlf(char *s)
{
    char *p;
    p = strchr(s, '\n'); if (p) *p = 0;
    p = strchr(s, '\r'); if (p) *p = 0;
}

// Read binary .fnt file, write ASCII dump to outfile
int Font2Txt(const char *infile, const char *outfile)
{
    int chr, row, bit;
    unsigned char mask;
    FILE *in, *out;
    Glyph glyph;

    // Open binary input file
    if ( (in = fopen(infile, "rb")) == NULL ) {
        fprintf(stderr, "%s: can't read '%s': %s\n",
	    G_self, infile, strerror(errno));
	return(1);
    }

    // Open output file
    if ( strcmp(outfile, "-") == 0 ) out = stdout;
    else if ( (out = fopen(outfile, "w")) == NULL ) {
        fprintf(stderr, "%s: can't write '%s': %s\n",
	    G_self, outfile, strerror(errno));
	return(1);
    }

    // For 8x16 fonts, simple .FNT files contain 256 chars,
    // 16 bytes of bitmap data per char to represent each char's glyph.
    //
    for ( chr=0; chr<256; chr++ ) {	// character loop
	// Character header
	fprintf(out, "# character %02x", chr);
	if ( chr>=0x20 && chr <= 0x7f ) fprintf(out, " '%c'\n", chr);
	else                            fprintf(out, "\n");

        // Read the glyph
        fread(&glyph, 16, 1, in);	  // read 16 scanline bytes for chr

	// Write 16 scanlines of ASCII dump
	for ( row=0; row<16; row++ ) {	  // write 16 row ASCII art per chr
	    // Character glyph as ASCII art
	    mask = 0x80;
	    for ( bit=0; bit<8; bit++ )	{ // bit loop
	        fprintf(out, "%c", (glyph.scan[row] & mask) ? 'X' : '.');
		mask >>= 1;		  // rotate bit mask left-to-right
	    }
	    fprintf(out, "\n");
	}
    }

    // Close, done
    fclose(in);
    if ( out != stdout ) fclose(out);
    return 0;
}

// Read ASCII font dump, write binary .FNT file
//
// Ascii file format: each scanline must contain 8 characters,
// each character either being '.' (clear bit) or 'X' (set bit).
//
//    # character 00     <-- optional comment line (ignored)
//    ........           <-- 1st scanline of first character
//    ........           <-- 2nd scanline of first character
//    ........
//    [..etc..]
//    ........
//    ........
//    ........           <-- 16th scanline of first character
//    # character 01     <-- optional comment line (ignored)
//    ........           <-- 1st scanline of SECOND character
//    ........           <-- etc..
//    .XXXXXX.
//    X......X
//    X.X..X.X
//    X......X
//    X......X
//    X.XXXX.X
//    X..XX..X
//    X......X
//    X......X
//    .XXXXXX.
//    ........
//    ........
//    ........
//    ........
//    [etc, etc... for 256 characters total, until end of file]
//
// NOTE: Specifying '-' for a .FNT outfile is probably a bad idea,
//       since MS-DOS stdout is text mode by default, and I don't
//       wanna mess with DOS setmode(1, O_BINARY)..
//
int Txt2Font(const char *infile, const char *outfile)
{
    int chr, row, bit, line;
    unsigned char byte;
    char s[256], *p;
    unsigned char mask;
    FILE *in, *out;

    // Open input file
    if ( (in = fopen(infile, "r")) == NULL ) {
        fprintf(stderr, "%s: can't read '%s': %s\n",
	    G_self, infile, strerror(errno));
	return(1);
    }

    // Open binary output file
    if ( strcmp(outfile, "-") == 0 ) out = stdout;
    else if ( (out = fopen(outfile, "wb")) == NULL ) {
        fprintf(stderr, "%s: can't write '%s': %s\n",
	    G_self, outfile, strerror(errno));
	return(1);
    }

    // Read the ASCII art file, and generate binary .FNT data
    // Assumes 8x16 fonts containing 256 chars, 16 bytes of bitmap
    // data per char to represent each char's glyph.
    //
    chr = 0;
    row = 0;
    line = 0;
    while ( fgets(s, 256, in) ) {	// read lines from ascii art .txt
	++line;				// keep count of lines (for errors)
	strip_crlf(s);
        // Ignore comment lines or stray blank lines 
	if ( s[0] == '\0' || s[0] == '#' ) continue;

	// Parse a line of 8 chars representing bits in byte
        mask = 0x80;
	byte = 0x00;
	for ( bit=0; bit<8; bit++ ) {
	    if ( s[bit] == '\0' ) break;	   // short line
	    byte |= (s[bit] == '.') ? 0x00 : mask; // apply bit
	    mask >>= 1; // rotate mask
	}
	fwrite(&byte, 1, 1, out);
	if ( bit < 8 ) {
	    fprintf(stderr,
		"%s (Line %d): expected 8 characters on line, got only %d: '%s'\n",
		infile, line, bit, s);
	    exit(1);
	}

	// Check for first row beyond last char
	if ( chr == 256 ) {
	    fprintf(stderr,
		"%s (Line %d): more than 256 characters of data at: '%s'\n",
		infile, line, s);
	    exit(1);
	}
	// Last row in this char? Keep count of chars
	if ( ++row == 16 ) {
	    row = 0;
	    ++chr;
	}
    }

    // Check for short file
    if ( chr != 256 || row != 0 ) {
	char emsg[80];
	sprintf(emsg, "input file was short: EOF while reading"
	              "row %d of chr# %d (0x%02x)\n", row, chr, chr);
	Error(emsg);
    }

    // Close, done
    fclose(in);
    fclose(out);
    return 0;
}

// 80 //////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    int t;
    char *infile = 0;
    char *outfile = 0;

    // Parse arguments
    G_self = argv[0];
    for ( t=1; t<argc; t++ ) {
        // Handle option flags
	if ( argv[t][0] == '-' && argv[t][1] ) {
	    switch (argv[t][1]) {
	        case 'h':		// help
		    HelpAndExit(0);
		default:		// unknown argument
		    HelpAndExit(argv[t]);
	    }
	}
	// Handle input/output filenames
        if ( !infile  ) { infile  = argv[t]; continue; }
        if ( !outfile ) { outfile = argv[t]; continue; }
    }

    // Sanity check
    if ( !infile  ) Error("missing input file (-h for help)\n");
    if ( !outfile ) Error("missing output file (-h for help)\n");

    // Load input, write output
    if ( strstr(infile, ".fnt") || strstr(infile, ".FNT") )
        return Font2Txt(infile, outfile);
    else
        return Txt2Font(infile, outfile);
}
