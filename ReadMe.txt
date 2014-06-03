 sigExtract.exe extracts data from an image file based on a signature file
 Copyright (C) 2014  Johan 't Hart (Made for Digirec Data Recovery)

Usage: sigExtract -s <signature file> -i <input file> -o <output directory> [-v]

Use -v (verbose) to show the file names it is extracting

The signature file is a tab-separated file which currently enforces the
following format:

-----
type	header	footer	maxBytes	extension
txt	<WayPoint>	</WayPoint>	65535	xml
txt	*BEGIN*	*END*	65535	txt
hex	40 41 42 43	40 44 45 46	65535	hex.txt
-----

The header is ignored. Every row not beginning with txt or hex is ignored.
Columns must be in this order.

type:      Format of the header and footer
		     txt: in plain text
		     hex: Hex codes like 41 for A, 42 for B etc
header:    Start of a section to extract
footer:    End of a section to extract
maxBytes:  Stop when section is greater then these number of bytes
extension: Extension given to a file with this header/footer


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
	http://www.gnu.org/licenses/
