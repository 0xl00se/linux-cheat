# Unicode

Maps numbers, known as code points, to characters (the idea, no specified font).

Does not specify how sequences of numbers should be encoded: this is typically done by UTF-8 and UTF-16.

Currently only 10% of the code points have been taken up, and since big languages like Chinese are already included, we don't have to worry about running out of space for a long time.

## Standards

Unicode Consortium that maintains the standard: <http://unicode.org/>

Latest version of the standard: <http://www.unicode.org/versions/latest/>

Code chart: browser by category or input as specific hex: <http://www.unicode.org/charts/>

## Plane

<http://en.wikipedia.org/wiki/Plane_(Unicode)>

There are 17 planes, 0 to 16, so not a multiple of 2! This is because of the limit of the UTF-16 encoding.

0 is filled and has the most important characters.

1 and 2 are used for less common scripts.

3 through 13 are not used yet.

14 and 15 are the private use area.

## Point notation

Points are noted as `U+` followed by an hexadecimal number, with leading zeros stripped.

Therefore:

- plane 0 is noted as `U+XXXX`,
- planes 1 - 9 are noted as: `U+XXXXX`
- planes nd 10 to 16 as `U+XXXXXX`

## Interesting characters

-   Emoticon set: <http://www.unicode.org/charts/PDF/U1F600.pdf>

-   Trigram for heaven `U+2630` ☰

    Similar to the popular "Menu" symbol found in many current applications. Interesting reuse for an ancient Chinese symbol!

-   Middle finger! Introduced in 2014.

    Amazing description: "Reversed hand with middle finger extended."

-   <http://en.wikipedia.org/wiki/Non-breaking_space#Width_variations>

    Unicode NBSP variants.

    Unicode also has other exotic spaces: <http://en.wikipedia.org/wiki/Whitespace_character>

## Replacement character

`U+FFFD` �

Commonly used when the software does have the glyph in the right font, or for invalid code points.

### Non-characters

<http://en.wikipedia.org/wiki/Universal_Character_Set_characters#Non-characters>

60 characters that are guaranteed never to be mapped to anything.

### Private use area

<http://en.wikipedia.org/wiki/Private_Use_Areas>

Planes 15 and 16.

## Tools

-   <http://shapecatcher.com/index.html>

    Unicode hand drawing to character recognizer. Seems to be down now, and closed source.

    Claims backend runs on CUDA!

# UTF-8

<http://en.wikipedia.org/wiki/UTF-8>

| Number of bits | 1        | 2        | 3        | 4        | 5        | 6        |
|----------------|----------|----------|----------|----------|----------|----------|
| 7              | 0xxxxxxx |          |          |          |          |          |
| 11             | 110xxxxx | 10xxxxxx |          |          |          |          |
| 16             | 1110xxxx | 10xxxxxx | 10xxxxxx |          |          |          |
| 21             | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |          |          |
| 26             | 111110xx | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |          |
| 31             | 1111110x | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |

Features:

- backwards compatible with ASCII
- easy to spot multi byte chars: if if starts with 1 it is part of a multi byte
- synchronization: easy to spot the start of a character: iff it starts with either `0` or `11` it is the start of a character.
- code length: given by the number of 1 bits on the leading byte

All of 5, 6 and many 4 byte characters don't map to any Unicode point to match the maximum number of characters encoded by Unicode.

## Overlong encoding

It is possible to encode many values in multiple ways, e.g., NUL can be coded as all of:

- `00000000`
- `11000000 10000000`
- `11100000 10000000 10000000`

UTF-8 only allows the shortest possible representation.

One possible use case for overlong representations is to overlong encode NUL as `C0F0`, so that the NUL character can be used as a string or file terminator. This works because there is no other valid 0 byte.
