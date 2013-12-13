/*

2012-02-20 minify.c basado en jsmin.c, Jorge Chamorro Bieling, Proyectos Equis Ka, s.l.

Minifica un .js, y lo convierte en una sentencia de C que fabrica una
string de C debidamente acabada en \0, con el nombre que se haya pasado en arg[1],
lista para ser guardada en un fichero del que se puede hacer un #include "fichero".


*/

/* jsmin.c
2012-01-09

Copyright (c) 2002 Douglas Crockford  (www.crockford.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>

static int   theA;
static int   theB;
static int   theLookahead = EOF;
static const char* hex= "0123456789abcdef";

/*

static int len;
static int first;
#define maxLen 80

static void put_hex (int c) {
  int lo= c & 0x000f;
  int hi= (c & 0x00f0) >> 4;

  if (first) {
    first= 0;
    fprintf(stdout, "  0x%c%c", hex[hi], hex[lo]);
    len+= 6;
  }
  else {
    len+= 6;
    if (len > maxLen) {
      len= 6;
      fprintf(stdout, ",\n  0x%c%c", hex[hi], hex[lo]);
    }
    else {
      fprintf(stdout, ", 0x%c%c", hex[hi], hex[lo]);
    }
  }
}
*/

/*
static void put_hex (int c) {
  if (c < ' ') {
    if (c == '\n') {
      fprintf(stdout, "\\n");
    }
    else if (c == '\t') {
      fprintf(stdout, "\\t");
    }
    else if (c == '\r') {
      fprintf(stdout, "\\r");
    }
    else {
      int lo= c & 0x000f;
      int hi= (c & 0x00f0) >> 4;
    
      fprintf(stdout, "\\x%c%c", hex[hi], hex[lo]);
    }
  }
  else if ((c == '\\') || (c == '"')) {
    fprintf(stdout, "\\%c", c);
  }
  else {
    putc(c, stdout);
  }
}
*/

/*

static void put_hex (int c) {
  int lo, hi;
  
  if (c == '\n') {
    fprintf(stdout, "\\n");
    lastCharWasHex= 0;
    return;
  }
  
  if (c == '\r') {
    fprintf(stdout, "\\r");
    lastCharWasHex= 0;
    return;
  }
  
  if (c == '\t') {
    fprintf(stdout, "\\t");
    lastCharWasHex= 0;
    return;
  }
  
  if ( (c == '"') || (c == '\\') || (c < 32) || (c > 126) ) {
    goto printHex;
  }
  
  if (lastCharWasHex && ( (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') )) {
    goto printHex;
  }
  
  putc(c, stdout);
  lastCharWasHex= 0;
  goto quit;
  
  printHex:
  lo= c & 0x000f;
  hi= (c & 0x00f0) >> 4;
  fprintf(stdout, "\\x%c%c", hex[hi], hex[lo]);
  lastCharWasHex= 1;
  
  quit:
  return;
}
*/


static void put_hex (int c) {
  if (c == '\n') {
    fputs("\\n", stdout);
  }
  else if (c == '\r') {
    fputs("\\r", stdout);
  }
  else if (c == '\t') {
    fputs("\\t", stdout);
  }
  else if (c == ' ') {
    putc(c, stdout);
  }
  else {
    int lo= c & 0x000f;
    int hi= (c & 0x00f0) >> 4;
    fprintf(stdout, "\\x%c%c", hex[hi], hex[lo]);
  }
}


/* isAlphanum -- return true if the character is a letter, digit, underscore,
dollar sign, or non-ASCII character.
*/
static int isAlphanum (int c) {
  return ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
    (c >= 'A' && c <= 'Z') || c == '_' || c == '$' || c == '\\' ||
    c > 126);
}


/* get -- return the next character from stdin. Watch out for lookahead. If
the character is a control character, translate it to a space or
linefeed.
*/

static int get() {
  int c = theLookahead;
  theLookahead = EOF;
  if (c == EOF) {
    c = getc(stdin);
  }
  if (c >= ' ' || c == '\n' || c == EOF) {
    return c;
  }
  if (c == '\r') {
    return '\n';
  }
  return ' ';
}


/* peek -- get the next character without getting it.
*/

static int peek () {
  theLookahead = get();
  return theLookahead;
}


