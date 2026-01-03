#include <stdio.h>

// Forward declarations
int matchFrom(char *regex, char *text);
int matchStar(int c, char *regex, char *text);

// Entry point for matching
int match(char *regex, char *text) {
  if (regex[0] == '^') {
    return matchFrom(regex + 1, text);
  }

  do {
    if (matchFrom(regex, text)) {
      return 1;
    }
  } while (*text++ != '\0');

  return 0;
}

// Match from start of regex and text
int matchFrom(char *regex, char *text) {
  if (regex[0] == '\0') return 1;

  if (regex[1] == '*') {
    return matchStar(regex[0], regex + 2, text);
  }

  if (regex[0] == '$' && regex[1] == '\0') {
    return *text == '\0';
  }

  if (*text != '\0' && (regex[0] == '.' || regex[0] == *text)) {
    return matchFrom(regex + 1, text + 1);
  }

  return 0;
}

// Handles character followed by '*'
int matchStar(int c, char *regex, char *text) {
  do {
    if (matchFrom(regex, text)) {
      return 1;
    }
  } while (*text != '\0' && (*text == c || c == '.') && text++);

  return 0;
}

int main() {
  // Testing
  printf("match(\"a*b\", \"aaab\") = %d\n", match("a*b", "aaab"));
  printf("match(\"^a.*b$\", \"acb\") = %d\n", match("^a.*b$", "acb"));
  printf("match(\"^a.*c$\", \"acb\") = %d\n", match("^a.*c$", "acb"));
  printf("match(\"abc\", \"123abc456\") = %d\n", match("abc", "123abc456"));
  printf("match(\"^abc\", \"123abc456\") = %d\n", match("^abc", "123abc456"));
}
