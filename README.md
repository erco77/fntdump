# fntdump
A tool to convert binary 8x16 .FNT files to/from 'ASCII art' text file dumps to allow editing glyphs in a text editor

Example usage:

    fntdump SOMEFONT.FNT SOMEFONT.TXT   -- convert font to an 'ascii art' .txt file
    vi SOMEFONT.TXT                     -- edit the 'ascii art' to make changes to the font's bitmap
    fntdump SOMEFONT.TXT NEWFONT.FNT    -- convert the 'ascii art' back into a new .fnt file
    
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