/* next -- get the next character, excluding comments. peek() is used to see
if a '/' is followed by a '/' or '*'.
*/

static int next () {
  int c = get();
  if  (c == '/') {
    switch (peek()) {
      case '/':
      for (;;) {
        c = get();
        if (c <= '\n') {
          return c;
        }
      }
      case '*':
      get();
      for (;;) {
        switch (get()) {
          case '*':
          if (peek() == '/') {
            get();
            return ' ';
          }
          break;
          case EOF:
          fprintf(stderr, "Error: JSMIN Unterminated comment.\n");
          exit(1);
        }
      }
      default:
      return c;
    }
  }
  return c;
}


/* action -- do something! What you do is determined by the argument:
1   Output A. Copy B to A. Get the next B.
2   Copy B to A. Get the next B. (Delete A).
3   Get the next B. (Delete B).
action treats a string as a single character. Wow!
action recognizes a regular expression if it is preceded by ( or , or =.
*/

static void action (int d) {
  switch (d) {
    case 1:
    put_hex(theA);
    case 2:
    theA = theB;
    if (theA == '\'' || theA == '"' || theA == '`') {
      for (;;) {
        put_hex(theA);
        theA = get();
        if (theA == theB) {
          break;
        }
        if (theA == '\\') {
          put_hex(theA);
          theA = get();
        }
        if (theA == EOF) {
          fprintf(stderr, "Error: JSMIN unterminated string literal.");
          exit(1);
        }
      }
    }
    case 3:
    theB = next();
    if (theB == '/' && (theA == '(' || theA == ',' || theA == '=' ||
      theA == ':' || theA == '[' || theA == '!' ||
      theA == '&' || theA == '|' || theA == '?' ||
      theA == '{' || theA == '}' || theA == ';' ||
    theA == '\n')) {
      put_hex(theA);
      put_hex(theB);
      for (;;) {
        theA = get();
        if (theA == '[') {
          for (;;) {
            put_hex(theA);
            theA = get();
            if (theA == ']') {
              break;
            }
            if (theA == '\\') {
              put_hex(theA);
              theA = get();
            }
            if (theA == EOF) {
              fprintf(stderr,
                "Error: JSMIN unterminated set in Regular Expression literal.\n");
              exit(1);
            }
          }
          } else if (theA == '/') {
            break;
            } else if (theA =='\\') {
              put_hex(theA);
              theA = get();
            }
            if (theA == EOF) {
              fprintf(stderr,
                "Error: JSMIN unterminated Regular Expression literal.\n");
              exit(1);
            }
            put_hex(theA);
          }
          theB = next();
        }
      }
    }


/* jsmin -- Copy the input to the output, deleting the characters which are
insignificant to JavaScript. Comments will be removed. Tabs will be
replaced with spaces. Carriage returns will be replaced with linefeeds.
Most spaces and linefeeds will be removed.
*/

static void jsmin() {
  if (peek() == 0xEF) {
    get();
    get();
    get();
  }
  theA = '\n';
  action(3);
  while (theA != EOF) {
    switch (theA) {
      case ' ':
      if (isAlphanum(theB)) {
        action(1);
      } else {
        action(2);
      }
      break;
      case '\n':
      switch (theB) {
        case '{':
        case '[':
        case '(':
        case '+':
        case '-':
        action(1);
        break;
        case ' ':
        action(3);
        break;
        default:
        if (isAlphanum(theB)) {
          action(1);
        } else {
          action(2);
        }
      }
      break;
      default:
      switch (theB) {
        case ' ':
        if (isAlphanum(theA)) {
          action(1);
          break;
        }
        action(3);
        break;
        case '\n':
        switch (theA) {
          case '}':
          case ']':
          case ')':
          case '+':
          case '-':
          case '"':
          case '\'':
          case '`':
          action(1);
          break;
          default:
          if (isAlphanum(theA)) {
            action(1);
          } else {
            action(3);
          }
        }
        break;
        default:
        action(1);
        break;
      }
    }
  }
}


/* main -- Output any command line arguments as comments
and then minify the input.
*/
int main (int argc, char* argv[]) {
  if (argv[1] == NULL) exit(1);
  fprintf(stdout, "static const char* %s= \"", argv[1]);
  /*
  len= 0;
  first= 1;
  */
  jsmin();
  puts("\";");
  return 0;
}