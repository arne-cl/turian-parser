#
#   posclass.py
#
#   Group POS tags into POS classes.
#   Based upon penn2idm from I. Dan Melamed.
#   The only change is that "," and ":" are now part of the same POS
#   classes as "CC".
#
#   $Id: posclass.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from_tag = {
"CD": "::CD",
"LS": "::CD",

"CC": "::CJ",
",": "::CJ",
":": "::CJ",

#"--": "::EOP",
#",": "::EOP",
#":": "::EOP",

"DT": "::D",
"PDT": "::D",
"PRP$": "::D",
"WDT": "::D",

".": "::EOS",

"IN": "::IN",
"RP": "::IN",
"TO": "::IN",

"JJ": "::J",
"JJR": "::J",
"JJS": "::J",

"$": "::N",
"NN": "::N",
"NNS": "::N",

"NNP": "::NP",
"NNPS": "::NP",

"EX": "::P",
"POS": "::P",
"PRP": "::P",
"WP": "::P",
"WP$": "::P",

"RB": "::R",
"RBR": "::R",
"RBS": "::R",
"WRB": "::R",

"``": "::SCM",
"''": "::SCM",
'"': "::SCM",
"-LRB-": "::SCM",
"-RRB-": "::SCM",

"#": "::SYM",
"SYM": "::SYM",

"UH": "::UH",

"FW": "::UK",

"AUX": "::AUX",
"AUXG": "::AUX",

"VBG": "::VBG",

"VBN": "::VBN",

"MD": "::V",
"VB": "::V",
"VBD": "::V",
"VBP": "::V",
"VBZ": "::V",

"-NONE-": "::NONE"}

to_idx = {"::CD": 1, "::CJ": 2, "::D": 3, "::EOS": 5, "::IN": 6, "::J": 7, "::N": 8, "::NONE": 9, "::NP": 10, "::P": 11, "::R": 12, "::SCM": 13, "::SYM": 14, "::UH": 15, "::UK": 16, "::V": 17, "::VBG": 18, "::VBN": 19, "::AUX": 20}
#to_idx = {"::CD": 1, "::CJ": 2, "::D": 3, "::EOP": 4, "::EOS": 5, "::IN": 6, "::J": 7, "::N": 8, "::NONE": 9, "::NP": 10, "::P": 11, "::R": 12, "::SCM": 13, "::SYM": 14, "::UH": 15, "::UK": 16, "::V": 17, "::VBG": 18, "::VBN": 19}
