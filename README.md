# fntdump
A tool to convert binary 8x16 .FNT files to/from 'ASCII art' text file dumps to allow editing glyphs in a text editor.

This should be able to build on both linux (tested) and MS-DOS (Turbo C 3).
Currently only 8x16 monospaced bitmap fonts are supported.
(I believe 8xN fonts are also supported, where N <= 16, since they seem to just be padded out to 8x16)

Example usage:

    fntdump SOMEFONT.FNT SOMEFONT.TXT   -- convert font to an 'ascii art' .txt file
    vi SOMEFONT.TXT                     -- edit the 'ascii art' .txt file to change font's bitmap
    fntdump SOMEFONT.TXT NEWFONT.FNT    -- convert the 'ascii art' .txt back into a new .fnt file
    
Basically you just change around the 'X's and '.'s to the way you want them, without
changing the length of the line, or the number of lines per character (must be 16).

The format of the 'ascii art' file is meant to be easy to edit in a text editor,
so you can change the glyphs around by simply changing the ascii content. Example:

    [--snip--]
    ........
    # character 01
    ........
    ........
    .XXXXXX.
    X......X
    X.X..X.X
    X......X
    X......X
    X.XXXX.X
    X..XX..X
    X......X
    X......X
    .XXXXXX.
    ........
    ........
    ........
    ........
    # character 02
    ........
    ........
    .XXXXXX.
    XXXXXXXX
    XX.XX.XX
    XXXXXXXX
    XXXXXXXX
    XX....XX
    XXX..XXX
    XXXXXXXX
    .XXXXXX.
    ........
    ........
    [--snip--]
